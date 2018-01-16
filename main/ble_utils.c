#include "ble_utils.h"
#include "config.h"
#include "gatt.h"
#include <string.h>

#define CASE_STR(x) case x: return #x
char *gap_event_to_str(esp_gap_ble_cb_event_t event)
{
    switch (event)
    {
    CASE_STR(ESP_GAP_BLE_ADD_WHITELIST_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_ADV_START_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_AUTH_CMPL_EVT);
    CASE_STR(ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_KEY_EVT);
    CASE_STR(ESP_GAP_BLE_LOCAL_ER_EVT);
    CASE_STR(ESP_GAP_BLE_LOCAL_IR_EVT);
    CASE_STR(ESP_GAP_BLE_NC_REQ_EVT);
    CASE_STR(ESP_GAP_BLE_OOB_REQ_EVT);
    CASE_STR(ESP_GAP_BLE_PASSKEY_NOTIF_EVT);
    CASE_STR(ESP_GAP_BLE_PASSKEY_REQ_EVT);
    CASE_STR(ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SCAN_RESULT_EVT);
    CASE_STR(ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SCAN_START_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SEC_REQ_EVT);
    CASE_STR(ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT);
    CASE_STR(ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT);
    CASE_STR(ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT);
    default: return "Invalid GAP event";
    }
}

char *gattc_event_to_str(esp_gattc_cb_event_t event)
{
    switch (event)
    {
    CASE_STR(ESP_GATTC_REG_EVT);
    CASE_STR(ESP_GATTC_UNREG_EVT);
    CASE_STR(ESP_GATTC_OPEN_EVT);
    CASE_STR(ESP_GATTC_READ_CHAR_EVT);
    CASE_STR(ESP_GATTC_WRITE_CHAR_EVT);
    CASE_STR(ESP_GATTC_CLOSE_EVT);
    CASE_STR(ESP_GATTC_SEARCH_CMPL_EVT);
    CASE_STR(ESP_GATTC_SEARCH_RES_EVT);
    CASE_STR(ESP_GATTC_READ_DESCR_EVT);
    CASE_STR(ESP_GATTC_WRITE_DESCR_EVT);
    CASE_STR(ESP_GATTC_NOTIFY_EVT);
    CASE_STR(ESP_GATTC_PREP_WRITE_EVT);
    CASE_STR(ESP_GATTC_EXEC_EVT);
    CASE_STR(ESP_GATTC_ACL_EVT);
    CASE_STR(ESP_GATTC_CANCEL_OPEN_EVT);
    CASE_STR(ESP_GATTC_SRVC_CHG_EVT);
    CASE_STR(ESP_GATTC_ENC_CMPL_CB_EVT);
    CASE_STR(ESP_GATTC_CFG_MTU_EVT);
    CASE_STR(ESP_GATTC_ADV_DATA_EVT);
    CASE_STR(ESP_GATTC_MULT_ADV_ENB_EVT);
    CASE_STR(ESP_GATTC_MULT_ADV_UPD_EVT);
    CASE_STR(ESP_GATTC_MULT_ADV_DATA_EVT);
    CASE_STR(ESP_GATTC_MULT_ADV_DIS_EVT);
    CASE_STR(ESP_GATTC_CONGEST_EVT);
    CASE_STR(ESP_GATTC_BTH_SCAN_ENB_EVT);
    CASE_STR(ESP_GATTC_BTH_SCAN_CFG_EVT);
    CASE_STR(ESP_GATTC_BTH_SCAN_RD_EVT);
    CASE_STR(ESP_GATTC_BTH_SCAN_THR_EVT);
    CASE_STR(ESP_GATTC_BTH_SCAN_PARAM_EVT);
    CASE_STR(ESP_GATTC_BTH_SCAN_DIS_EVT);
    CASE_STR(ESP_GATTC_SCAN_FLT_CFG_EVT);
    CASE_STR(ESP_GATTC_SCAN_FLT_PARAM_EVT);
    CASE_STR(ESP_GATTC_SCAN_FLT_STATUS_EVT);
    CASE_STR(ESP_GATTC_ADV_VSC_EVT);
    CASE_STR(ESP_GATTC_REG_FOR_NOTIFY_EVT);
    CASE_STR(ESP_GATTC_UNREG_FOR_NOTIFY_EVT);
    CASE_STR(ESP_GATTC_CONNECT_EVT);
    CASE_STR(ESP_GATTC_DISCONNECT_EVT);
    CASE_STR(ESP_GATTC_READ_MUTIPLE_EVT);
    CASE_STR(ESP_GATTC_QUEUE_FULL_EVT);
    default: return "Invalid GATTC event";
    }
}
#undef CASE_STR

char *mactoa(mac_addr_t mac)
{
    static char s[18];

    sprintf(s, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3],
        mac[4], mac[5]);

    return s;
}

int atomac(const char *str, mac_addr_t mac)
{
    return sscanf(str, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
        &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6;
}

char *uuidtoa(ble_uuid_t uuid)
{
    static char s[37];

    sprintf(s,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid[15], uuid[14], uuid[13], uuid[12],
        uuid[11], uuid[10],
        uuid[9], uuid[8], 
        uuid[7], uuid[6],
        uuid[5], uuid[4], uuid[3], uuid[2], uuid[1], uuid[0]);

    return s;
}

int atouuid(const char *str, ble_uuid_t uuid)
{
    return sscanf(str,
        "%2hhx%2hhx%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-%2hhx%2hhx-"
        "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
        &uuid[15], &uuid[14], &uuid[13], &uuid[12],
        &uuid[11], &uuid[10],
        &uuid[9], &uuid[8], 
        &uuid[7], &uuid[6],
        &uuid[5], &uuid[4], &uuid[3], &uuid[2], &uuid[1], &uuid[0]) != 16;
}

static const char *ble_get_sig_service_name(ble_uuid_t uuid)
{
    service_desc_t *p;

    for (p = services; p->name; p++)
    {
        if (memcmp(p->uuid, uuid, sizeof(ble_uuid_t)))
            continue;

        return p->name;
    }

    return NULL;
}

const char *ble_service_name_get(ble_uuid_t uuid)
{
    const char *name = config_ble_service_name_get(uuidtoa(uuid));

    if (name)
        return name;

    return ble_get_sig_service_name(uuid) ? : uuidtoa(uuid);
}

static const char *ble_get_sig_characteristic_name(ble_uuid_t uuid)
{
    characteristic_desc_t *p;

    for (p = characteristics; p->name; p++)
    {
        if (memcmp(p->uuid, uuid, sizeof(ble_uuid_t)))
            continue;

        return p->name;
    }

    return NULL;
}

const char *ble_characteristic_name_get(ble_uuid_t uuid)
{
    const char *name = config_ble_characteristic_name_get(uuidtoa(uuid));

    if (name)
        return name;

    return ble_get_sig_characteristic_name(uuid) ? : uuidtoa(uuid);
}

ble_device_t *ble_device_add(ble_device_t **list, mac_addr_t mac,
    uint16_t conn_id)
{
    ble_device_t *dev, **cur;

    dev = malloc(sizeof(*dev));
    dev->next = NULL;
    memcpy(dev->mac, mac, sizeof(mac_addr_t));
    dev->conn_id = conn_id;
    dev->services = NULL;

    for (cur = list; *cur; cur = &(*cur)->next);
    *cur = dev;

    return dev;
}

ble_device_t *ble_device_find_by_mac(ble_device_t *list, mac_addr_t mac)
{
    ble_device_t *cur;

    for (cur = list; cur; cur = cur->next)
    {
        if (!memcmp(cur->mac, mac, sizeof(mac_addr_t)))
            break;
    }

    return cur;
}

void ble_device_foreach(ble_device_t *list, ble_on_device_cb_t cb)
{
    for (; list; list = list->next)
        cb(list);
}

ble_device_t *ble_device_find_by_conn_id(ble_device_t *list, uint16_t conn_id)
{
    ble_device_t *cur;

    for (cur = list; cur; cur = cur->next)
    {
        if (cur->conn_id == conn_id)
            break;
    }

    return cur;
}

void ble_device_remove_by_mac(ble_device_t **list, mac_addr_t mac)
{
    ble_device_t **cur, *tmp;

    for (cur = list; *cur; cur = &(*cur)->next)
    {
        if (!memcmp((*cur)->mac, mac, sizeof(mac_addr_t)))
            break;
    }

    if (!*cur)
        return;

    tmp = *cur;
    *cur = (*cur)->next;
    ble_device_free(tmp);
}

void ble_device_remove_by_conn_id(ble_device_t **list, uint16_t conn_id)
{
    ble_device_t **cur, *tmp;

    for (cur = list; *cur; cur = &(*cur)->next)
    {
        if ((*cur)->conn_id == conn_id)
            break;
    }

    if (!*cur)
        return;

    tmp = *cur;
    *cur = (*cur)->next;
    ble_device_free(tmp);
}

void ble_device_free(ble_device_t *dev)
{
    ble_device_services_free(&dev->services);
    free(dev);
}

void ble_devices_free(ble_device_t **list)
{
    ble_device_t *cur, **head = list;

    while (*list)
    {
        cur = *list;
        *list = cur->next;
        ble_device_free(cur);
    }
    *head = NULL;
}

ble_service_t *ble_device_service_add(ble_device_t *device, ble_uuid_t uuid)
{
    ble_service_t *service, **cur;

    service = malloc(sizeof(*service));
    service->next = NULL;
    memcpy(service->uuid, uuid, sizeof(ble_uuid_t));
    service->characteristics = NULL;

    for (cur = &device->services; *cur; cur = &(*cur)->next);
    *cur = service;

    return service;
}

ble_service_t *ble_device_service_find(ble_device_t *device, ble_uuid_t uuid)
{
    ble_service_t *cur;

    for (cur = device->services; cur; cur = cur->next)
    {
        if (!memcmp(cur->uuid, uuid, sizeof(ble_uuid_t)))
            break;
    }

    return cur;
}

void ble_device_service_free(ble_service_t *service)
{
    ble_device_characteristics_free(&service->characteristics);
    free(service);
}

void ble_device_services_free(ble_service_t **list)
{
    ble_service_t *cur, **head = list;

    while (*list)
    {
        cur = *list;
        *list = cur->next;
        ble_device_service_free(cur);
    }
    *head = NULL;
}

ble_characteristic_t *ble_device_characteristic_add(ble_service_t *service,
    ble_uuid_t uuid, uint16_t handle, uint8_t properties)
{
    ble_characteristic_t *characteristic, **cur;

    characteristic = malloc(sizeof(*characteristic));
    characteristic->next = NULL;
    memcpy(characteristic->uuid, uuid, sizeof(ble_uuid_t));
    characteristic->handle = handle;
    characteristic->properties = properties;
    characteristic->client_config_handle = 0;

    for (cur = &service->characteristics; *cur; cur = &(*cur)->next);
    *cur = characteristic;

    return characteristic;
}

ble_characteristic_t *ble_device_characteristic_find_by_uuid(
    ble_service_t *service, ble_uuid_t uuid)
{
    ble_characteristic_t *cur;

    for (cur = service->characteristics; cur; cur = cur->next)
    {
        if (!memcmp(cur->uuid, uuid, sizeof(ble_uuid_t)))
            break;
    }

    return cur;
}

ble_characteristic_t *ble_device_characteristic_find_by_handle(
    ble_service_t *service, uint16_t handle)
{
    ble_characteristic_t *cur;

    for (cur = service->characteristics; cur; cur = cur->next)
    {
        if (cur->handle == handle)
            break;
    }

    return cur;
}

void ble_device_characteristic_free(ble_characteristic_t *characteristic)
{
    free(characteristic);
}

void ble_device_characteristics_free(ble_characteristic_t **list)
{
    ble_characteristic_t *cur, **head = list;

    while (*list)
    {
        cur = *list;
        *list = cur->next;
        ble_device_characteristic_free(cur);
    }
    *head = NULL;
}

int ble_device_info_get_by_conn_id_handle(ble_device_t *list, uint16_t conn_id,
    uint16_t handle, ble_device_t **device, ble_service_t **service,
    ble_characteristic_t **characteristic)
{
    if (!(*device = ble_device_find_by_conn_id(list, conn_id)))
        return -1;
    
    for (*service = (*device)->services; *service; *service = (*service)->next)
    {
        for (*characteristic = (*service)->characteristics; *characteristic;
            *characteristic = (*characteristic)->next)
        {
            if ((*characteristic)->handle == handle)
                return 0;
        }
    }

    return -1;
}
