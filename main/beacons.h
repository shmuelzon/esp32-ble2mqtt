#ifndef BEACONS_H
#define BEACONS_H

#include <stddef.h>
#include <stdint.h>

#define MAX_BEACON_NAME 16

/* Event callback types */
typedef void (*beacon_meta_data_cb_t)(char *name, char *val, void *ctx);

/* Types */
typedef struct {
    char name[MAX_BEACON_NAME];
    int (*is_beacon)(uint8_t *adv_data, size_t adv_data_len);
    void (*metadata_get)(uint8_t *adv_data, size_t adv_data_len, int rssi,
        beacon_meta_data_cb_t cb, void *ctx);
} beacon_ops_t;

beacon_ops_t *beacon_ops_get(uint8_t *adv_data, size_t adv_data_len);

#endif
