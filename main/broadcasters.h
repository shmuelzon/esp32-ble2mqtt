#ifndef BROADCASTERS_H
#define BROADCASTERS_H

#include <stddef.h>
#include <stdint.h>

#define MAX_BROADCASTER_NAME 32

/* Event callback types */
typedef void (*broadcaster_meta_data_cb_t)(char *name, char *val, void *ctx);

/* Types */
typedef struct {
    char name[MAX_BROADCASTER_NAME];
    int (*is_broadcaster)(uint8_t *adv_data, size_t adv_data_len);
    void (*metadata_get)(uint8_t *adv_data, size_t adv_data_len, int rssi,
        broadcaster_meta_data_cb_t cb, void *ctx);
} broadcaster_ops_t;

broadcaster_ops_t *broadcaster_ops_get(uint8_t *adv_data, size_t adv_data_len);

#endif
