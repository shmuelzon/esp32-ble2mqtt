#include "config.h"
#include "broadcasters.h"
#include "ble.h"
#include "ble_utils.h"
#include "eth.h"
#include "log.h"
#include "mqtt.h"
#include "ota.h"
#include "resolve.h"
#include "wifi.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <mdns.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <string.h>

#define MAX_TOPIC_LEN 256
static const char *TAG = "BLE2MQTT";

typedef struct {
    mac_addr_t mac;
    ble_uuid_t service;
    ble_uuid_t characteristic;
} mqtt_ctx_t;

static char *device_name_get(void)
{
    static char name[14] = {};

    if (!*name)
    {
        uint8_t *mac = NULL;
        switch (config_network_type_get())
        {
        case NETWORK_TYPE_ETH:
            mac = eth_mac_get();
            break;
        case NETWORK_TYPE_WIFI:
            mac = wifi_mac_get();
            break;
        }
        sprintf(name, "BLE2MQTT-%02X%02X", mac[4], mac[5]);
    }

    return name;
}

/* Bookkeeping functions */
static void uptime_publish(void)
{
    char topic[MAX_TOPIC_LEN];
    char buf[16];

    /* Only publish uptime when connected, we don't want it to be queued */
    if (!mqtt_is_connected())
        return;

    /* Uptime (in seconds) */
    sprintf(buf, "%lld", esp_timer_get_time() / 1000 / 1000);
    snprintf(topic, MAX_TOPIC_LEN, "%s/Uptime", device_name_get());
    mqtt_publish(topic, (uint8_t *)buf, strlen(buf), config_mqtt_qos_get(),
        config_mqtt_retained_get());

    /* Free memory (in bytes) */
    sprintf(buf, "%u", esp_get_free_heap_size());
    snprintf(topic, MAX_TOPIC_LEN, "%s/FreeMemory", device_name_get());
    mqtt_publish(topic, (uint8_t *)buf, strlen(buf), config_mqtt_qos_get(),
        config_mqtt_retained_get());
}

static void self_publish(void)
{
    char topic[MAX_TOPIC_LEN];
    char *payload;

    /* App version */
    payload = BLE2MQTT_VER;
    snprintf(topic, MAX_TOPIC_LEN, "%s/Version", device_name_get());
    mqtt_publish(topic, (uint8_t *)payload, strlen(payload),
        config_mqtt_qos_get(), config_mqtt_retained_get());

    /* Config version */
    payload = config_version_get();
    snprintf(topic, MAX_TOPIC_LEN, "%s/ConfigVersion", device_name_get());
    mqtt_publish(topic, (uint8_t *)payload, strlen(payload),
        config_mqtt_qos_get(), config_mqtt_retained_get());

    uptime_publish();
}

/* OTA functions */
static void ota_on_completed(ota_type_t type, ota_err_t err)
{
    ESP_LOGI(TAG, "Update completed: %s", ota_err_to_str(err));

    /* All done, restart */
    if (err == OTA_ERR_SUCCESS)
    {
        if (type == OTA_TYPE_CONFIG)
            ble_clear_bonding_info();
        esp_restart();
    }
    else
        ble_scan_start();
}

static void ota_on_mqtt(const char *topic, const uint8_t *payload, size_t len,
    void *ctx)
{
    char *url = malloc(len + 1);
    ota_type_t type = (ota_type_t)ctx;
    ota_err_t err;

    memcpy(url, payload, len);
    url[len] = '\0';
    ESP_LOGI(TAG, "Starting %s update from %s",
        type == OTA_TYPE_FIRMWARE ? "firmware" : "configuration", url);

    if ((err = ota_start(type, url)) != OTA_ERR_SUCCESS)
        ESP_LOGE(TAG, "Failed updating: %s", ota_err_to_str(err));

    ble_disconnect_all();
    ble_scan_stop();
    free(url);
}

static void ota_subscribe(void)
{
    char topic[27];

    /* Register for both a specific topic for this device and a general one */
    sprintf(topic, "%s/OTA/Firmware", device_name_get());
    mqtt_subscribe(topic, 0, ota_on_mqtt, (void *)OTA_TYPE_FIRMWARE, NULL);
    mqtt_subscribe("BLE2MQTT/OTA/Firmware", 0, ota_on_mqtt,
        (void *)OTA_TYPE_FIRMWARE, NULL);

    sprintf(topic, "%s/OTA/Config", device_name_get());
    mqtt_subscribe(topic, 0, ota_on_mqtt, (void *)OTA_TYPE_CONFIG, NULL);
    mqtt_subscribe("BLE2MQTT/OTA/Config", 0, ota_on_mqtt,
        (void *)OTA_TYPE_CONFIG, NULL);
}

static void ota_unsubscribe(void)
{
    char topic[27];

    sprintf(topic, "%s/OTA/Firmware", device_name_get());
    mqtt_unsubscribe(topic);
    mqtt_unsubscribe("BLE2MQTT/OTA/Firmware");

    sprintf(topic, "%s/OTA/Config", device_name_get());
    mqtt_unsubscribe(topic);
    mqtt_unsubscribe("BLE2MQTT/OTA/Config");
}

static void cleanup(void)
{
    ble_disconnect_all();
    ble_scan_stop();
    ota_unsubscribe();
}

/* Network callback functions */
static void network_on_connected(void)
{
    log_start(config_log_host_get(), config_log_port_get());
    ESP_LOGI(TAG, "Connected to the network, connecting to MQTT");
    mqtt_connect(config_mqtt_host_get(), config_mqtt_port_get(),
        config_mqtt_client_id_get(), config_mqtt_username_get(),
        config_mqtt_password_get(), config_mqtt_ssl_get(),
        config_mqtt_server_cert_get(), config_mqtt_client_cert_get(),
        config_mqtt_client_key_get());
}

static void network_on_disconnected(void)
{
    log_stop();
    ESP_LOGI(TAG, "Disconnected from the network, stopping MQTT");
    mqtt_disconnect();
    /* We don't get notified when manually stopping MQTT */
    cleanup();
}

/* MQTT callback functions */
static void mqtt_on_connected(void)
{
    ESP_LOGI(TAG, "Connected to MQTT, scanning for BLE devices");
    self_publish();
    ota_subscribe();
    ble_scan_start();
}

static void mqtt_on_disconnected(void)
{
    ESP_LOGI(TAG, "Disconnected from MQTT, stopping BLE");
    cleanup();
}

/* BLE functions */
static void ble_on_mqtt_connected_cb(const char *topic, const uint8_t *payload,
    size_t len, void *ctx)
{
    char new_topic[MAX_TOPIC_LEN];

    if (len == 4 && !strncmp((char *)payload, "true", len))
        return;

    /* Someone published our device is disconnected, set them straight */
    snprintf(new_topic, MAX_TOPIC_LEN, "%s%s/Connected",
        config_mqtt_prefix_get(), (char *)ctx);
    mqtt_publish(new_topic, (uint8_t *)"true", 4, config_mqtt_qos_get(),
        config_mqtt_retained_get());
}

static void ble_publish_connected(mac_addr_t mac, uint8_t is_connected)
{
    char topic[MAX_TOPIC_LEN];

    snprintf(topic, MAX_TOPIC_LEN, "%s" MAC_FMT "/Connected",
        config_mqtt_prefix_get(), MAC_PARAM(mac));

    if (!is_connected)
        mqtt_unsubscribe(topic);

    mqtt_publish(topic, (uint8_t *)(is_connected ? "true" : "false"),
        is_connected ? 4 : 5, config_mqtt_qos_get(),
        config_mqtt_retained_get());

    if (is_connected)
    {
        /* Subscribe for other devices claiming this device is disconnected */
        mqtt_subscribe(topic, config_mqtt_qos_get(), ble_on_mqtt_connected_cb,
            strdup(mactoa(mac)), free);
        /* We are now the owner of this device */
        snprintf(topic, MAX_TOPIC_LEN, "%s" MAC_FMT "/Owner",
            config_mqtt_prefix_get(), MAC_PARAM(mac));
        mqtt_publish(topic, (uint8_t *)device_name_get(), 14,
            config_mqtt_qos_get(), config_mqtt_retained_get());
    }
}

static mqtt_ctx_t *ble_ctx_gen(mac_addr_t mac, ble_uuid_t service,
    ble_uuid_t characteristic)
{
    mqtt_ctx_t *ctx = malloc(sizeof(mqtt_ctx_t));

    memcpy(ctx->mac, mac, sizeof(mac_addr_t));
    memcpy(ctx->service, service, sizeof(ble_uuid_t));
    memcpy(ctx->characteristic, characteristic, sizeof(ble_uuid_t));

    return ctx;
}

/* BLE callback functions */
static void ble_on_broadcaster_metadata(char *name, char *val, void *ctx)
{
    char topic[MAX_TOPIC_LEN];

    sprintf(topic, "%s/%s/%s", device_name_get(), (char *)ctx, name);
    /* Broadcaster topics shouldn't be retained */
    mqtt_publish(topic, (uint8_t *)val, strlen(val), config_mqtt_qos_get(), 0);
}

static void ble_on_broadcaster_discovered(mac_addr_t mac, uint8_t *adv_data,
    size_t adv_data_len, int rssi, broadcaster_ops_t *ops)
{
    char *mac_str = strdup(mactoa(mac));
    char rssi_str[6];
    ESP_LOGI(TAG, "Discovered %s broadcaster", ops->name);

    ble_on_broadcaster_metadata("Type", ops->name, mac_str);
    sprintf(rssi_str, "%d", rssi);
    ble_on_broadcaster_metadata("RSSI", rssi_str, mac_str);
    ops->metadata_get(adv_data, adv_data_len, rssi, ble_on_broadcaster_metadata,
        mac_str);

    free(mac_str);
}

static void ble_on_device_discovered(mac_addr_t mac, int rssi)
{
    uint8_t connect = config_ble_should_connect(mactoa(mac));

    ESP_LOGI(TAG, "Discovered BLE device: " MAC_FMT " (RSSI: %d), %sconnecting",
        MAC_PARAM(mac), rssi, connect ? "" : "not ");

    if (!connect)
        return;

    ble_connect(mac);
}

static void ble_on_device_connected(mac_addr_t mac)
{
    ESP_LOGI(TAG, "Connected to device: " MAC_FMT ", scanning",
        MAC_PARAM(mac));
    ble_publish_connected(mac, 1);
    ble_services_scan(mac);
}

static char *ble_topic_suffix(char *base, uint8_t is_get)
{
    static char topic[MAX_TOPIC_LEN];

    sprintf(topic, "%s%s", base, is_get ? config_mqtt_get_suffix_get() :
        config_mqtt_set_suffix_get());

    return topic;
}

static char *ble_topic(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid)
{
    static char topic[MAX_TOPIC_LEN];
    int i;

    i = snprintf(topic, MAX_TOPIC_LEN, "%s" MAC_FMT "/%s",
        config_mqtt_prefix_get(), MAC_PARAM(mac),
        ble_service_name_get(service_uuid));
    snprintf(topic + i, MAX_TOPIC_LEN - i, "/%s",
        ble_characteristic_name_get(characteristic_uuid));

    return topic;
}

static void ble_on_characteristic_removed(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid, uint8_t properties)
{
    char *topic = ble_topic(mac, service_uuid, characteristic_uuid);

    if (properties & CHAR_PROP_READ)
        mqtt_unsubscribe(ble_topic_suffix(topic, 1));

    if (properties & CHAR_PROP_WRITE)
        mqtt_unsubscribe(ble_topic_suffix(topic, 0));

    if (properties & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE))
    {
        ble_characteristic_notify_unregister(mac, service_uuid,
            characteristic_uuid);
    }
}

static void ble_on_device_disconnected(mac_addr_t mac)
{
    ESP_LOGI(TAG, "Disconnected from device: " MAC_FMT, MAC_PARAM(mac));
    ble_publish_connected(mac, 0);
    ble_foreach_characteristic(mac, ble_on_characteristic_removed);
}

static void ble_on_mqtt_get(const char *topic, const uint8_t *payload,
    size_t len, void *ctx)
{
    ESP_LOGD(TAG, "Got read request: %s", topic);
    mqtt_ctx_t *data = (mqtt_ctx_t *)ctx;

    ble_characteristic_read(data->mac, data->service, data->characteristic);
}

static void ble_on_mqtt_set(const char *topic, const uint8_t *payload,
    size_t len, void *ctx)
{
    ESP_LOGD(TAG, "Got write request: %s, len: %u", topic, len);
    mqtt_ctx_t *data = (mqtt_ctx_t *)ctx;
    size_t buf_len;
    uint8_t *buf = atochar(data->characteristic, (const char *)payload,
        len, &buf_len);

    ble_characteristic_write(data->mac, data->service, data->characteristic,
        buf, buf_len);

    /* Issue a read request to get latest value */
    ble_characteristic_read(data->mac, data->service, data->characteristic);
}

static void ble_on_characteristic_found(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid, uint8_t properties)
{
    ESP_LOGD(TAG, "Found new characteristic: service: " UUID_FMT
      ", characteristic: " UUID_FMT ", properties: 0x%x",
      UUID_PARAM(service_uuid), UUID_PARAM(characteristic_uuid), properties);
    char *topic;

    if (!config_ble_service_should_include(uuidtoa(service_uuid)) ||
        !config_ble_characteristic_should_include(uuidtoa(characteristic_uuid)))
    {
        return;
    }

    topic = ble_topic(mac, service_uuid, characteristic_uuid);

    /* Characteristic is readable */
    if (properties & CHAR_PROP_READ)
    {
        mqtt_subscribe(ble_topic_suffix(topic, 1), config_mqtt_qos_get(),
            ble_on_mqtt_get, ble_ctx_gen(mac, service_uuid,
            characteristic_uuid), free);
        ble_characteristic_read(mac, service_uuid, characteristic_uuid);
    }

    /* Characteristic is writable */
    if (properties & (CHAR_PROP_WRITE | CHAR_PROP_WRITE_NR))
    {
        mqtt_subscribe(ble_topic_suffix(topic, 0), config_mqtt_qos_get(),
            ble_on_mqtt_set, ble_ctx_gen(mac, service_uuid,
            characteristic_uuid), free);
    }

    /* Characteristic can notify / indicate on changes */
    if (properties & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE))
    {
        ble_characteristic_notify_register(mac, service_uuid,
            characteristic_uuid);
    }
}

static void ble_on_device_services_discovered(mac_addr_t mac)
{
    ESP_LOGD(TAG, "Services discovered on device: " MAC_FMT, MAC_PARAM(mac));
    ble_foreach_characteristic(mac, ble_on_characteristic_found);
}

static void ble_on_device_characteristic_value(mac_addr_t mac,
    ble_uuid_t service, ble_uuid_t characteristic, uint8_t *value,
    size_t value_len)
{
    char *topic = ble_topic(mac, service, characteristic);
    char *payload = chartoa(characteristic, value, value_len);
    size_t payload_len = strlen(payload);

    ESP_LOGI(TAG, "Publishing: %s = %s", topic, payload);
    mqtt_publish(topic, (uint8_t *)payload, payload_len, config_mqtt_qos_get(),
        config_mqtt_retained_get());
}

static uint32_t ble_on_passkey_requested(mac_addr_t mac)
{
    char *s = mactoa(mac);
    uint32_t passkey = config_ble_passkey_get(s);

    ESP_LOGI(TAG, "Initiating pairing with %s using the passkey %u", s,
        passkey);

    return passkey;
}

/* BLE2MQTT Task and event callbacks */
typedef enum {
    EVENT_TYPE_HEARTBEAT_TIMER,
    EVENT_TYPE_NETWORK_CONNECTED,
    EVENT_TYPE_NETWORK_DISCONNECTED,
    EVENT_TYPE_OTA_COMPLETED,
    EVENT_TYPE_MQTT_CONNECTED,
    EVENT_TYPE_MQTT_DISCONNECTED,
    EVENT_TYPE_BLE_BROADCASTER_DISCOVERED,
    EVENT_TYPE_BLE_DEVICE_DISCOVERED,
    EVENT_TYPE_BLE_DEVICE_CONNECTED,
    EVENT_TYPE_BLE_DEVICE_DISCONNECTED,
    EVENT_TYPE_BLE_DEVICE_SERVICES_DISCOVERED,
    EVENT_TYPE_BLE_DEVICE_CHARACTERISTIC_VALUE,
} event_type_t;

typedef struct {
    event_type_t type;
    union {
        struct {
            ota_type_t type;
            ota_err_t err;
        } ota_completed;
        struct {
            mac_addr_t mac;
            uint8_t *adv_data;
            size_t adv_data_len;
            int rssi;
            broadcaster_ops_t *ops;
        } ble_broadcaster_discovered;
        struct {
            mac_addr_t mac;
            int rssi;
        } ble_device_discovered;
        struct {
            mac_addr_t mac;
        } ble_device_connected;
        struct {
            mac_addr_t mac;
        } ble_device_disconnected;
        struct {
            mac_addr_t mac;
        } ble_device_services_discovered;
        struct {
            mac_addr_t mac;
            ble_uuid_t service;
            ble_uuid_t characteristic;
            uint8_t *value;
            size_t value_len;
        } ble_device_characteristic_value;
    };
} event_t;

static QueueHandle_t event_queue;

static void ble2mqtt_handle_event(event_t *event)
{
    switch (event->type)
    {
    case EVENT_TYPE_HEARTBEAT_TIMER:
        uptime_publish();
        break;
    case EVENT_TYPE_NETWORK_CONNECTED:
        network_on_connected();
        break;
    case EVENT_TYPE_NETWORK_DISCONNECTED:
        network_on_disconnected();
        break;
    case EVENT_TYPE_OTA_COMPLETED:
        ota_on_completed(event->ota_completed.type, event->ota_completed.err);
        break;
    case EVENT_TYPE_MQTT_CONNECTED:
        mqtt_on_connected();
        break;
    case EVENT_TYPE_MQTT_DISCONNECTED:
        mqtt_on_disconnected();
        break;
    case EVENT_TYPE_BLE_BROADCASTER_DISCOVERED:
        ble_on_broadcaster_discovered(event->ble_broadcaster_discovered.mac,
            event->ble_broadcaster_discovered.adv_data,
            event->ble_broadcaster_discovered.adv_data_len,
            event->ble_broadcaster_discovered.rssi,
            event->ble_broadcaster_discovered.ops);
        free(event->ble_broadcaster_discovered.adv_data);
        break;
    case EVENT_TYPE_BLE_DEVICE_DISCOVERED:
        ble_on_device_discovered(event->ble_device_discovered.mac,
            event->ble_device_discovered.rssi);
        break;
    case EVENT_TYPE_BLE_DEVICE_CONNECTED:
        ble_on_device_connected(event->ble_device_connected.mac);
        break;
    case EVENT_TYPE_BLE_DEVICE_DISCONNECTED:
        ble_on_device_disconnected(event->ble_device_disconnected.mac);
        break;
    case EVENT_TYPE_BLE_DEVICE_SERVICES_DISCOVERED:
        ble_on_device_services_discovered(
            event->ble_device_services_discovered.mac);
        break;
    case EVENT_TYPE_BLE_DEVICE_CHARACTERISTIC_VALUE:
        ble_on_device_characteristic_value(
            event->ble_device_characteristic_value.mac,
            event->ble_device_characteristic_value.service,
            event->ble_device_characteristic_value.characteristic,
            event->ble_device_characteristic_value.value,
            event->ble_device_characteristic_value.value_len);
        free(event->ble_device_characteristic_value.value);
        break;
    }

    free(event);
}

static void ble2mqtt_task(void *pvParameter)
{
    event_t *event;

    while (1)
    {
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) != pdTRUE)
            continue;

        ble2mqtt_handle_event(event);
    }

    vTaskDelete(NULL);
}

static void heartbeat_timer_cb(TimerHandle_t xTimer)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_HEARTBEAT_TIMER;

    ESP_LOGD(TAG, "Queuing event HEARTBEAT_TIMER");
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static int start_ble2mqtt_task(void)
{
    TimerHandle_t hb_timer;

    if (!(event_queue = xQueueCreate(10, sizeof(event_t *))))
        return -1;

    if (xTaskCreatePinnedToCore(ble2mqtt_task, "ble2mqtt_task", 4096, NULL, 5,
        NULL, 1) != pdPASS)
    {
        return -1;
    }


    hb_timer = xTimerCreate("heartbeat", pdMS_TO_TICKS(60 * 1000), pdTRUE,
        NULL, heartbeat_timer_cb);
    xTimerStart(hb_timer, 0);

    return 0;
}

static void _network_on_connected(void)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_NETWORK_CONNECTED;

    ESP_LOGD(TAG, "Queuing event NETWORK_CONNECTED");
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _network_on_disconnected(void)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_NETWORK_DISCONNECTED;

    ESP_LOGD(TAG, "Queuing event NETWORK_DISCONNECTED");
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ota_on_completed(ota_type_t type, ota_err_t err)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_OTA_COMPLETED;
    event->ota_completed.type = type;
    event->ota_completed.err = err;

    ESP_LOGD(TAG, "Queuing event HEARTBEAT_TIMER (%d, %d)", type, err);
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _mqtt_on_connected(void)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_MQTT_CONNECTED;

    ESP_LOGD(TAG, "Queuing event MQTT_CONNECTED");
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _mqtt_on_disconnected(void)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_MQTT_DISCONNECTED;

    ESP_LOGD(TAG, "Queuing event MQTT_DISCONNECTED");
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ble_on_broadcaster_discovered(mac_addr_t mac, uint8_t *adv_data,
    size_t adv_data_len, int rssi, broadcaster_ops_t *ops)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_BLE_BROADCASTER_DISCOVERED;
    memcpy(event->ble_broadcaster_discovered.mac, mac, sizeof(mac_addr_t));
    event->ble_broadcaster_discovered.adv_data = malloc(adv_data_len);
    memcpy(event->ble_broadcaster_discovered.adv_data, adv_data, adv_data_len);
    event->ble_broadcaster_discovered.adv_data_len = adv_data_len;
    event->ble_broadcaster_discovered.rssi = rssi;
    event->ble_broadcaster_discovered.ops = ops;

    ESP_LOGD(TAG, "Queuing event BLE_BROADCASTER_DISCOVERED (" MAC_FMT ", "
        "%p, %u, %d)", MAC_PARAM(mac), adv_data, adv_data_len, rssi);
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ble_on_device_discovered(mac_addr_t mac, int rssi)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_BLE_DEVICE_DISCOVERED;
    memcpy(event->ble_device_discovered.mac, mac, sizeof(mac_addr_t));
    event->ble_device_discovered.rssi = rssi;

    ESP_LOGD(TAG, "Queuing event BLE_DEVICE_DISCOVERED (" MAC_FMT ", %d)",
        MAC_PARAM(mac), rssi);
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ble_on_device_connected(mac_addr_t mac)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_BLE_DEVICE_CONNECTED;
    memcpy(event->ble_device_connected.mac, mac, sizeof(mac_addr_t));

    ESP_LOGD(TAG, "Queuing event BLE_DEVICE_CONNECTED (" MAC_FMT ")",
        MAC_PARAM(mac));
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ble_on_device_disconnected(mac_addr_t mac)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_BLE_DEVICE_DISCONNECTED;
    memcpy(event->ble_device_disconnected.mac, mac, sizeof(mac_addr_t));

    ESP_LOGD(TAG, "Queuing event BLE_DEVICE_DISCONNECTED (" MAC_FMT ")",
        MAC_PARAM(mac));
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ble_on_device_services_discovered(mac_addr_t mac)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_BLE_DEVICE_SERVICES_DISCOVERED;
    memcpy(event->ble_device_services_discovered.mac, mac, sizeof(mac_addr_t));

    ESP_LOGD(TAG, "Queuing event BLE_DEVICE_SERVICES_DISCOVERED (" MAC_FMT ")",
        MAC_PARAM(mac));
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

static void _ble_on_device_characteristic_value(mac_addr_t mac,
    ble_uuid_t service, ble_uuid_t characteristic, uint8_t *value,
    size_t value_len)
{
    event_t *event = malloc(sizeof(*event));

    event->type = EVENT_TYPE_BLE_DEVICE_CHARACTERISTIC_VALUE;
    memcpy(event->ble_device_characteristic_value.mac, mac, sizeof(mac_addr_t));
    memcpy(event->ble_device_characteristic_value.service, service,
        sizeof(ble_uuid_t));
    memcpy(event->ble_device_characteristic_value.characteristic,
        characteristic, sizeof(ble_uuid_t));
    event->ble_device_characteristic_value.value = malloc(value_len);
    memcpy(event->ble_device_characteristic_value.value, value, value_len);
    event->ble_device_characteristic_value.value_len = value_len;

    ESP_LOGD(TAG, "Queuing event BLE_DEVICE_CHARACTERISTIC_VALUE (" MAC_FMT ", "
        UUID_FMT ", %p, %u)", MAC_PARAM(mac), UUID_PARAM(characteristic), value,
        value_len);
    xQueueSend(event_queue, &event, portMAX_DELAY);
}

void app_main()
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Version: %s", BLE2MQTT_VER);

    /* Init configuration */
    ESP_ERROR_CHECK(config_initialize());

    /* Init remote logging */
    ESP_ERROR_CHECK(log_initialize());

    /* Init OTA */
    ESP_ERROR_CHECK(ota_initialize());
    ota_set_on_completed_cb(_ota_on_completed);

    /* Init Network */
    switch (config_network_type_get())
    {
    case NETWORK_TYPE_ETH:
        /* Init Ethernet */
        ESP_ERROR_CHECK(eth_initialize());
        eth_hostname_set(device_name_get());
        eth_set_on_connected_cb(_network_on_connected);
        eth_set_on_disconnected_cb(_network_on_disconnected);
        break;
    case NETWORK_TYPE_WIFI:
        /* Init Wi-Fi */
        ESP_ERROR_CHECK(wifi_initialize());
        wifi_hostname_set(device_name_get());
        wifi_set_on_connected_cb(_network_on_connected);
        wifi_set_on_disconnected_cb(_network_on_disconnected);
        break;
    }

    /* Init mDNS */
    ESP_ERROR_CHECK(mdns_init());
    mdns_hostname_set(device_name_get());

    /* Init name resolver */
    ESP_ERROR_CHECK(resolve_initialize());

    /* Init MQTT */
    ESP_ERROR_CHECK(mqtt_initialize());
    mqtt_set_on_connected_cb(_mqtt_on_connected);
    mqtt_set_on_disconnected_cb(_mqtt_on_disconnected);

    /* Init BLE */
    ESP_ERROR_CHECK(ble_initialize());
    ble_set_on_broadcaster_discovered_cb(_ble_on_broadcaster_discovered);
    ble_set_on_device_discovered_cb(_ble_on_device_discovered);
    ble_set_on_device_connected_cb(_ble_on_device_connected);
    ble_set_on_device_disconnected_cb(_ble_on_device_disconnected);
    ble_set_on_device_services_discovered_cb(
        _ble_on_device_services_discovered);
    ble_set_on_device_characteristic_value_cb(
        _ble_on_device_characteristic_value);
    ble_set_on_passkey_requested_cb(ble_on_passkey_requested);

    /* Start BLE2MQTT task */
    ESP_ERROR_CHECK(start_ble2mqtt_task());

    switch (config_network_type_get())
    {
    case NETWORK_TYPE_ETH:
        eth_connect(eth_phy_atophy(config_eth_phy_get()),
            eth_clk_mode_atoclk_mode(config_eth_clk_mode_get()),
            config_eth_phy_gpio_power_get());
        break;
    case NETWORK_TYPE_WIFI:
        /* Start by connecting to network */
        wifi_connect(config_wifi_ssid_get(), config_wifi_password_get(),
            wifi_eap_atomethod(config_eap_method_get()),
            config_eap_identity_get(),
            config_eap_username_get(), config_eap_password_get(),
            config_eap_ca_cert_get(), config_eap_client_cert_get(),
            config_eap_client_key_get());
        break;
    }
}
