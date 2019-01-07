#include "broadcasters.h"
#include "ble_utils.h"
#include <esp_log.h>
#include <esp_gap_ble_api.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>

/* Constants */
static const char *TAG = "Broadcaster";

/* Utilities */
static char *hex2a(char *s, uint8_t *buf, size_t len)
{
    int i;
    char *p;

    for (i = 0, p = s; i < len; i++)
        p += sprintf(p, "%02x", buf[i]);

    return s;
}

/* UUID's in big-endian (compared to uuidtoa()) */
static char *_uuidtoa(ble_uuid_t uuid)
{
    static char s[37];

    sprintf(s,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid[0], uuid[1], uuid[2], uuid[3],
        uuid[4], uuid[5],
        uuid[6], uuid[7],
        uuid[8], uuid[9],
        uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

    return s;
}

/* iBeacon */
typedef struct {
    uint16_t company_id;
    uint16_t beacon_type;
    uint8_t proximity_uuid[16];
    uint16_t major;
    uint16_t minor;
    int8_t measured_power;
} __attribute__((packed)) ibeacon_t;

static ibeacon_t *ibeacon_data_get(uint8_t *adv_data, uint8_t adv_data_len,
    uint8_t *ibeacon_len)
{
    uint8_t len;
    uint8_t *data = esp_ble_resolve_adv_data(adv_data,
        ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE, &len);

    if (ibeacon_len)
        *ibeacon_len = len;

    return (ibeacon_t *)data;
}

static int ibeacon_is_broadcaster(uint8_t *adv_data, size_t adv_data_len)
{
    uint8_t len;
    ibeacon_t *beacon = ibeacon_data_get(adv_data, adv_data_len, &len);

    if (!beacon || len != sizeof(ibeacon_t))
        return 0;

    /* Technically, we should also check the device is BLE only and
     * non-connectable, but most iBeacon simulators don't advertise as such */
    return le16toh(beacon->company_id) == 0x004C /* Apple */ &&
        le16toh(beacon->beacon_type) == 0x1502;
}

static void ibeacon_metadata_get(uint8_t *adv_data, size_t adv_data_len,
    int rssi, broadcaster_meta_data_cb_t cb, void *ctx)
{
    char s[5];
    ibeacon_t *beacon = ibeacon_data_get(adv_data, adv_data_len, NULL);

    cb("UUID", _uuidtoa(beacon->proximity_uuid), ctx);
    sprintf(s, "%u", be16toh(beacon->major));
    cb("Major", s, ctx);
    sprintf(s, "%u", be16toh(beacon->minor));
    cb("Minor", s, ctx);
    sprintf(s, "%.2f", pow(10, (beacon->measured_power - rssi) / 20.0));
    cb("Distance", s, ctx);
}

static broadcaster_ops_t ibeacon_ops = {
    .name = "iBeacon",
    .is_broadcaster = ibeacon_is_broadcaster,
    .metadata_get = ibeacon_metadata_get,
};

/* Eddystone */
#define EDDYSTONE_SERVICE_UUID 0xFEAA
#define EDDYSTONE_FRAME_TYPE_UID 0x00
#define EDDYSTONE_FRAME_TYPE_URL 0x10
#define EDDYSTONE_FRAME_TYPE_TLM 0x20

typedef struct {
    int8_t ranging_data; /* Calibrated Tx power at 0 m */
    uint8_t nid[10]; /* Namespace */
    uint8_t bid[6]; /* Instance */
    uint8_t rfu[2]; /* Reserved for future use */
} __attribute__((packed)) eddystone_uid_t;

typedef struct {
    int8_t tx_power; /* Calibrated Tx power at 0 m */
    uint8_t url_scheme; /* Encoded Scheme Prefix */
    uint8_t url[0]; /* Length 1-17 */
} __attribute__((packed)) eddystone_url_t;

typedef struct {
    int8_t version;
    uint16_t vbatt; /* Battery voltage, 1mV/bit */
    uint16_t temp; /* Beacon temperature */
    uint32_t adv_cnt; /* Advertising PDU count */
    uint32_t sec_cnt; /* Time since power-on or reboot */
} __attribute__((packed)) eddystone_tlm_t;

typedef struct {
    uint16_t service_uuid;
    uint8_t frame_type;
    union {
        eddystone_uid_t uid;
        eddystone_url_t url;
        eddystone_tlm_t tlm;
    } u;
} eddystone_t;

static eddystone_t *eddystone_data_get(uint8_t *adv_data, uint8_t adv_data_len,
    uint8_t *eddystone_len)
{
    uint8_t len;
    uint8_t *data = esp_ble_resolve_adv_data(adv_data,
        ESP_BLE_AD_TYPE_SERVICE_DATA, &len);

    if (*eddystone_len)
        *eddystone_len = len;

    return (eddystone_t *)data;
}

static int eddystone_is_broadcaster(uint8_t *adv_data, size_t adv_data_len)
{
    uint8_t len;
    uint8_t *data = esp_ble_resolve_adv_data(adv_data,
        ESP_BLE_AD_TYPE_16SRV_CMPL, &len);
    eddystone_t *eddystone;

    if (!data || len != 2 ||
        le16toh(*(uint16_t *)data) != EDDYSTONE_SERVICE_UUID)
    {
        return NULL;
    }

    eddystone = eddystone_data_get(adv_data, adv_data_len, &len);

    /* Make sure we have enough bytes to read UUID and type */
    if (!eddystone || len < offsetof(eddystone_t, u) ||
        le16toh(eddystone->service_uuid) != EDDYSTONE_SERVICE_UUID)
    {
        return NULL;
    }

    /* Validate length */
    if (eddystone->frame_type == EDDYSTONE_FRAME_TYPE_UID)
    {
        if (len - offsetof(eddystone_t, u) != sizeof(eddystone_uid_t) &&
            /* RFU is not always available */
            len - offsetof(eddystone_t, u) != offsetof(eddystone_uid_t, rfu))
        {
            return 0;
        }
    }
    else if (eddystone->frame_type == EDDYSTONE_FRAME_TYPE_URL)
    {
        if (len - offsetof(eddystone_t, u) < sizeof(eddystone_url_t))
            return 0;
    }
    else if (eddystone->frame_type == EDDYSTONE_FRAME_TYPE_TLM)
    {
        if (len - offsetof(eddystone_t, u) != sizeof(eddystone_tlm_t))
            return 0;
    }
    else
        return 0; /* Unsupported frame type */

    return 1;
}

static char *eddystone_url_scheme_get(uint8_t url_scheme)
{
    if (url_scheme == 0) return "http://www.";
    if (url_scheme == 1) return "https://www.";
    if (url_scheme == 2) return "http://";
    if (url_scheme == 3) return "https://";

    ESP_LOGE(TAG, "Unsupported URL scheme: %d", url_scheme);
    return "";
}

static char *eddystone_url_get(char url)
{
    static char c;

    c = url;
    if (c == 0) return ".com/";
    if (c == 1) return ".org/";
    if (c == 2) return ".edu/";
    if (c == 3) return ".net/";
    if (c == 4) return ".info/";
    if (c == 5) return ".biz/";
    if (c == 6) return ".gov/";
    if (c == 7) return ".com";
    if (c == 8) return ".org";
    if (c == 9) return ".edu";
    if (c == 10) return ".net";
    if (c == 11) return ".info";
    if (c == 12) return ".biz";
    if (c == 13) return ".gov";
    if (c > 32 && c < 127)
        return &c;

    ESP_LOGE(TAG, "Unsupported URL character: 0x%0x", url);
    return "";
}

static void eddystone_metadata_get(uint8_t *adv_data, size_t adv_data_len,
    int rssi, broadcaster_meta_data_cb_t cb, void *ctx)
{
    char s[30];
    uint8_t len;
    eddystone_t *eddystone = eddystone_data_get(adv_data, adv_data_len, &len);

    if (eddystone->frame_type == EDDYSTONE_FRAME_TYPE_UID)
    {
        /* Note: 41dBm is the signal loss that occurs over 1 meter */
        sprintf(s, "%.2f",
            pow(10, (eddystone->u.uid.ranging_data - rssi - 41) / 20.0));
        cb("Distance", s, ctx);
        cb("Namespace", hex2a(s, eddystone->u.uid.nid, 10), ctx);
        cb("Instance", hex2a(s, eddystone->u.uid.bid, 6), ctx);
    }
    else if (eddystone->frame_type == EDDYSTONE_FRAME_TYPE_URL)
    {
        char *p = s;
        int i;

        /* Note: 41dBm is the signal loss that occurs over 1 meter */
        sprintf(s, "%.2f",
            pow(10, (eddystone->u.url.tx_power - rssi - 41) / 20.0));
        cb("Distance", s, ctx);

        p += sprintf(p, "%s",
            eddystone_url_scheme_get(eddystone->u.url.url_scheme));
        /* Calculate remaining size of URL */
        len -= offsetof(eddystone_t, u) + offsetof(eddystone_url_t, url);
        for (i = 0; len > 0; len--, i++)
            p += sprintf(p, "%s", eddystone_url_get(eddystone->u.url.url[i]));
        cb("URL", s, ctx);
    }
    else if (eddystone->frame_type == EDDYSTONE_FRAME_TYPE_TLM)
    {
        if (eddystone->u.tlm.version == 0)
        {
            sprintf(s, "%d", be16toh(eddystone->u.tlm.vbatt));
            cb("Voltage", s, ctx);
            sprintf(s, "%.2f", be16toh(eddystone->u.tlm.temp) / 256.0);
            cb("Temperature", s, ctx);
            sprintf(s, "%u", be32toh(eddystone->u.tlm.adv_cnt));
            cb("Count", s, ctx);
            sprintf(s, "%u", be32toh(eddystone->u.tlm.sec_cnt));
            cb("Uptime", s, ctx);
        }
        else
        {
            ESP_LOGE(TAG, "Unsupported TLM verison %d",
                eddystone->u.tlm.version);
        }
    }
}

static broadcaster_ops_t eddystone_ops = {
    .name = "Eddystone",
    .is_broadcaster = eddystone_is_broadcaster,
    .metadata_get = eddystone_metadata_get,
};

/* Common */
static broadcaster_ops_t *broadcaster_ops[] = {
    &ibeacon_ops,
    &eddystone_ops,
    NULL
};

broadcaster_ops_t *broadcaster_ops_get(uint8_t *adv_data, size_t adv_data_len)
{
    broadcaster_ops_t **ops;

    for (ops = broadcaster_ops; *ops; ops++)
    {
        if ((*ops)->is_broadcaster(adv_data, adv_data_len))
            return (*ops);
    }

    return NULL;
}
