#include "mqtt.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_mqtt.h>
#include <string.h>

/* Constants */
static const char *TAG = "MQTT";

/* Types */
typedef struct mqtt_subscription_t {
    struct mqtt_subscription_t *next;
    char *topic;
    mqtt_on_message_received_cb_t cb;
    void *ctx;
    mqtt_free_ctx_cb_t free_cb;
} mqtt_subscription_t;

/* Internal state */
static mqtt_subscription_t *subscription_list = NULL;
static uint8_t is_connected = 0;

/* Callback functions */
static mqtt_on_connected_cb_t on_connected_cb = NULL;
static mqtt_on_disconnected_cb_t on_disconnected_cb = NULL;

void mqtt_set_on_connected_cb(mqtt_on_connected_cb_t cb)
{
    on_connected_cb = cb;
}

void mqtt_set_on_disconnected_cb(mqtt_on_disconnected_cb_t cb)
{
    on_disconnected_cb = cb;
}

static mqtt_subscription_t *mqtt_subscription_add(mqtt_subscription_t **list,
    const char *topic, mqtt_on_message_received_cb_t cb, void *ctx,
    mqtt_free_ctx_cb_t free_cb)
{
    mqtt_subscription_t *sub, **cur;

    sub = malloc(sizeof(*sub));
    sub->next = NULL;
    sub->topic = strdup(topic);
    sub->cb = cb;
    sub->ctx = ctx;
    sub->free_cb = free_cb;

    for (cur = list; *cur; cur = &(*cur)->next);
    *cur = sub;

    return sub;
}

static void mqtt_subscription_free(mqtt_subscription_t *mqtt_subscription)
{
    if (mqtt_subscription->ctx && mqtt_subscription->free_cb)
        mqtt_subscription->free_cb(mqtt_subscription->ctx);
    free(mqtt_subscription->topic);
    free(mqtt_subscription);
}

static void mqtt_subscriptions_free(mqtt_subscription_t **list)
{
    mqtt_subscription_t *cur, **head = list;

    while (*list)
    {
        cur = *list;
        *list = cur->next;
        mqtt_subscription_free(cur);
    }
    *head = NULL;
}

static void mqtt_subscription_remove(mqtt_subscription_t **list,
    const char *topic)
{
    mqtt_subscription_t **cur, *tmp;
    size_t len = strlen(topic);

    for (cur = list; *cur; cur = &(*cur)->next)
    {
        if (!strncmp((*cur)->topic, topic, len))
            break;
    }

    if (!*cur)
        return;

    tmp = *cur;
    *cur = (*cur)->next;
    mqtt_subscription_free(tmp);
}

int mqtt_subscribe(const char *topic, int qos, mqtt_on_message_received_cb_t cb,
    void *ctx, mqtt_free_ctx_cb_t free_cb)
{
    if (!is_connected)
        return -1;

    ESP_LOGD(TAG, "Subscribing to %s", topic);
    mqtt_subscription_add(&subscription_list, topic, cb, ctx, free_cb);
    return esp_mqtt_subscribe(topic, qos) != true;
}

int mqtt_unsubscribe(const char *topic)
{
    ESP_LOGD(TAG, "Unsubscribing from %s", topic);
    mqtt_subscription_remove(&subscription_list, topic);

    if (!is_connected)
        return 0;

    return esp_mqtt_unsubscribe(topic);
}

int mqtt_publish(const char *topic, uint8_t *payload, size_t len, int qos,
    uint8_t retained)
{
    if (!is_connected)
        return -1;

    return esp_mqtt_publish(topic, payload, len, qos, retained) != true;
}

static void mqtt_status_cb(esp_mqtt_status_t status)
{
    switch (status) {
    case ESP_MQTT_STATUS_CONNECTED:
        ESP_LOGI(TAG, "MQTT client connected");
        is_connected = 1;
        if (on_connected_cb)
            on_connected_cb();
        break;
    case ESP_MQTT_STATUS_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT client disconnected");
        is_connected = 0;
        mqtt_subscriptions_free(&subscription_list);
        if (on_disconnected_cb)
            on_disconnected_cb();
        break;
    }
}

static void mqtt_message_cb(const char *topic, uint8_t *payload, size_t len)
{
    mqtt_subscription_t *cur;

    ESP_LOGD(TAG, "Recevied: %s => %s (%d)\n", topic, payload, (int)len);

    for (cur = subscription_list; cur; cur = cur->next)
    {
        /* TODO: Correctly match MQTT topics (i.e. support wildcards) */
        if (strcmp(cur->topic, topic))
            continue;

        cur->cb(topic, payload, len, cur->ctx);
    }
}

int mqtt_connect(const char *host, uint16_t port, const char *client_id,
    const char *username, const char *password)
{
    ESP_LOGI(TAG, "Connecting MQTT client");
    esp_mqtt_start(host, port, client_id, username, password);
    return 0;
}

int mqtt_disconnect(void)
{
    ESP_LOGI(TAG, "Disconnecting MQTT client");
    is_connected = 0;
    esp_mqtt_stop();
    return 0;
}

int mqtt_initialize(void)
{
    ESP_LOGD(TAG, "Initializing MQTT client");
    esp_mqtt_init(mqtt_status_cb, mqtt_message_cb, 256, 2000);
    return 0;
}
