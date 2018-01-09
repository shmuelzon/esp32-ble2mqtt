#include "config.h"
#include "ble.h"
#include "ble_utils.h"
#include "mqtt.h"
#include "wifi.h"
#include <esp_err.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <string.h>

#define MAX_TOPIC_LEN 256
static const char *TAG = "BLE2MQTT";

typedef struct {
    mac_addr_t mac;
    ble_uuid_t service;
    ble_uuid_t characteristic;
} mqtt_ctx_t;

/* Wi-Fi callback functions */
static void wifi_on_connected(void)
{
    ESP_LOGI(TAG, "Connected to WiFi, connecting to MQTT");
    mqtt_connect(config_mqtt_host_get(), config_mqtt_port_get(),
        config_mqtt_client_id_get(), config_mqtt_username_get(),
        config_mqtt_password_get());
}

static void wifi_on_disconnected(void)
{
    ESP_LOGI(TAG, "Disonnected from WiFi, stopping MQTT");
    mqtt_disconnect();
}

/* MQTT callback functions */
static void mqtt_on_connected(void)
{
    ESP_LOGI(TAG, "Connected to MQTT, scanning for BLE devices");
    ble_scan_start();
}

static void mqtt_on_disconnected(void)
{
    ESP_LOGI(TAG, "Disonnected from MQTT, stopping BLE");
    ble_disconnect_all();
    mqtt_connect(config_mqtt_host_get(), config_mqtt_port_get(),
        config_mqtt_client_id_get(), config_mqtt_username_get(),
        config_mqtt_password_get());
}

/* BLE functions */
static void ble_publish_connected(mac_addr_t mac, uint8_t is_connected)
{
    char topic[28];

    sprintf(topic, "%s/Connected", mactoa(mac));
    mqtt_publish(topic, (uint8_t *)(is_connected ? "true" : "false"),
        is_connected ? 4 : 5, config_mqtt_qos_get(),
        config_mqtt_retained_get());
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
static void ble_on_device_discovered(mac_addr_t mac)
{
    uint8_t connect = config_ble_should_connect(mactoa(mac));

    ESP_LOGI(TAG, "Discovered BLE device: %s, %sconnecting", mactoa(mac),
        connect ? "" : "not ");

    if (!connect)
        return;

    ble_connect(mac);
}

static void ble_on_device_connected(mac_addr_t mac)
{
    ESP_LOGI(TAG, "Connected to device: %s, scanning", mactoa(mac));
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

    i = sprintf(topic, "%s/%s", mactoa(mac),
        config_ble_service_name_get(uuidtoa(service_uuid)));
    sprintf(topic + i, "/%s",
        config_ble_characteristic_name_get(uuidtoa(characteristic_uuid)));

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

    if (properties & CHAR_PROP_NOTIFY)
    {
        ble_characteristic_notify_unregister(mac, service_uuid,
            characteristic_uuid);
    }
}

static void ble_on_device_disconnected(mac_addr_t mac)
{
    ESP_LOGI(TAG, "Disconnected from device: %s", mactoa(mac));
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

    ble_characteristic_write(data->mac, data->service, data->characteristic,
        payload, len);

    /* Issue a read request to get latest value */
    ble_characteristic_read(data->mac, data->service, data->characteristic);
}

static void ble_on_characteristic_found(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid, uint8_t properties)
{
    ESP_LOGD(TAG, "Found new characteristic!");
    ESP_LOGD(TAG, "  Service: %s", uuidtoa(service_uuid));
    ESP_LOGD(TAG, "  Characteristic: %s", uuidtoa(characteristic_uuid));
    char *topic = ble_topic(mac, service_uuid, characteristic_uuid);

    /* Characteristic is readable */
    if (properties & CHAR_PROP_READ)
    {
        mqtt_subscribe(ble_topic_suffix(topic, 1), config_mqtt_qos_get(),
            ble_on_mqtt_get, ble_ctx_gen(mac, service_uuid,
            characteristic_uuid), free);
        ble_characteristic_read(mac, service_uuid, characteristic_uuid);
    }

    /* Characteristic is writable */
    if (properties & CHAR_PROP_WRITE)
    {
        mqtt_subscribe(ble_topic_suffix(topic, 0), config_mqtt_qos_get(),
            ble_on_mqtt_set, ble_ctx_gen(mac, service_uuid,
            characteristic_uuid), free);
    }

    /* Characteristic can notify on changes */
    if (properties & CHAR_PROP_NOTIFY)
    {
        ble_characteristic_notify_register(mac, service_uuid,
            characteristic_uuid);
    }
}

static void ble_on_device_services_discovered(mac_addr_t mac)
{
    ESP_LOGD(TAG, "Services discovered on device: %s", mactoa(mac));
    ble_foreach_characteristic(mac, ble_on_characteristic_found);
}

static void ble_on_device_characteristic_value(mac_addr_t mac,
    ble_uuid_t service, ble_uuid_t characteristic, uint8_t *value,
    size_t value_len)
{
    char *topic = ble_topic(mac, service, characteristic);

    ESP_LOGI(TAG, "Publishing: %s", topic);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, value, value_len, ESP_LOG_DEBUG);
    mqtt_publish(topic, value, value_len, config_mqtt_qos_get(),
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

void app_main()
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Init configuration */
    ESP_ERROR_CHECK(config_initialize());

    /* Init Wi-Fi */
    ESP_ERROR_CHECK(wifi_initialize());
    wifi_set_on_connected_cb(wifi_on_connected);
    wifi_set_on_disconnected_cb(wifi_on_disconnected);

    /* Init MQTT */
    ESP_ERROR_CHECK(mqtt_initialize());
    mqtt_set_on_connected_cb(mqtt_on_connected);
    mqtt_set_on_disconnected_cb(mqtt_on_disconnected);

    /* Init BLE */
    ESP_ERROR_CHECK(ble_initialize());
    ble_set_on_device_discovered_cb(ble_on_device_discovered);
    ble_set_on_device_connected_cb(ble_on_device_connected);
    ble_set_on_device_disconnected_cb(ble_on_device_disconnected);
    ble_set_on_device_services_discovered_cb(ble_on_device_services_discovered);
    ble_set_on_device_characteristic_value_cb(
        ble_on_device_characteristic_value);
    ble_set_on_passkey_requested_cb(ble_on_passkey_requested);

    /* Start by connecting to WiFi */
    wifi_connect(config_wifi_ssid_get(), config_wifi_password_get());
}
