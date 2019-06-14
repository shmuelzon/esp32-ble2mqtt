#ifndef MQTT_H
#define MQTT_H

#include <stddef.h>
#include <stdint.h>

/* Event callback types */
typedef void (*mqtt_on_connected_cb_t)(void);
typedef void (*mqtt_on_disconnected_cb_t)(void);
typedef void (*mqtt_on_message_received_cb_t)(const char *topic,
    const uint8_t *payload, size_t len, void *ctx);
typedef void (*mqtt_free_ctx_cb_t)(void *ctx);

/* Event handlers */
void mqtt_set_on_connected_cb(mqtt_on_connected_cb_t cb);
void mqtt_set_on_disconnected_cb(mqtt_on_disconnected_cb_t cb);

/* Pub/Sub */
int mqtt_subscribe(const char *topic, int qos, mqtt_on_message_received_cb_t cb,
    void *ctx, mqtt_free_ctx_cb_t free_cb);
int mqtt_unsubscribe(const char *topic);
int mqtt_publish(const char *topic, uint8_t *payload, size_t len, int qos,
    uint8_t retained);

int mqtt_connect(const char *host, uint16_t port, const char *client_id,
    const char *username, const char *password, uint8_t ssl,
    const char *server_cert, const char *client_cert, const char *client_key);
int mqtt_disconnect(void);

uint8_t mqtt_is_connected(void);

int mqtt_initialize(void);

#endif
