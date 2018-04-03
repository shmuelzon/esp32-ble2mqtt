#include "ota.h"
#include "config.h"
#include <stddef.h>
#include <esp_request.h>
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
    int (*begin)(void **handle);
    int (*write)(void *handle, uint8_t *data, size_t len);
    int (*end)(void *handle);
    char *(*version_get)(void);
} ota_ops_t;

/* Internal state */
static struct {
    char *url;
    ota_ops_t *ops;
    size_t bytes_written;
    void *handle;
} ota_ctx;

/* Callback functions */
static ota_on_completed_cb_t on_completed_cb = NULL;

void ota_set_on_completed_cb(ota_on_completed_cb_t cb)
{
    on_completed_cb = cb;
}

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
    return config_update_begin((config_update_handle_t *)handle);
}

static int ota_config_write(void *handle, uint8_t *data, size_t len)
{
    return config_update_write((config_update_handle_t)handle, data, len);
}

static int ota_config_end(void *handle)
{
    return config_update_end((config_update_handle_t)handle);
}

static char *ota_config_version_get(void)
{
    return config_version_get();
}

static ota_ops_t ota_config_ops = {
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

    ESP_LOGI(TAG, "Running partition type 0x%0x subtype 0x%0x (offset 0x%08x)",
        running->type, running->subtype, running->address);
    ESP_LOGI(TAG, "Writing partition type 0x%0x subtype 0x%0x (offset 0x%08x)",
        update->type, update->subtype, update->address);

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
        "0x%08x)", update->type, update->subtype, update->address);

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
    .begin = ota_firmware_begin,
    .write = ota_firmware_write,
    .end = ota_firmware_end,
    .version_get = ota_firmware_version_get,
};

static int download_callback(request_t *req, char *data, int len)
{
    if (!ota_ctx.bytes_written)
    {
        if (ota_ctx.ops->begin(&ota_ctx.handle))
            return OTA_ERR_FAILED_BEGIN;
    }

    if (ota_ctx.ops->write(ota_ctx.handle, (uint8_t *)data, len))
    {
        ESP_LOGE(TAG, "Failed writing data");
        return OTA_ERR_FAILED_WRITE;
    }
    ota_ctx.bytes_written += len;
    ESP_LOGI(TAG, "Wrote %d bytes (total: %d)", len, ota_ctx.bytes_written);

    return 0;
}

static void ota_task(void *pvParameter)
{
    request_t *req;
    char header[128];
    int http_status = 200;
    ota_err_t err = OTA_ERR_FAILED_DOWNLOAD;

    ESP_LOGI(TAG, "Starting OTA from %s", ota_ctx.url);
    req = req_new(ota_ctx.url);

    /* Set HTTP headers */
    req_setopt(req, REQ_FUNC_DOWNLOAD_CB, download_callback);
    sprintf(header, "User-Agent: BLE2MQTT/%s", BLE2MQTT_VER);
    req_setopt(req, REQ_SET_HEADER, header);
    sprintf(header, "If-None-Match: \"%s\"", ota_ctx.ops->version_get());
    req_setopt(req, REQ_SET_HEADER, header);

    /* Start HTTP request */
    http_status = req_perform(req);
    ESP_LOGI(TAG, "HTTP request response: %d, read %d (%d) bytes", http_status,
        req->buffer->bytes_total, ota_ctx.bytes_written);

    /* Call ops->end() only if we actually downloaded something */
    if (http_status == 200 && ota_ctx.bytes_written > 0)
    {
        err = ota_ctx.ops->end(ota_ctx.handle) ?
            OTA_ERR_FAILED_END : OTA_ERR_SUCCESS;
    }
    else if (http_status == 304)
        err = OTA_ERR_NO_CHANGE;

    if (on_completed_cb)
        on_completed_cb(err);

    free(ota_ctx.url);
    ota_ctx.url = NULL;
    req_clean(req);
    vTaskDelete(NULL);
}

int ota_start(ota_type_t type, const char *url)
{
    if (ota_ctx.url)
        return OTA_ERR_IN_PROGRESS;

    ota_ctx.ops = type == OTA_TYPE_FIRMWARE ?
        &ota_firmware_ops : &ota_config_ops;

    ota_ctx.url = strdup(url);
    ota_ctx.bytes_written = 0;

    xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);

    return 0;
}

int ota_initialize(void)
{
    ESP_LOGI(TAG, "Initializing OTA");
    return 0;
}
