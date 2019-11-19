#ifndef BLE_H
#define BLE_H

#include "broadcasters.h"
#include "ble_utils.h"
#include <stddef.h>
#include <stdint.h>

/* Constants */
#define CHAR_PROP_BROADCAST (1 << 0)
#define CHAR_PROP_READ      (1 << 1)
#define CHAR_PROP_WRITE_NR  (1 << 2)
#define CHAR_PROP_WRITE     (1 << 3)
#define CHAR_PROP_NOTIFY    (1 << 4)
#define CHAR_PROP_INDICATE  (1 << 5)
#define CHAR_PROP_AUTH      (1 << 6)
#define CHAR_PROP_EXT_PROP  (1 << 7)

/* Event callback types */
typedef void (*ble_on_broadcaster_discovered_cb_t)(mac_addr_t mac,
    uint8_t *adv_data, size_t adv_data_len, int rssi, broadcaster_ops_t *ops);
typedef void (*ble_on_device_discovered_cb_t)(mac_addr_t mac, int rssi);
typedef void (*ble_on_device_connected_cb_t)(mac_addr_t mac);
typedef void (*ble_on_device_disconnected_cb_t)(mac_addr_t mac);
typedef void (*ble_on_device_services_discovered_cb_t)(mac_addr_t mac);
typedef void (*ble_on_device_characteristic_found_cb_t)(mac_addr_t mac,
    ble_uuid_t service_uuid, ble_uuid_t characteristic_uuid,
    uint8_t properties);
typedef void (*ble_on_device_characteristic_value_cb_t)(mac_addr_t mac,
    ble_uuid_t service, ble_uuid_t characteristic, uint8_t *value,
    size_t value_len);
typedef uint32_t (*ble_on_passkey_requested_cb_t)(mac_addr_t mac);

/* Event handlers */
void ble_set_on_broadcaster_discovered_cb(
    ble_on_broadcaster_discovered_cb_t cb);
void ble_set_on_device_discovered_cb(ble_on_device_discovered_cb_t cb);
void ble_set_on_device_connected_cb(ble_on_device_connected_cb_t cb);
void ble_set_on_device_disconnected_cb(ble_on_device_disconnected_cb_t cb);
void ble_set_on_device_services_discovered_cb(
    ble_on_device_services_discovered_cb_t cb);
void ble_set_on_device_characteristic_value_cb(
    ble_on_device_characteristic_value_cb_t cb);
void ble_set_on_passkey_requested_cb(ble_on_passkey_requested_cb_t cb);

/* BLE Operations */
void ble_clear_bonding_info(void);

int ble_scan_start(void);
int ble_scan_stop(void);

int ble_connect(mac_addr_t mac);
int ble_disconnect(mac_addr_t mac);
int ble_disconnect_all(void);

int ble_services_scan(mac_addr_t mac);
int ble_foreach_characteristic(mac_addr_t mac,
    ble_on_device_characteristic_found_cb_t cb);

int ble_characteristic_read(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid);
int ble_characteristic_write(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid, const uint8_t *value, size_t value_len);
int ble_characteristic_notify_register(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid);
int ble_characteristic_notify_unregister(mac_addr_t mac,
    ble_uuid_t service_uuid, ble_uuid_t characteristic_uuid);

int ble_initialize(void);

#endif
