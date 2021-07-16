#include "httpd.h"
#include "httpd_static_files.h"
#include "ota.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <cJSON.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

static const char *TAG = "HTTPD";

/* Internal state */
httpd_handle_t server = NULL;

/* Callback functions */
static httpd_on_ota_completed_cb_t on_ota_completed_cb = NULL;

void httpd_set_on_ota_completed_cb(httpd_on_ota_completed_cb_t cb)
{
    on_ota_completed_cb = cb;
}

static void delayed_restart_timer_cb(TimerHandle_t xTimer)
{
    vTaskSuspendAll();
    esp_restart();
    xTaskResumeAll();
    xTimerDelete(xTimer, 0);
}

static esp_err_t restart_handler(httpd_req_t *req)
{
    TimerHandle_t delayed_restart_timer = xTimerCreate("delayed_restart",
        pdMS_TO_TICKS(1000), pdFALSE, NULL, delayed_restart_timer_cb);

    httpd_resp_sendstr(req, "OK");
    xTimerStart(delayed_restart_timer, 0);

    return ESP_OK;
}

static int register_management_routes(httpd_handle_t server)
{
    httpd_uri_t uri_restart = {
        .uri      = "/restart",
        .method   = HTTP_POST,
        .handler  = restart_handler,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server, &uri_restart);

    return 0;
}

static void ota_delayed_reset_timer_cb(TimerHandle_t xTimer)
{
    if (!on_ota_completed_cb)
        return;

    on_ota_completed_cb((ota_type_t)pvTimerGetTimerID(xTimer), OTA_ERR_SUCCESS);
    xTimerDelete(xTimer, 0);
}

static esp_err_t ota_handler(httpd_req_t *req)
{
    ota_type_t ota_type = (ota_type_t)req->user_ctx;
    char buf[2048];
    int ret;
    size_t total_received = 0;
    TimerHandle_t delayed_reset_timer = NULL;

    ESP_LOGD(TAG, "Handling route for OTA type %d", ota_type);

    if ((ret = ota_open(ota_type)))
    {
        ESP_LOGE(TAG, "Failed starting OTA: %s", ota_err_to_str(ret));
        return httpd_resp_send_500(req);
    }
    while (req->content_len - total_received > 0)
    {
        if ((ret = httpd_req_recv(req, buf, 2048)) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                continue;
            break;
        }
        total_received += ret;
        if ((ret = ota_write((uint8_t *)buf, ret)))
        {
            ESP_LOGE(TAG, "Failed writing OTA: %s", ota_err_to_str(ret));
            break;
        }
    }
    if ((ret = ota_close()))
    {
        ESP_LOGE(TAG, "Failed completing OTA: %s", ota_err_to_str(ret));
        return httpd_resp_send_500(req);
    }
    httpd_resp_sendstr(req, "OK");

    delayed_reset_timer = xTimerCreate("delayed_reset", pdMS_TO_TICKS(1000),
        pdFALSE, (void *)ota_type, ota_delayed_reset_timer_cb);
    xTimerStart(delayed_reset_timer, 0);

    return ESP_OK;
}

static int register_ota_routes(httpd_handle_t server)
{
    httpd_uri_t uri_ota = {
        .uri      = NULL,
        .method   = HTTP_POST,
        .handler  = ota_handler,
        .user_ctx = NULL,
    };

    uri_ota.uri = "/ota/firmware";
    uri_ota.user_ctx = (void *)OTA_TYPE_FIRMWARE;
    httpd_register_uri_handler(server, &uri_ota);
    uri_ota.uri = "/ota/configuration";
    uri_ota.user_ctx = (void *)OTA_TYPE_CONFIG;
    httpd_register_uri_handler(server, &uri_ota);

    return 0;
}

static void fs_add_directory_to_response(cJSON *response, const char *name)
{
    int i, size = cJSON_GetArraySize(response);
    cJSON *object;

    for (i = 0; i < size; i++)
    {
        cJSON *entry = cJSON_GetArrayItem(response, i);
        if (!strcmp(cJSON_GetObjectItem(entry, "name")->valuestring, name) &&
            !strcmp(cJSON_GetObjectItem(entry, "type")->valuestring,
            "directory"))
        {
            return;
        }
    }

    ESP_LOGD(TAG, "Adding directory %s", name);
    object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "type", "directory");
    cJSON_AddStringToObject(object, "name", name);
    cJSON_AddItemToArray(response, object);
}

static void fs_add_file_to_response(cJSON *response, const char *full_name,
    const char *name)
{
    char full_path[PATH_MAX];
    struct stat st;
    cJSON *object;

    snprintf(full_path, sizeof(full_path), "/spiffs/%s", full_name);
    if (stat(full_path, &st))
        return;

    ESP_LOGD(TAG, "Adding file %s", name);
    object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "type", "file");
    cJSON_AddStringToObject(object, "name", name);
    cJSON_AddNumberToObject(object, "size", st.st_size);
    cJSON_AddItemToArray(response, object);
}

static esp_err_t fs_serve_directory(httpd_req_t *req, const char *path)
{
    DIR *dir;
    struct dirent *entry;
    esp_err_t ret;
    cJSON *response;
    char *response_str;
    size_t path_len = strlen(path);

    ESP_LOGD(TAG, "Serving file list of: %s", path );

    if (!(dir = opendir("/spiffs")))
        return httpd_resp_send_500(req);

    response = cJSON_CreateArray();
    while((entry = readdir(dir)))
    {
        char *slash_location, *relative_name;

        ESP_LOGD(TAG, "Found entry %s", entry->d_name);

        /* Check if path is under this directory */
        if (strncmp(path + 1, entry->d_name, path_len - 1))
            continue;

        relative_name = strdup(entry->d_name + path_len - 1);

        if ((slash_location = strchr(relative_name, '/')))
        {
            *slash_location = '\0';
            fs_add_directory_to_response(response, relative_name);
        }
        else
            fs_add_file_to_response(response, entry->d_name, relative_name);

        free(relative_name);
    }
    closedir(dir);

    if (!cJSON_GetArraySize(response))
    {
        cJSON_Delete(response);
        return httpd_resp_send_404(req);
    }

    response_str = cJSON_PrintUnformatted(response);
    httpd_resp_set_type(req, "application/json");
    ret = httpd_resp_sendstr(req, response_str);

    cJSON_free(response_str);
    cJSON_Delete(response);
    return ret;
}

static esp_err_t fs_serve_file(httpd_req_t *req, const char *path)
{
    char full_path[PATH_MAX], buffer[2048];
    struct stat st;
    int fd, len;

    snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
    ESP_LOGD(TAG, "Serving file %s", full_path);

    if (stat(full_path, &st))
        return httpd_resp_send_404(req);

    if ((fd = open(full_path, O_RDONLY)) < 0)
        return httpd_resp_send_500(req);

    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_send(req, NULL, st.st_size);
    while ((len = read(fd, buffer, sizeof(buffer))) > 0)
        httpd_send(req, buffer, len);

    close(fd);
    return len < 0 ? ESP_FAIL : ESP_OK;
}

static esp_err_t fs_get_handler(httpd_req_t *req)
{
    const char *path = req->uri + strlen("/fs");

    ESP_LOGD(TAG, "Handling GET for: '%s'", path);

    if (path[strlen(path) - 1] == '/')
        return fs_serve_directory(req, path);
    return fs_serve_file(req, path);
}

static esp_err_t fs_post_handler(httpd_req_t *req)
{
    const char *path = req->uri + strlen("/fs");
    char full_path[PATH_MAX], buffer[2048];
    int fd, ret = 0;
    size_t total_received = 0;

    ESP_LOGD(TAG, "Handling POST for: '%s'", path);

    if (path[strlen(path) - 1] == '/')
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, NULL);

    snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
    if ((fd = open(full_path, O_WRONLY | O_TRUNC | O_CREAT)) < 0)
    {
        ESP_LOGE(TAG, "Failed opening file: %d (%s)", errno, strerror(errno));
        return httpd_resp_send_500(req);
    }

    while (req->content_len - total_received > 0)
    {
        if ((ret = httpd_req_recv(req, buffer, 2048)) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                continue;
            break;
        }
        total_received += ret;
        if ((ret = write(fd, buffer, ret)) < 0)
        {
            ESP_LOGE(TAG, "Failed writing to file: %d (%s)", errno,
                strerror(errno));
            break;
        }
        ESP_LOGD(TAG, "Wrote %d bytes (%zu/%zu)", ret, total_received,
            req->content_len);
    }
    close(fd);

    if (ret < 0 || total_received != req->content_len)
    {
        ESP_LOGE(TAG, "Failed downloading file: %d (%s)", errno,
            strerror(errno));
        unlink(full_path);
        return httpd_resp_send_500(req);
    }

    return httpd_resp_sendstr(req, "OK");
}

static esp_err_t fs_delete_handler(httpd_req_t *req)
{
    const char *path = req->uri + strlen("/fs");
    char full_path[PATH_MAX];

    ESP_LOGD(TAG, "Handling DELETE for: '%s'", path);

    snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
    if (unlink(full_path))
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, NULL);

    return httpd_resp_sendstr(req, "OK");
}

static int register_fs_routes(httpd_handle_t server)
{
    httpd_uri_t uri_fs = {
        .uri      = "/fs/*",
        .method   = HTTP_GET,
        .handler  = NULL,
        .user_ctx = NULL,
    };

    uri_fs.method = HTTP_GET;
    uri_fs.handler = fs_get_handler;
    httpd_register_uri_handler(server, &uri_fs);

    uri_fs.method = HTTP_POST;
    uri_fs.handler = fs_post_handler;
    httpd_register_uri_handler(server, &uri_fs);

    uri_fs.method = HTTP_DELETE;
    uri_fs.handler = fs_delete_handler;
    httpd_register_uri_handler(server, &uri_fs);

    return 0;
}

static esp_err_t static_file_handler(httpd_req_t *req)
{
    httpd_static_file *static_file = (httpd_static_file *)req->user_ctx;

    ESP_LOGD(TAG, "Handling route for %s", static_file->path);
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)static_file->start,
        static_file->end - static_file->start);
}

static int register_static_routes(httpd_handle_t server)
{
    httpd_static_file *static_file;
    httpd_uri_t uri_static_file = {
        .uri      = NULL,
        .method   = HTTP_GET,
        .handler  = static_file_handler,
        .user_ctx = NULL,
    };

    for (static_file = httpd_static_files; static_file->path; static_file++)
    {
        ESP_LOGD(TAG, "Registerting route %s", static_file->path);
        uri_static_file.uri = static_file->path;
        uri_static_file.user_ctx = static_file;
        httpd_register_uri_handler(server, &uri_static_file);
    }

    return 0;
}

int httpd_initialize(void)
{
    ESP_LOGI(TAG, "Initializing HTTP server");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    config.max_uri_handlers = 20;
    config.stack_size = 8192;
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    /* Register URI handlers */
    register_management_routes(server);
    register_ota_routes(server);
    register_fs_routes(server);
    register_static_routes(server);

    return 0;
}
