#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/* BLE Configuration*/
const char *config_ble_service_name_get(const char *uuid);
const char *config_ble_characteristic_name_get(const char *uuid);
uint8_t config_ble_should_connect(const char *mac);
uint32_t config_ble_passkey_get(const char *mac);

/* MQTT Configuration*/
const char *config_mqtt_host_get(void);
uint16_t config_mqtt_port_get(void);
const char *config_mqtt_client_id_get(void);
const char *config_mqtt_username_get(void);
const char *config_mqtt_password_get(void);
uint8_t config_mqtt_qos_get(void);
uint8_t config_mqtt_retained_get(void);
const char *config_mqtt_get_suffix_get(void);
const char *config_mqtt_set_suffix_get(void);

/* WiFi Configuration*/
const char *config_wifi_ssid_get(void);
const char *config_wifi_password_get(void);

int config_initialize(void);

#endif
