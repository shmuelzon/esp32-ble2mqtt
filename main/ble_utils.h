#ifndef BLE_UTILS_H
#define BLE_UTILS_H

#include "ble.h"
#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>

/* Types */
typedef struct ble_characteristic_t {
    struct ble_characteristic_t *next;
    ble_uuid_t uuid;
    uint16_t handle;
    uint8_t properties;
    uint16_t client_config_handle;
} ble_characteristic_t;

typedef struct ble_service_t {
    struct ble_service_t *next;
    ble_uuid_t uuid;
    ble_characteristic_t *characteristics;
} ble_service_t;

typedef struct ble_device_t {
    struct ble_device_t *next;
    mac_addr_t mac;
    uint16_t conn_id;
    ble_service_t *services;
} ble_device_t;

/* Callback functions */
typedef int (*ble_on_device_cb_t)(ble_device_t *device);

/* Enumerations to strings */
char *gap_event_to_str(esp_gap_ble_cb_event_t event);
char *gattc_event_to_str(esp_gattc_cb_event_t event);

/* Conversion functions */
char *mactoa(mac_addr_t mac);
int atomac(const char *str, mac_addr_t mac);
char *uuidtoa(ble_uuid_t uuid);
int atouuid(const char *str, ble_uuid_t uuid);

/* Devices list */
ble_device_t *ble_device_add(ble_device_t **list, mac_addr_t mac,
    uint16_t conn_id);
ble_device_t *ble_device_find_by_mac(ble_device_t *list, mac_addr_t mac);
ble_device_t *ble_device_find_by_conn_id(ble_device_t *list, uint16_t conn_id);
void ble_device_foreach(ble_device_t *list, ble_on_device_cb_t cb);
void ble_device_remove_by_mac(ble_device_t **list, mac_addr_t mac);
void ble_device_remove_by_conn_id(ble_device_t **list, uint16_t conn_id);
void ble_device_free(ble_device_t *device);
void ble_devices_free(ble_device_t **list);

ble_service_t *ble_device_service_add(ble_device_t *device, ble_uuid_t uuid);
ble_service_t *ble_device_service_find(ble_device_t *device, ble_uuid_t uuid);
void ble_device_service_free(ble_service_t *service);
void ble_device_services_free(ble_service_t **list);

ble_characteristic_t *ble_device_characteristic_add(ble_service_t *service,
    ble_uuid_t uuid, uint16_t handle, uint8_t properties);
ble_characteristic_t *ble_device_characteristic_find_by_uuid(
    ble_service_t *service, ble_uuid_t uuid);
ble_characteristic_t *ble_device_characteristic_find_by_handle(
    ble_service_t *service, uint16_t handle);
void ble_device_characteristic_free(ble_characteristic_t *characteristic);
void ble_device_characteristics_free(ble_characteristic_t **list);

int ble_device_info_get_by_conn_id_handle(ble_device_t *list, uint16_t conn_id,
    uint16_t handle, ble_device_t **device, ble_service_t **service,
    ble_characteristic_t **characteristic);

#endif
