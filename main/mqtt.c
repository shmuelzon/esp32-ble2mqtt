#include "mqtt.h"
#include "resolve.h"
#include <esp_err.h>
#include <esp_log.h>
#include <mqtt_client.h>
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

typedef struct mqtt_publications_t {
    struct mqtt_publications_t *next;
    char *topic;
    uint8_t *payload;
    size_t len;
    int qos;
    uint8_t retained;
} mqtt_publications_t;

/* Internal state */
static esp_mqtt_client_handle_t mqtt_handle = NULL;
static mqtt_subscription_t *subscription_list = NULL;
static mqtt_publications_t *publications_list = NULL;
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

    for (cur = list; *cur; cur = &(*cur)->next)
    {
        if (!strcmp((*cur)->topic, topic))
            break;
    }

    if (!*cur)
        return;

    tmp = *cur;
    *cur = (*cur)->next;
    mqtt_subscription_free(tmp);
}

static mqtt_publications_t *mqtt_publication_add(mqtt_publications_t **list,
    const char *topic, uint8_t *payload, size_t len, int qos, uint8_t retained)
{
    mqtt_publications_t *pub = malloc(sizeof(*pub));

    pub->topic = strdup(topic);
    pub->payload = malloc(len);
    memcpy(pub->payload, payload, len);
    pub->len = len;
    pub->qos = qos;
    pub->retained = retained;

    pub->next = *list;
    *list = pub;

    return pub;
}

static void mqtt_publication_free(mqtt_publications_t *mqtt_publication)
{
    free(mqtt_publication->topic);
    free(mqtt_publication->payload);
    free(mqtt_publication);
}

static void mqtt_publications_free(mqtt_publications_t **list)
{
    mqtt_publications_t *cur, **head = list;

    while (*list)
    {
        cur = *list;
        *list = cur->next;
        mqtt_publication_free(cur);
    }
    *head = NULL;
}

static void mqtt_publications_publish(mqtt_publications_t *list)
{
    for (; list; list = list->next)
    {
        ESP_LOGI(TAG, "Publishing from queue: %s = %.*s", list->topic,
            list->len, list->payload);

        mqtt_publish(list->topic, list->payload, list->len, list->qos,
            list->retained);
    }
}

int mqtt_subscribe(const char *topic, int qos, mqtt_on_message_received_cb_t cb,
    void *ctx, mqtt_free_ctx_cb_t free_cb)
{
    if (!is_connected)
        return -1;

    ESP_LOGD(TAG, "Subscribing to %s", topic);
    if (esp_mqtt_client_subscribe(mqtt_handle, topic, qos) < 0)
    {
        ESP_LOGE(TAG, "Failed subscribing to %s", topic);
        return -1;
    }

    mqtt_subscription_add(&subscription_list, topic, cb, ctx, free_cb);
    return 0;
}

int mqtt_unsubscribe(const char *topic)
{
    ESP_LOGD(TAG, "Unsubscribing from %s", topic);
    mqtt_subscription_remove(&subscription_list, topic);

    if (!is_connected)
        return 0;

    return esp_mqtt_client_unsubscribe(mqtt_handle, topic);
}

int mqtt_publish(const char *topic, uint8_t *payload, size_t len, int qos,
    uint8_t retained)
{
    if (is_connected)
    {
        return esp_mqtt_client_publish(mqtt_handle, (char *)topic,
            (char *)payload, len, qos, retained) < 0;
    }

    /* If we're currently not connected, queue publication */
    ESP_LOGD(TAG, "MQTT is disconnected, adding publication to queue...");
    mqtt_publication_add(&publications_list, topic, payload, len, qos,
        retained);

    return 0;
}

static void mqtt_message_cb(const char *topic, size_t topic_len,
    uint8_t *payload, size_t len)
{
    mqtt_subscription_t *cur;

    ESP_LOGD(TAG, "Received: %.*s => %.*s (%d)\n", topic_len, topic, len,
        payload, (int)len);

    for (cur = subscription_list; cur; cur = cur->next)
    {
        /* TODO: Correctly match MQTT topics (i.e. support wildcards) */
        if (strncmp(cur->topic, topic, topic_len) ||
            cur->topic[topic_len] != '\0')
        {
            continue;
        }

        cur->cb(cur->topic, payload, len, cur->ctx);
    }
}

static esp_err_t mqtt_event_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT client connected");
        is_connected = 1;
        mqtt_publications_publish(publications_list);
        mqtt_publications_free(&publications_list);
        if (on_connected_cb)
            on_connected_cb();
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT client disconnected");
        is_connected = 0;
        mqtt_subscriptions_free(&subscription_list);
        if (on_disconnected_cb)
            on_disconnected_cb();
        break;
    case MQTT_EVENT_DATA:
        mqtt_message_cb(event->topic, event->topic_len, (uint8_t *)event->data,
            event->data_len);
        break;
    default:
        break;
    }

    return ESP_OK;
}

int mqtt_connect(const char *host, uint16_t port, const char *client_id,
    const char *username, const char *password, uint8_t ssl,
    const char *server_cert, const char *client_cert, const char *client_key)
{
    esp_mqtt_client_config_t config = {
        .event_handle = mqtt_event_cb,
        .host = resolve_host(host),
        .port = port,
        .client_id = client_id,
        .username = username,
        .password = password,
        .transport = ssl ? MQTT_TRANSPORT_OVER_SSL : MQTT_TRANSPORT_OVER_TCP,
        .cert_pem = server_cert,
        .client_cert_pem = client_cert,
        .client_key_pem = client_key,
    };

    ESP_LOGI(TAG, "Connecting MQTT client");
    if (mqtt_handle)
        esp_mqtt_client_destroy(mqtt_handle);
    if (!(mqtt_handle = esp_mqtt_client_init(&config)))
        return -1;
    esp_mqtt_client_start(mqtt_handle);
    return 0;
}

int mqtt_disconnect(void)
{
    ESP_LOGI(TAG, "Disconnecting MQTT client");
    is_connected = 0;
    mqtt_subscriptions_free(&subscription_list);
    if (mqtt_handle)
        esp_mqtt_client_destroy(mqtt_handle);
    mqtt_handle = NULL;

    return 0;
}

uint8_t mqtt_is_connected(void)
{
    return is_connected;
}

int mqtt_initialize(void)
{
    ESP_LOGD(TAG, "Initializing MQTT client");
    return 0;
}
