#ifndef HTTPD_H
#define HTTPD_H

#include "ota.h"

/* Event callback types */
typedef void (*httpd_on_ota_completed_cb_t)(ota_type_t type, ota_err_t err);

/* Event handlers */
void httpd_set_on_ota_completed_cb(httpd_on_ota_completed_cb_t cb);

int httpd_initialize(void);

#endif
