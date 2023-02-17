#include "ota.h"
#include "config.h"
#include <stddef.h>
#include <esp_http_client.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_ota_ops.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <string.h>

/* Constants */
static const char *TAG = "OTA";

/* Types */
typedef struct {
    ota_type_t type;
    int (*begin)(void **handle);
    int (*write)(void *handle, uint8_t *data, size_t len);
    int (*end)(void *handle);
    char *(*version_get)(void);
} ota_ops_t;

typedef struct {
    char *url;
    ota_on_completed_cb_t on_completed_cb;
} ota_download_ctx;

/* Internal state */
struct {
    int in_progress;
    ota_ops_t *ops;
    size_t bytes_written;
    void *handle;
} ota_ctx;

char *ota_err_to_str(ota_err_t err)
{
    switch (err)
    {
    case OTA_ERR_SUCCESS: return "Success";
    case OTA_ERR_NO_CHANGE: return "No change";
    case OTA_ERR_IN_PROGRESS: return "In progress";
    case OTA_ERR_FAILED_DOWNLOAD: return "Failed downloading file";
    case OTA_ERR_FAILED_BEGIN: return "Failed initializing OTA process";
    case OTA_ERR_FAILED_WRITE: return "Failed writing data";
    case OTA_ERR_FAILED_END: return "Failed finalizing OTA process";
    }

    return "Invalid OTA error";
}

/* Config OTA Wrappers */
static int ota_config_begin(void **handle)
{
    return config_update_begin((config_update_handle_t **)handle);
}

static int ota_config_write(void *handle, uint8_t *data, size_t len)
{
    return config_update_write((config_update_handle_t *)handle, data, len);
}

static int ota_config_end(void *handle)
{
    return config_update_end((config_update_handle_t *)handle);
}

static char *ota_config_version_get(void)
{
    return config_version_get();
}

static ota_ops_t ota_config_ops = {
    .type = OTA_TYPE_CONFIG,
    .begin = ota_config_begin,
    .write = ota_config_write,
    .end = ota_config_end,
    .version_get = ota_config_version_get,
};

/* Firmware OTA Wrappers */
static int ota_firmware_begin(void **handle)
{
    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();
    const esp_partition_t *update = NULL;
    esp_ota_handle_t update_handle = 0;
    esp_err_t err;

    if (configured != running)
    {
        ESP_LOGW(TAG, "Configured OTA boot partition is different than "
            "running partition");
    }

    if (!(update = esp_ota_get_next_update_partition(NULL)))
    {
        ESP_LOGE(TAG, "Failed getting update partition");
        return OTA_ERR_FAILED_BEGIN;
    }

    ESP_LOGI(TAG, "Running partition type 0x%0x subtype 0x%0x (offset 0x%08"
        PRIx32 ")", running->type, running->subtype, running->address);
    ESP_LOGI(TAG, "Writing partition type 0x%0x subtype 0x%0x (offset 0x%08"
        PRIx32 ")", update->type, update->subtype, update->address);

    err = esp_ota_begin(update, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed beginning OTA: 0x%x", err);
        return OTA_ERR_FAILED_BEGIN;
    }

    *handle = (void *)update_handle;

    return 0;
}

static int ota_firmware_write(void *handle, uint8_t *data, size_t len)
{
    esp_ota_handle_t update_handle = (esp_ota_handle_t)handle;
    esp_err_t err = esp_ota_write(update_handle, (const void *)data, len);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed writing OTA: 0x%x", err);
        return OTA_ERR_FAILED_WRITE;
    }

    return 0;
}

static int ota_firmware_end(void *handle)
{
    esp_ota_handle_t update_handle = (esp_ota_handle_t)handle;
    const esp_partition_t *update = esp_ota_get_next_update_partition(NULL);
    esp_err_t err = esp_ota_end(update_handle);

    if (!update || err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed ending OTA: 0x%x", err);
        return OTA_ERR_FAILED_END;
    }

    ESP_LOGI(TAG, "Setting boot partition type 0x%0x subtype 0x%0x (offset "
        "0x%08" PRIx32 ")", update->type, update->subtype, update->address);

    err = esp_ota_set_boot_partition(update);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed setting boot partition: 0x%x", err);
        return OTA_ERR_FAILED_END;
    }

    return 0;
}

static char *ota_firmware_version_get(void)
{
    return BLE2MQTT_VER;
}

static ota_ops_t ota_firmware_ops = {
    .type = OTA_TYPE_FIRMWARE,
    .begin = ota_firmware_begin,
    .write = ota_firmware_write,
    .end = ota_firmware_end,
    .version_get = ota_firmware_version_get,
};

static int http_event_cb(esp_http_client_event_t *event)
{
    if (event->event_id != HTTP_EVENT_ON_DATA)
        return ESP_OK;

    if (ota_write(event->data, event->data_len))
        return ESP_FAIL;

    return ESP_OK;
}

static void ota_task(void *pvParameter)
{
    ota_download_ctx *ctx = (ota_download_ctx *)pvParameter;
    esp_http_client_handle_t handle;
    char header[128];
    int http_status = -1;
    ota_err_t err;
    esp_http_client_config_t config = {
        .event_handler = http_event_cb,
        .method = HTTP_METHOD_GET,
        .url = ctx->url,
        .buffer_size = 2048,
    };

    ESP_LOGI(TAG, "Starting OTA from %s", ctx->url);
    handle = esp_http_client_init(&config);

    /* Set HTTP headers */
    sprintf(header, "BLE2MQTT/%s", BLE2MQTT_VER);
    esp_http_client_set_header(handle, "User-Agent", header);
    sprintf(header, "\"%s\"", ota_ctx.ops->version_get());
    esp_http_client_set_header(handle, "If-None-Match", header);

    /* Start HTTP request */
    if (esp_http_client_perform(handle) == ESP_OK)
        http_status = esp_http_client_get_status_code(handle);

    ESP_LOGI(TAG, "HTTP request response: %d, read %" PRId64 " (%zu) bytes",
        http_status, esp_http_client_get_content_length(handle),
        ota_ctx.bytes_written);

    err = ota_close();
    if (http_status != 200 && http_status != 304)
        err = OTA_ERR_FAILED_DOWNLOAD;

    if (ctx->on_completed_cb)
        ctx->on_completed_cb(ota_ctx.ops->type, err);

    free(ctx->url);
    free(ctx);
    esp_http_client_cleanup(handle);
    vTaskDelete(NULL);
}

int ota_download(ota_type_t type, const char *url, ota_on_completed_cb_t cb)
{
    int ret;
    ota_download_ctx *ctx;

    if ((ret = ota_open(type)))
        return ret;

    ctx = malloc(sizeof(*ctx));
    ctx->url = strdup(url);
    ctx->on_completed_cb = cb;

    xTaskCreatePinnedToCore(ota_task, "ota_task", 8192, ctx, 5, NULL, 1);

    return 0;
}

ota_err_t ota_open(ota_type_t type)
{
    if (ota_ctx.in_progress)
        return OTA_ERR_IN_PROGRESS;

    ota_ctx.ops = type == OTA_TYPE_FIRMWARE ?
        &ota_firmware_ops : &ota_config_ops;

    ota_ctx.in_progress = 1;
    ota_ctx.bytes_written = 0;

    return 0;
}

ota_err_t ota_write(uint8_t *data, size_t len)
{
    if (!ota_ctx.in_progress)
        return OTA_ERR_FAILED_WRITE;

    if (!ota_ctx.bytes_written)
    {
        if (ota_ctx.ops->begin(&ota_ctx.handle))
            return OTA_ERR_FAILED_BEGIN;
    }

    if (ota_ctx.ops->write(ota_ctx.handle, data, len))
    {
        ESP_LOGE(TAG, "Failed writing data");
        return OTA_ERR_FAILED_WRITE;
    }
    ota_ctx.bytes_written += len;
    ESP_LOGI(TAG, "Wrote %d bytes (total: %d)", len, ota_ctx.bytes_written);

    return 0;
}

ota_err_t ota_close(void)
{
    if (!ota_ctx.in_progress)
        return OTA_ERR_FAILED_END;

    ota_ctx.in_progress = 0;

    if (!ota_ctx.bytes_written)
        return OTA_ERR_NO_CHANGE;
    return ota_ctx.ops->end(ota_ctx.handle) ?
        OTA_ERR_FAILED_END : OTA_ERR_SUCCESS;
}

int ota_initialize(void)
{
    ESP_LOGI(TAG, "Initializing OTA");
    return 0;
}
