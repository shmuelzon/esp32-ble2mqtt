#include "config.h"
#include "broadcasters.h"
#include "ble.h"
#include "ble_utils.h"
#include "mqtt.h"
#include "ota.h"
#include "wifi.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
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

static char *device_name_get(void)
{
    static char name[14] = {};

    if (!*name)
    {
        uint8_t *mac = wifi_mac_get();
        sprintf(name, "BLE2MQTT-%02X%02X", mac[4], mac[5]);
    }

    return name;
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
    ESP_LOGI(TAG, "Disconnected from WiFi, stopping MQTT");
    mqtt_disconnect();
    /* We don't get notified when manually stopping MQTT */
    cleanup();
}

/* MQTT callback functions */
static void mqtt_on_connected(void)
{
    ESP_LOGI(TAG, "Connected to MQTT, scanning for BLE devices");
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

    snprintf(topic, MAX_TOPIC_LEN, "%s%s/Connected", config_mqtt_prefix_get(),
        mactoa(mac));

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
        snprintf(topic, MAX_TOPIC_LEN, "%s%s/Owner", config_mqtt_prefix_get(),
            mactoa(mac));
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

    i = snprintf(topic, MAX_TOPIC_LEN, "%s%s/%s", config_mqtt_prefix_get(),
        mactoa(mac), ble_service_name_get(service_uuid));
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
    ESP_LOGD(TAG, "Found new characteristic!");
    ESP_LOGD(TAG, "  Service: %s", uuidtoa(service_uuid));
    ESP_LOGD(TAG, "  Characteristic: %s", uuidtoa(characteristic_uuid));
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

    /* Init OTA */
    ota_initialize();
    ota_set_on_completed_cb(ota_on_completed);

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
    ble_set_on_broadcaster_discovered_cb(ble_on_broadcaster_discovered);
    ble_set_on_device_discovered_cb(ble_on_device_discovered);
    ble_set_on_device_connected_cb(ble_on_device_connected);
    ble_set_on_device_disconnected_cb(ble_on_device_disconnected);
    ble_set_on_device_services_discovered_cb(ble_on_device_services_discovered);
    ble_set_on_device_characteristic_value_cb(
        ble_on_device_characteristic_value);
    ble_set_on_passkey_requested_cb(ble_on_passkey_requested);

    /* Start by connecting to WiFi */
    wifi_hostname_set(device_name_get());
    wifi_connect(config_wifi_ssid_get(), config_wifi_password_get());
}
