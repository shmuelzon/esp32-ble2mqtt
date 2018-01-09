#include "ble.h"
#include "ble_utils.h"
#include <freertos/FreeRTOSConfig.h> /* Needed bt esp_bt.h */
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>
#include <esp_gatt_defs.h>
#include <esp_gatt_common_api.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>

/* Constants */
#define INVALID_HANDLE 0

static const char *TAG = "BLE";
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    /* Scan interval. This is defined as the time interval from
     * when the Controller started its last LE scan until it begins the
     * subsequent LE scan.
     * Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
     * Time = N * 0.625 msec
     * Time Range: 2.5 msec to 10.24 seconds
     */
    .scan_interval = 16, /* 16 * 0.625ms = 10ms */
    /* Scan window. The duration of the LE scan. LE_Scan_Window shall be
     * less than or equal to LE_Scan_Interval
     * Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
     * Time = N * 0.625 msec
     * Time Range: 2.5 msec to 10240 msec
     */
    .scan_window = 16, /* 16 * 0.625ms = 10ms */
};

/* Internal state */
static uint8_t scan_requested = 0;
static esp_gatt_if_t g_gattc_if = ESP_GATT_IF_NONE;
static ble_device_t *devices_list = NULL;

/* Callback functions */
static ble_on_device_discovered_cb_t on_device_discovered_cb = NULL;
static ble_on_device_connected_cb_t on_device_connected_cb = NULL;
static ble_on_device_disconnected_cb_t on_device_disconnected_cb = NULL;
static ble_on_device_services_discovered_cb_t
    on_device_services_discovered_cb = NULL;
static ble_on_device_characteristic_value_cb_t
    on_device_characteristic_value_cb = NULL;
static ble_on_passkey_requested_cb_t on_passkey_requested_cb = NULL;

void ble_set_on_device_discovered_cb(ble_on_device_discovered_cb_t cb)
{
    on_device_discovered_cb = cb;
}

void ble_set_on_device_connected_cb(ble_on_device_connected_cb_t cb)
{
    on_device_connected_cb = cb;
}

void ble_set_on_device_disconnected_cb(ble_on_device_disconnected_cb_t cb)
{
    on_device_disconnected_cb = cb;
}

void ble_set_on_device_services_discovered_cb(
    ble_on_device_services_discovered_cb_t cb)
{
    on_device_services_discovered_cb = cb;
}

void ble_set_on_device_characteristic_value_cb(
    ble_on_device_characteristic_value_cb_t cb)
{
    on_device_characteristic_value_cb = cb;
}

void ble_set_on_passkey_requested_cb(ble_on_passkey_requested_cb_t cb)
{
    on_passkey_requested_cb = cb;
}

int ble_scan_start(void)
{
    ESP_LOGD(TAG, "Starting BLE scan");
    if (scan_requested)
        return 0;

    scan_requested = 1;
    return esp_ble_gap_start_scanning(-1);
}

int ble_scan_stop(void)
{
    ESP_LOGD(TAG, "Stopping BLE scan");
    scan_requested = 0;
    return esp_ble_gap_stop_scanning();
}

int ble_connect(mac_addr_t mac)
{
    /* Stop scanning while attempting to connect */
    esp_ble_gap_stop_scanning();
    return esp_ble_gattc_open(g_gattc_if, mac, true);
}

static int _ble_disconnect(ble_device_t *dev)
{
    return esp_ble_gattc_close(g_gattc_if, dev->conn_id);
}

int ble_disconnect(mac_addr_t mac)
{
    ble_device_t *dev = ble_device_find_by_mac(devices_list, mac);

    if (!dev)
        return -1;

    return _ble_disconnect(dev);
}

int ble_disconnect_all(void)
{
    ble_device_foreach(devices_list, _ble_disconnect);
    return 0;
}

int ble_services_scan(mac_addr_t mac)
{
    ble_device_t *dev = ble_device_find_by_mac(devices_list, mac);

    if (!dev)
        return -1;

    return esp_ble_gattc_search_service(g_gattc_if, dev->conn_id, NULL);
}

static void esp_uuid_to_bt_uuid(esp_bt_uuid_t esp, ble_uuid_t bt)
{
    /* Bluetooth SIG Base UUID */
    static uint8_t base_uuid[16] = { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00,
        0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if (esp.len == ESP_UUID_LEN_128)
        memcpy(bt, &esp.uuid, esp.len);
    else
    {
        memcpy(bt, base_uuid, sizeof(base_uuid));
        memcpy(&bt[12], &esp.uuid, esp.len);
    }
}

static void ble_update_cache(ble_device_t *dev)
{
    esp_gattc_db_elem_t *db;
    ble_service_t *service = NULL;
    ble_characteristic_t *characteristic = NULL;
    ble_uuid_t service_uuid, characteristic_uuid;
    uint16_t count, i;

    if (!dev)
        return;

    /* Get GATT DB Size */
    if (esp_ble_gattc_get_attr_count(g_gattc_if, dev->conn_id, ESP_GATT_DB_ALL,
        0, UINT16_MAX, INVALID_HANDLE, &count))
    {
        return;
    }

    /* Get GATT DB */
    db = malloc(sizeof(*db) * count);
    if (esp_ble_gattc_get_db(g_gattc_if, dev->conn_id, 0, UINT16_MAX, db,
        &count))
    {
        free(db);
        return;
    }
    
    /* Find all characteristics and cache them */
    for (i = 0; i < count; i++)
    {
        if (db[i].type == ESP_GATT_DB_PRIMARY_SERVICE)
        {
            esp_uuid_to_bt_uuid(db[i].uuid, service_uuid);
            service = ble_device_service_add(dev, service_uuid);
        }
        else if (db[i].type == ESP_GATT_DB_CHARACTERISTIC)
        {
            esp_uuid_to_bt_uuid(db[i].uuid, characteristic_uuid);
            characteristic = ble_device_characteristic_add(service,
                characteristic_uuid, db[i].attribute_handle, db[i].properties);
        }
        else if (db[i].type == ESP_GATT_DB_DESCRIPTOR &&
            db[i].uuid.len == ESP_UUID_LEN_16 &&
            db[i].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG)
        {
            characteristic->client_config_handle = db[i].attribute_handle;
        }
    }
    free(db);
}

int ble_foreach_characteristic(mac_addr_t mac,
    ble_on_device_characteristic_found_cb_t cb)
{
    ble_device_t *dev = ble_device_find_by_mac(devices_list, mac);
    ble_service_t *service;
    ble_characteristic_t *characteristic;

    if (!dev)
        return -1;

    /* If services list is empty, try to update it */
    if (!dev->services)
        ble_update_cache(dev);

    for (service = dev->services; service; service = service->next)
    {
        for (characteristic = service->characteristics; characteristic;
            characteristic = characteristic->next)
        {
            cb(mac, service->uuid, characteristic->uuid,
                characteristic->properties);
        }
    }

    return 0;
}

int ble_characteristic_read(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid)
{
    ble_device_t *device;
    ble_service_t *service;
    ble_characteristic_t *characteristic;

    if (!(device = ble_device_find_by_mac(devices_list, mac)))
        return -1;

    if (!(service = ble_device_service_find(device, service_uuid)))
        return -1;

    if (!(characteristic = ble_device_characteristic_find_by_uuid(service,
        characteristic_uuid)))
    {
        return -1;
    }

    if (!(characteristic->properties & CHAR_PROP_READ))
        return -1;

    return esp_ble_gattc_read_char(g_gattc_if, device->conn_id,
        characteristic->handle, ESP_GATT_AUTH_REQ_NONE);
}

int ble_characteristic_write(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid, const uint8_t *value, size_t value_len)
{
    ble_device_t *device;
    ble_service_t *service;
    ble_characteristic_t *characteristic;

    if (!(device = ble_device_find_by_mac(devices_list, mac)))
        return -1;

    if (!(service = ble_device_service_find(device, service_uuid)))
        return -1;

    if (!(characteristic = ble_device_characteristic_find_by_uuid(service,
        characteristic_uuid)))
    {
        return -1;
    }

    if (!(characteristic->properties & CHAR_PROP_WRITE))
        return -1;

    return esp_ble_gattc_write_char(g_gattc_if, device->conn_id,
        characteristic->handle, value_len, (uint8_t *)value,
        ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
}

int ble_characteristic_notify_register(mac_addr_t mac, ble_uuid_t service_uuid,
    ble_uuid_t characteristic_uuid)
{
    uint16_t notify_en = 1;
    ble_device_t *device;
    ble_service_t *service;
    ble_characteristic_t *characteristic;

    if (!(device = ble_device_find_by_mac(devices_list, mac)))
        return -1;

    if (!(service = ble_device_service_find(device, service_uuid)))
        return -1;

    if (!(characteristic = ble_device_characteristic_find_by_uuid(service,
        characteristic_uuid)))
    {
        return -1;
    }

    if (!(characteristic->properties & CHAR_PROP_NOTIFY))
        return -1;

    if (characteristic->client_config_handle == 0)
        return -1;

    if (esp_ble_gattc_register_for_notify(g_gattc_if, device->mac,
        characteristic->handle))
    {
        return -1;
    }

    return esp_ble_gattc_write_char_descr(g_gattc_if, device->conn_id,
        characteristic->client_config_handle, sizeof(notify_en),
        (uint8_t *)&notify_en, ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
}

int ble_characteristic_notify_unregister(mac_addr_t mac,
    ble_uuid_t service_uuid, ble_uuid_t characteristic_uuid)
{
    ble_device_t *device;
    ble_service_t *service;
    ble_characteristic_t *characteristic;

    if (!(device = ble_device_find_by_mac(devices_list, mac)))
        return -1;

    if (!(service = ble_device_service_find(device, service_uuid)))
        return -1;

    if (!(characteristic = ble_device_characteristic_find_by_uuid(service,
        characteristic_uuid)))
    {
        return -1;
    }

    return esp_ble_gattc_unregister_for_notify(g_gattc_if, device->mac,
        characteristic->handle);
}

static void gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGD(TAG, "Received GAP event %d (%s)", event, gap_event_to_str(event));

    switch (event)
    {
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Configuring local privacy failed, status: 0x%x",
                param->local_privacy_cmpl.status);
            break;
        }
        ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&ble_scan_params));
        break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Scanning starting failed, status: 0x%x",
                param->scan_start_cmpl.status);
        }
        break;
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(TAG, "Scanning stopping failed, status: 0x%x",
                param->scan_stop_cmpl.status);
        }
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
    {
        ble_device_t *device;

        if (param->scan_rst.search_evt != ESP_GAP_SEARCH_INQ_RES_EVT)
            break;
            
        device = ble_device_find_by_mac(devices_list, param->scan_rst.bda);

        /* Device already discovered, nothing to do*/
        if (device)
            break;

        /* Cache device information */
        ble_device_add(&devices_list, param->scan_rst.bda, -1);

        /* Notify app only on newly connected devices */
        if(on_device_discovered_cb)
            on_device_discovered_cb(param->scan_rst.bda);

        break;
    }
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:
        esp_ble_passkey_reply(param->ble_security.ble_req.bd_addr, true,
            on_passkey_requested_cb ?
            on_passkey_requested_cb(param->ble_security.ble_req.bd_addr) : 0);
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        if (!param->ble_security.auth_cmpl.success)
        {
            ESP_LOGE(TAG, "Authentication failed, status: 0x%x",
                param->ble_security.auth_cmpl.fail_reason);
        }
        break;
    }
    default:
        ESP_LOGD(TAG, "GAP event %d wasn't handled", event);
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param)
{
    ESP_LOGD(TAG, "Received GATTC event %d (%s), gattc_if %d", event,
        gattc_event_to_str(event), gattc_if);

    switch (event)
    {
    case ESP_GATTC_REG_EVT:
        /* Save the interface for later */
        g_gattc_if = gattc_if;
        break;
    case ESP_GATTC_OPEN_EVT:
    {
        ble_device_t *device;
        /* Resume scanning, if requested */
        if (scan_requested)
            esp_ble_gap_start_scanning(-1);

        if (param->open.status != ESP_GATT_OK)
        {
            ESP_LOGE(TAG, "Open failed, status = 0x%x", param->open.status);
            /* Remove device from cache */
            ble_device_remove_by_mac(&devices_list, param->open.remote_bda);
            break;
        }

        /* Save device connection ID */
        device = ble_device_find_by_mac(devices_list, param->open.remote_bda);
        device->conn_id = param->open.conn_id;

        /* Configure MTU */
        ESP_ERROR_CHECK(esp_ble_gattc_send_mtu_req(gattc_if,
            param->open.conn_id));

        break;
    }
    case ESP_GATTC_CLOSE_EVT:
        ESP_LOGI(TAG, "Connection closed, reason = 0x%x", param->close.reason);
        /* Notify app that the device is disconnected */
        if (on_device_disconnected_cb)
            on_device_disconnected_cb(param->close.remote_bda);

        /* Remove device from cache */
        ble_device_remove_by_mac(&devices_list, param->close.remote_bda);
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if (param->cfg_mtu.status != ESP_GATT_OK)
        {
            ESP_LOGE(TAG, "Configuring MTU failed, status = 0x%x",
            param->cfg_mtu.status);
        }

        /* Notify app that the device is connected */
        if (on_device_connected_cb)
        {
            ble_device_t *dev = ble_device_find_by_conn_id(devices_list,
                param->cfg_mtu.conn_id);

            if (dev)
                on_device_connected_cb(dev->mac);
        }

        break;
    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (param->search_cmpl.status != ESP_GATT_OK)
        {
            ESP_LOGE(TAG, "Searching services failed, status = 0x%x",
                param->search_cmpl.status);
            break;
        }

        /* Notify app that the services were discovered */
        if (on_device_services_discovered_cb)
        {
            ble_device_t *dev = ble_device_find_by_conn_id(devices_list,
                param->search_cmpl.conn_id);

            if (dev)
                on_device_services_discovered_cb(dev->mac);
        }

        break;
    case ESP_GATTC_READ_CHAR_EVT:
    {
        ble_device_t *device;
        ble_service_t *service;
        ble_characteristic_t *characteristic;

        if (param->read.status != ESP_GATT_OK)
        {
            ESP_LOGE(TAG, "Failed reading characteristic, status = 0x%x",
                param->read.status);

            /* Check if encryption/pairing is needed */
            if (param->read.status == ESP_GATT_INSUF_AUTHENTICATION ||
                param->read.status == ESP_GATT_INSUF_ENCRYPTION)
            {
                device = ble_device_find_by_conn_id(devices_list,
                    param->read.conn_id);
                if (device)
                {
                    esp_ble_set_encryption(device->mac,
                        ESP_BLE_SEC_ENCRYPT_MITM);
                }
            }
        }
        else if (!ble_device_info_get_by_conn_id_handle(devices_list,
            param->read.conn_id, param->read.handle, &device, &service,
            &characteristic) && on_device_characteristic_value_cb)
        {
            on_device_characteristic_value_cb(device->mac, service->uuid,
                characteristic->uuid, param->read.value, param->read.value_len);
        }

        break;
    }
    case ESP_GATTC_WRITE_CHAR_EVT:
        if (param->write.status != ESP_GATT_OK)
        {
            ESP_LOGE(TAG, "Failed writing characteristic, status = 0x%x",
                param->write.status);
        }
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT:
        if (param->reg_for_notify.status != ESP_GATT_OK)
        {
            ESP_LOGE(TAG, "Failed registering for notification, status = 0x%x",
                param->reg_for_notify.status);
        }
        break;
    case ESP_GATTC_NOTIFY_EVT:
    {
        ble_device_t *device;
        ble_service_t *service;
        ble_characteristic_t *characteristic;

        if (!ble_device_info_get_by_conn_id_handle(devices_list,
            param->notify.conn_id, param->notify.handle, &device, &service,
            &characteristic) && on_device_characteristic_value_cb)
        {
            on_device_characteristic_value_cb(device->mac, service->uuid,
                characteristic->uuid, param->notify.value,
                param->notify.value_len);
        }

        break;
    }
    default:
        ESP_LOGD(TAG, "GATTC event %d wasn't handled", event);
        break;
    }
}

int ble_initialize(void)
{
    ESP_LOGD(TAG, "Initializing BLE client");

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BTDM));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));
    ESP_ERROR_CHECK(esp_ble_gap_config_local_privacy(true));
    ESP_ERROR_CHECK(esp_ble_gattc_register_callback(esp_gattc_cb))
    ESP_ERROR_CHECK(esp_ble_gattc_app_register(0));
    ESP_ERROR_CHECK(esp_ble_gatt_set_local_mtu(200));
    /* Set security IO capability to KeyboardOnly */
    esp_ble_io_cap_t iocap = ESP_IO_CAP_IN;
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap,
        sizeof(iocap));

    return 0;
}
