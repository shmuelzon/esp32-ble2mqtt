#include "httpd.h"
#include "httpd_static_files.h"
#include "ota.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

static const char *TAG = "HTTPD";

/* Internal state */
httpd_handle_t server = NULL;

/* Callback functions */
static httpd_on_ota_completed_cb_t on_ota_completed_cb = NULL;

void httpd_set_on_ota_completed_cb(httpd_on_ota_completed_cb_t cb)
{
    on_ota_completed_cb = cb;
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

    config.max_uri_handlers = 20;
    config.stack_size = 8192;
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    /* Register URI handlers */
    register_ota_routes(server);
    register_static_routes(server);

    return 0;
}
