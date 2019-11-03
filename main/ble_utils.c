#include "ble_utils.h"
#include "config.h"
#include "gatt.h"
#include <math.h>
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
    CASE_STR(ESP_GATTC_READ_MULTIPLE_EVT);
    CASE_STR(ESP_GATTC_QUEUE_FULL_EVT);
    default: return "Invalid GATTC event";
    }
}
#undef CASE_STR

char *mactoa(mac_addr_t mac)
{
    static char s[18];

    sprintf(s, MAC_FMT, MAC_PARAM(mac));

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

    sprintf(s, UUID_FMT, UUID_PARAM(uuid));

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

static service_desc_t *ble_get_sig_service(ble_uuid_t uuid)
{
    service_desc_t *p;

    for (p = services; p->name; p++)
    {
        if (memcmp(p->uuid, uuid, sizeof(ble_uuid_t)))
            continue;

        return p;
    }

    return NULL;
}

static characteristic_desc_t *ble_get_sig_characteristic(ble_uuid_t uuid)
{
    characteristic_desc_t *p;

    for (p = characteristics; p->name; p++)
    {
        if (memcmp(p->uuid, uuid, sizeof(ble_uuid_t)))
            continue;

        return p;
    }

    return NULL;
}

static characteristic_type_t *ble_get_sig_characteristic_types(ble_uuid_t uuid)
{
    characteristic_desc_t *c = ble_get_sig_characteristic(uuid);
    return c ? c->types : NULL;
}

static characteristic_type_t ble_atotype(const char *type)
{
    struct {
        const char *name;
        int type;
    } *p, types[] = {
        { "boolean", CHAR_TYPE_BOOLEAN },
        { "2bit", CHAR_TYPE_2BIT },
        { "4bit", CHAR_TYPE_4BIT },
        { "nibble", CHAR_TYPE_NIBBLE },
        { "8bit", CHAR_TYPE_8BIT },
        { "uint8", CHAR_TYPE_UINT8 },
        { "sint8", CHAR_TYPE_SINT8 },
        { "uint12", CHAR_TYPE_UINT12 },
        { "16bit", CHAR_TYPE_16BIT },
        { "uint16", CHAR_TYPE_UINT16 },
        { "sint16", CHAR_TYPE_SINT16 },
        { "24bit", CHAR_TYPE_24BIT },
        { "uint24", CHAR_TYPE_UINT24 },
        { "sint24", CHAR_TYPE_SINT24 },
        { "32bit", CHAR_TYPE_32BIT },
        { "uint32", CHAR_TYPE_UINT32 },
        { "sint32", CHAR_TYPE_SINT32 },
        { "uint40", CHAR_TYPE_UINT40 },
        { "uint48", CHAR_TYPE_UINT48 },
        { "uint128", CHAR_TYPE_UINT128 },
        { "utf8s", CHAR_TYPE_UTF8S },
        { "float64", CHAR_TYPE_FLOAT64 },
        { "sfloat", CHAR_TYPE_SFLOAT },
        { "float", CHAR_TYPE_FLOAT },
        { "reg-cert-data-list", CHAR_TYPE_REG_CERT_DATA_LIST },
        { "variable", CHAR_TYPE_VARIABLE },
        { "gatt-uuid", CHAR_TYPE_GATT_UUID },
        { NULL, CHAR_TYPE_UNKNOWN },
    };

    for (p = types; p->name; p++)
    {
        if (!strcmp(p->name, type))
            break;
    }

    return p->type;
}

static characteristic_type_t *ble_get_characteristic_types(ble_uuid_t uuid)
{
    static characteristic_type_t ret[32];
    int i = 0;
    const char **iter, **conf_types =
        config_ble_characteristic_types_get(uuidtoa(uuid));

    if (!conf_types)
        return ble_get_sig_characteristic_types(uuid);

    for (iter = conf_types; *iter; iter++)
        ret[i++] = ble_atotype(*iter);
    ret[i] = -1;

    return ret;
}

static size_t ble_type_size(characteristic_type_t type)
{
    switch (type)
    {
    case CHAR_TYPE_BOOLEAN:
    case CHAR_TYPE_2BIT:
    case CHAR_TYPE_4BIT:
    case CHAR_TYPE_NIBBLE:
    case CHAR_TYPE_8BIT:
    case CHAR_TYPE_UINT8:
    case CHAR_TYPE_SINT8:
        return 1;
    case CHAR_TYPE_UINT12:
    case CHAR_TYPE_16BIT:
    case CHAR_TYPE_UINT16:
    case CHAR_TYPE_SINT16:
    case CHAR_TYPE_SFLOAT:
        return 2;
    case CHAR_TYPE_24BIT:
    case CHAR_TYPE_UINT24:
    case CHAR_TYPE_SINT24:
        return 3;
    case CHAR_TYPE_32BIT:
    case CHAR_TYPE_UINT32:
    case CHAR_TYPE_SINT32:
    case CHAR_TYPE_FLOAT:
        return 4;
    case CHAR_TYPE_UINT40:
        return 5;
    case CHAR_TYPE_UINT48:
        return 6;
    case CHAR_TYPE_FLOAT64:
        return 8;
    case CHAR_TYPE_UTF8S:
        /* String length are whatever is left in the payload, fall-through */
    case CHAR_TYPE_UINT128:
    case CHAR_TYPE_REG_CERT_DATA_LIST:
    case CHAR_TYPE_VARIABLE:
    case CHAR_TYPE_GATT_UUID:
    case CHAR_TYPE_UNKNOWN:
        return 0;
    }

    return 0;
}

char *chartoa(ble_uuid_t uuid, const uint8_t *data, size_t len)
{
    characteristic_type_t *types = ble_get_characteristic_types(uuid);
    static char buf[1024];
    char *p = buf;
    int i = 0;

    /* A note from the Bluetooth specification:
     * If a format is not a whole number of octets, then the data shall be
     * contained within the least significant bits of the value, and all other
     * bits shall be set to zero on transmission and ignored upon receipt. If
     * the Characteristic Value is less than an octet, it occupies an entire
     * octet.
     */
    for (; types && *types != -1; types++)
    {
        size_t type_len = ble_type_size(*types);

        if (len - i < type_len)
            break;

        switch (*types)
        {
        case CHAR_TYPE_BOOLEAN:
            p += sprintf(p, "%s,", data[i] & 0x01 ? "true" : "false");
            break;
        case CHAR_TYPE_2BIT:
            p += sprintf(p, "%hhu,", data[i] & 0x03);
            break;
        case CHAR_TYPE_4BIT:
        case CHAR_TYPE_NIBBLE:
            p += sprintf(p, "%hhu,", data[i] & 0x0F);
            break;
        case CHAR_TYPE_8BIT:
        case CHAR_TYPE_UINT8:
        case CHAR_TYPE_SINT8:
            if (*types == CHAR_TYPE_SINT8)
                p += sprintf(p, "%hhd,", data[i]);
            else
                p += sprintf(p, "%hhu,", data[i]);

            break;
        case CHAR_TYPE_UINT12:
        {
            uint16_t tmp = (data[i + 1] << 8) | data[i];

            p += sprintf(p, "%hu,", tmp & 0x0FFF);
            break;
        }
        case CHAR_TYPE_16BIT:
        case CHAR_TYPE_UINT16:
        case CHAR_TYPE_SINT16:
        {
            uint16_t tmp = (data[i + 1] << 8) | data[i];

            if (*types == CHAR_TYPE_SINT16)
                p += sprintf(p, "%hd,", tmp);
            else
                p += sprintf(p, "%hu,", tmp);

            break;
        }
        case CHAR_TYPE_24BIT:
        case CHAR_TYPE_UINT24:
        case CHAR_TYPE_SINT24:
        {
            uint32_t tmp = (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];

            if (*types == CHAR_TYPE_SINT24)
                p += sprintf(p, "%d,", (int32_t)tmp << 8 >> 8);
            else
                p += sprintf(p, "%u,", tmp);

            break;
        }
        case CHAR_TYPE_32BIT:
        case CHAR_TYPE_UINT32:
        case CHAR_TYPE_SINT32:
        {
            uint32_t tmp = (data[i + 3] << 24) | (data[i + 2] << 16) |
                (data[i + 1] << 8) | data[i];

            if (*types == CHAR_TYPE_SINT32)
                p += sprintf(p, "%d,", tmp);
            else
                p += sprintf(p, "%u,", tmp);

            break;
        }
        case CHAR_TYPE_UINT40:
        {
            uint64_t tmp = ((uint64_t)data[i + 4] << 32) | (data[i + 3] << 24) |
                (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];

            p += sprintf(p, "%llu,", tmp);

            break;
        }
        case CHAR_TYPE_UINT48:
        {
            uint64_t tmp = ((uint64_t)data[i + 5] << 40) |
                ((uint64_t)data[i + 4] << 32) | (data[i + 3] << 24) |
                (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];

            p += sprintf(p, "%llu,", tmp);

            break;
        }
        /* String values consume the rest of the buffer */
        case CHAR_TYPE_UTF8S:
        {
            int c = len - i;

            memcpy(p, &data[i], c);

            p += c;
            i += c;
            p += sprintf(p, ",");
            break;
        }
        /* IEEE-754 floating point format */
        /* Note, ESP-32 is little endian, as is the characteristic value */
        case CHAR_TYPE_FLOAT64:
        {
            union {
                double d;
                uint8_t b[8];
            } tmp;
            memcpy(&tmp.b, &data[i], 8);

            p += sprintf(p, "%f,", tmp.d);
            break;
        }
        /* IEEE-11073 floating point format */
        case CHAR_TYPE_SFLOAT:
        {
            uint16_t tmp = (data[i + 1] << 8) | data[i];
            int16_t mantissa = tmp & 0X0FFF;
            int8_t exponent = (tmp >> 12) & 0x0F;

            /* Fix sign */
            if (exponent >= 0x0008)
                exponent = -((0x000F + 1) - exponent);
            if (mantissa >= 0x0800)
                mantissa = -((0x0FFF + 1) - mantissa);

            p += sprintf(p, "%f,", mantissa * pow(10.0f, exponent));
            break;
        }
        case CHAR_TYPE_FLOAT:
        {
            int8_t exponent = data[i + 3];
            int32_t mantissa = ((data[i + 2] << 24) | (data[i + 1] << 16) |
                 data[i] << 8) >> 8;

            /* Fix sign */
            if (mantissa >= 0x800000)
                mantissa = -((0xFFFFFF + 1) - mantissa);

            p += sprintf(p, "%f,", mantissa * pow(10.0f, exponent));
            break;
        }
        case CHAR_TYPE_UINT128:
        case CHAR_TYPE_REG_CERT_DATA_LIST:
        case CHAR_TYPE_VARIABLE:
        case CHAR_TYPE_GATT_UUID:
        case CHAR_TYPE_UNKNOWN:
            printf(">>>> Unhandled characteristic type %d <<<<\n", *types);
        }

        i += type_len;
    }

    for (; i < len; i++)
        p += sprintf(p, "%u,", data[i]);

    *(p - 1) = '\0';
    return buf;
}

uint8_t *atochar(ble_uuid_t uuid, const char *data, size_t len, size_t *ret_len)
{
    characteristic_type_t *types = ble_get_characteristic_types(uuid);
    static uint8_t buf[512];
    uint8_t *p = buf;
    char *str = strndup(data, len);
    char *val = strtok(str, ",");

    for (; types && *types != -1 && val; types++)
    {
        switch (*types)
        {
        case CHAR_TYPE_BOOLEAN:
            *p = !strcmp(val, "true") ? 1 : 0;
            p += 1;
            break;
        case CHAR_TYPE_2BIT:
            *p = strtoul(val, NULL, 10) & 0x03;
            p += 1;
            break;
        case CHAR_TYPE_4BIT:
        case CHAR_TYPE_NIBBLE:
            *p = strtoul(val, NULL, 10) & 0x0F;
            p += 1;
            break;
        case CHAR_TYPE_8BIT:
        case CHAR_TYPE_UINT8:
        case CHAR_TYPE_SINT8:
            if (*types == CHAR_TYPE_SINT8)
                *p = strtol(val, NULL, 10);
            else
                *p = strtoul(val, NULL, 10);
            p += 1;
            break;
        case CHAR_TYPE_UINT12:
        {
            uint16_t tmp = strtoul(val, NULL, 10) & 0x0FFF;

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;

            p += 2;
            break;
        }
        case CHAR_TYPE_16BIT:
        case CHAR_TYPE_UINT16:
        {
            uint16_t tmp = strtoul(val, NULL, 10) & 0xFFFF;

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;

            p += 2;
            break;
        }
        case CHAR_TYPE_SINT16:
        {
            uint16_t tmp = strtol(val, NULL, 10) & 0xFFFF;

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;

            p += 2;
            break;
        }
        case CHAR_TYPE_24BIT:
        case CHAR_TYPE_UINT24:
        {
            uint32_t tmp = strtoul(val, NULL, 10) & 0x00FFFFFF;

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;
            *(p + 2) = (tmp >> 16) & 0xFF;

            p += 3;
            break;
        }
        case CHAR_TYPE_SINT24:
        {
            uint32_t tmp = strtol(val, NULL, 10) & 0x00FFFFFF;

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;
            *(p + 2) = (tmp >> 16) & 0xFF;

            p += 3;
            break;
        }
        case CHAR_TYPE_32BIT:
        {
            uint32_t tmp = strtol(val, NULL, 10);

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;
            *(p + 2) = (tmp >> 16) & 0xFF;
            *(p + 3) = (tmp >> 24) & 0xFF;

            p += 4;
            break;
        }
        case CHAR_TYPE_UINT32:
        case CHAR_TYPE_SINT32:
        {
            uint32_t tmp = strtoul(val, NULL, 10);

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;
            *(p + 2) = (tmp >> 16) & 0xFF;
            *(p + 3) = (tmp >> 24) & 0xFF;

            p += 4;
            break;
        }
        case CHAR_TYPE_UINT40:
        {
            uint64_t tmp = strtoul(val, NULL, 10);

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;
            *(p + 2) = (tmp >> 16) & 0xFF;
            *(p + 3) = (tmp >> 24) & 0xFF;
            *(p + 4) = (tmp >> 32) & 0xFF;

            p += 5;
            break;
        }
        case CHAR_TYPE_UINT48:
        {
            uint64_t tmp = strtoul(val, NULL, 10);

            *p = tmp & 0xFF;
            *(p + 1) = (tmp >> 8) & 0xFF;
            *(p + 2) = (tmp >> 16) & 0xFF;
            *(p + 3) = (tmp >> 24) & 0xFF;
            *(p + 4) = (tmp >> 32) & 0xFF;
            *(p + 5) = (tmp >> 40) & 0xFF;

            p += 6;
            break;
        }
        /* String values consume the rest of the buffer */
        case CHAR_TYPE_UTF8S:
        {
            size_t len = strlen(val);

            strcpy((char *)p, val);

            p += len + 1;
            break;
        }
        /* IEEE-754 floating point format */
        /* Note, ESP-32 is little endian, as is the characteristic value */
        case CHAR_TYPE_FLOAT64:
        {
            union {
                double d;
                uint8_t b[8];
            } tmp;
            tmp.d = strtod(val, NULL);

            memcpy(p, tmp.b, 8);

            p += 8;
            break;
        }
        case CHAR_TYPE_UINT128:
        /* IEEE-11073 floating point format */
        case CHAR_TYPE_SFLOAT:
        case CHAR_TYPE_FLOAT:
        case CHAR_TYPE_REG_CERT_DATA_LIST:
        case CHAR_TYPE_VARIABLE:
        case CHAR_TYPE_GATT_UUID:
        case CHAR_TYPE_UNKNOWN:
            printf(">>>> Unhandled characteristic type %d <<<<\n", *types);
        }

        val = strtok(NULL, ",");
    }

    while (val)
    {
        *p = strtoul(val, NULL, 10);
        p += 1;
        val = strtok(NULL, ",");
    }

    free(str);

    *ret_len = p - buf;
    return buf;
}

static const char *ble_get_sig_service_name(ble_uuid_t uuid)
{
    service_desc_t *p = ble_get_sig_service(uuid);

    return p ? p->name : NULL;
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
    characteristic_desc_t *p = ble_get_sig_characteristic(uuid);

    return p ? p->name : NULL;
}

const char *ble_characteristic_name_get(ble_uuid_t uuid)
{
    const char *name = config_ble_characteristic_name_get(uuidtoa(uuid));

    if (name)
        return name;

    return ble_get_sig_characteristic_name(uuid) ? : uuidtoa(uuid);
}

ble_device_t *ble_device_add(ble_device_t **list, mac_addr_t mac,
    esp_ble_addr_type_t addr_type, uint16_t conn_id)
{
    ble_device_t *dev, **cur;

    dev = calloc(1, sizeof(*dev));
    memcpy(dev->mac, mac, sizeof(mac_addr_t));
    dev->addr_type = addr_type;
    dev->conn_id = conn_id;

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

void ble_device_remove_disconnected(ble_device_t **list)
{
    ble_device_t *tmp, **cur = list;

    while (*cur)
    {
        if ((*cur)->conn_id == 0xffff)
        {
            tmp = *cur;
            *cur = (*cur)->next;
            ble_device_free(tmp);
        }
        else
            cur = &(*cur)->next;
    }
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
