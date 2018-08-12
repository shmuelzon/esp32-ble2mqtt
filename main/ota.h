#ifndef OTA_H
#define OTA_H

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

/* Event handlers */
void ota_set_on_completed_cb(ota_on_completed_cb_t cb);

char *ota_err_to_str(ota_err_t err);
int ota_start(ota_type_t type, const char *url);
int ota_initialize(void);

#endif
