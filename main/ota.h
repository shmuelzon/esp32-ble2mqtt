#ifndef OTA_H
#define OTA_H

#include <stdint.h>
#include <stddef.h>

/* Types */
typedef enum {
    OTA_TYPE_FIRMWARE,
    OTA_TYPE_CONFIG,
} ota_type_t;

typedef enum {
    OTA_ERR_SUCCESS = 0,
    OTA_ERR_NO_CHANGE,
    OTA_ERR_IN_PROGRESS,
    OTA_ERR_FAILED_DOWNLOAD,
    OTA_ERR_FAILED_BEGIN,
    OTA_ERR_FAILED_WRITE,
    OTA_ERR_FAILED_END,
} ota_err_t;

/* Event callback types */
typedef void (*ota_on_completed_cb_t)(ota_type_t type, ota_err_t err);

int ota_download(ota_type_t type, const char *url, ota_on_completed_cb_t cb);

ota_err_t ota_open(ota_type_t type);
ota_err_t ota_write(uint8_t *data, size_t len);
ota_err_t ota_close(void);

char *ota_err_to_str(ota_err_t err);
int ota_initialize(void);

#endif
