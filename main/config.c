#include "config.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_partition.h>
#include <esp_spiffs.h>
#include <nvs.h>
#include <cJSON.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Types */
typedef struct config_update_handle_t {
    const esp_partition_t *partition;
    uint8_t partition_id;
    size_t bytes_written;
} config_update_handle_t;

/* Constants */
static const char *TAG = "Config";
static const char *config_file_name = "/spiffs/config.json";
static const char *nvs_namespace = "ble2mqtt_config";
static const char *nvs_active_partition = "active_part";
static cJSON *config;

/* Internal variables */
static char config_version[65];
static nvs_handle nvs;

/* Common utilities */
static char *read_file(const char *path)
{
    int fd, len;
    struct stat st;
    char *buf, *p;

    if (stat(path, &st))
        return NULL;

    if ((fd = open(path, O_RDONLY)) < 0)
        return NULL;

    if (!(buf = p = malloc(st.st_size + 1)))
        return NULL;

    while ((len = read(fd, p, 1024)) > 0)
        p += len;
    close(fd);

    if (len < 0)
    {
        free(buf);
        return NULL;
    }

    *p = '\0';
    return buf;
}

/* BLE Configuration*/
static cJSON *config_ble_get_name_by_uuid(uint8_t is_service,
    const char *uuid, const char *field_name)
{
    cJSON *ble = cJSON_GetObjectItemCaseSensitive(config, "ble");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(ble,
        is_service ?  "services" : "characteristics");
    cJSON *list = cJSON_GetObjectItemCaseSensitive(type, "definitions");

    /* Check config.json for override values */
    cJSON *obj = cJSON_GetObjectItem(list, uuid);
    cJSON *field = cJSON_GetObjectItemCaseSensitive(obj, field_name);

    return field;
}

const char *config_ble_service_name_get(const char *uuid)
{
    cJSON *name = config_ble_get_name_by_uuid(1, uuid, "name");

    if (cJSON_IsString(name))
        return name->valuestring;

    return NULL;
}

const char *config_ble_characteristic_name_get(const char *uuid)
{
    cJSON *name = config_ble_get_name_by_uuid(0, uuid, "name");

    if (cJSON_IsString(name))
        return name->valuestring;

    return NULL;
}

const char **config_ble_characteristic_types_get(const char *uuid)
{
    cJSON *types = config_ble_get_name_by_uuid(0, uuid, "types");
    static char **ret = NULL;
    int i, size;

    if (ret)
    {
        free(ret);
        ret = NULL;
    }

    if (!cJSON_IsArray(types))
        return NULL;

    size = cJSON_GetArraySize(types);
    ret = malloc(sizeof(char *) * (size + 1));
    for (i = 0; i < size; i++)
    {
        cJSON *type = cJSON_GetArrayItem(types, i);
        ret[i] = type->valuestring;
    }
    ret[size] = NULL;

    return (const char **)ret;
}

int match_wildcard(const char *fmt, const char *str)
{
    while(*fmt &&
        (tolower((uint8_t)*fmt) == tolower((uint8_t)*str) || *fmt == '?'))
    {
        fmt++;
        str++;
    }

    return *fmt == *str || *fmt == '?';
}

cJSON *json_find_in_array(cJSON *arr, const char *item)
{
    cJSON *cur;

    if (!arr)
        return NULL;

    for (cur = arr->child; cur; cur = cur->next)
    {
        if (cJSON_IsString(cur) && match_wildcard(cur->valuestring, item))
            return cur;
    }

    return NULL;
}

static uint8_t json_is_in_lists(cJSON *base, const char *item)
{
    cJSON *whitelist = cJSON_GetObjectItemCaseSensitive(base, "whitelist");
    cJSON *blacklist = cJSON_GetObjectItemCaseSensitive(base, "blacklist");
    uint8_t action = whitelist ? 1 : 0;
    cJSON *list = whitelist ? : blacklist;

    /* No list was defined, accept all */
    if (!list)
        return 1;

    return json_find_in_array(list, item) ? action : !action;
}

uint8_t config_ble_characteristic_should_include(const char *uuid)
{
    cJSON *ble = cJSON_GetObjectItemCaseSensitive(config, "ble");
    cJSON *characteristics = cJSON_GetObjectItemCaseSensitive(ble,
        "characteristics");

    return json_is_in_lists(characteristics, uuid);
}

uint8_t config_ble_service_should_include(const char *uuid)
{
    cJSON *ble = cJSON_GetObjectItemCaseSensitive(config, "ble");
    cJSON *services = cJSON_GetObjectItemCaseSensitive(ble, "services");

    return json_is_in_lists(services, uuid);
}

uint8_t config_ble_should_connect(const char *mac)
{
    cJSON *ble = cJSON_GetObjectItemCaseSensitive(config, "ble");
    return json_is_in_lists(ble, mac);
}

uint32_t config_ble_passkey_get(const char *mac)
{
    cJSON *ble = cJSON_GetObjectItemCaseSensitive(config, "ble");
    cJSON *passkeys = cJSON_GetObjectItemCaseSensitive(ble, "passkeys");
    cJSON *key;

    if (!passkeys)
        return 0;

    for (key = passkeys->child; key; key = key->next)
    {
        if (cJSON_IsNumber(key) && match_wildcard(key->string, mac))
            return key->valuedouble;
    }

    return 0;
}

/* Ethernet Configuration */
const char *config_eth_clk_mode_get(void)
{
    cJSON *eth = cJSON_GetObjectItemCaseSensitive(config, "eth");
    cJSON *clk_mode = cJSON_GetObjectItemCaseSensitive(eth, "clk_mode");

    if (cJSON_IsString(clk_mode))
        return clk_mode->valuestring;

    return NULL;
}

const char *config_eth_phy_get(void)
{
    cJSON *eth = cJSON_GetObjectItemCaseSensitive(config, "eth");
    cJSON *phy = cJSON_GetObjectItemCaseSensitive(eth, "phy");

    if (cJSON_IsString(phy))
        return phy->valuestring;

    return NULL;
}

uint8_t config_eth_phy_gpio_power_get(void)
{
    cJSON *eth = cJSON_GetObjectItemCaseSensitive(config, "eth");
    cJSON *phy_gpio_power = cJSON_GetObjectItemCaseSensitive(eth, "phy_gpio_power");

    if (cJSON_IsNumber(phy_gpio_power))
        return phy_gpio_power->valuedouble;

    return 0;
}

/* MQTT Configuration*/
const char *config_mqtt_server_get(const char *param_name)
{
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(config, "mqtt");
    cJSON *server = cJSON_GetObjectItemCaseSensitive(mqtt, "server");
    cJSON *param = cJSON_GetObjectItemCaseSensitive(server, param_name);

    if (cJSON_IsString(param))
        return param->valuestring;

    return NULL;
}

const char *config_mqtt_host_get(void)
{
    return config_mqtt_server_get("host");
}

uint16_t config_mqtt_port_get(void)
{
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(config, "mqtt");
    cJSON *server = cJSON_GetObjectItemCaseSensitive(mqtt, "server");
    cJSON *port = cJSON_GetObjectItemCaseSensitive(server, "port");

    if (cJSON_IsNumber(port))
        return port->valuedouble;

    return 0;
}

uint8_t config_mqtt_ssl_get(void)
{
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(config, "mqtt");
    cJSON *server = cJSON_GetObjectItemCaseSensitive(mqtt, "server");
    cJSON *ssl = cJSON_GetObjectItemCaseSensitive(server, "ssl");

    return cJSON_IsTrue(ssl);
}

const char *config_mqtt_file_get(const char *field)
{
    const char *file = config_mqtt_server_get(field);
    char buf[128];

    if (!file)
        return NULL;

    snprintf(buf, sizeof(buf), "/spiffs%s", file);
    return read_file(buf);
}

const char *config_mqtt_server_cert_get(void)
{
    static const char *cert;

    if (!cert)
        cert = config_mqtt_file_get("server_cert");

    return cert;
}

const char *config_mqtt_client_cert_get(void)
{
    static const char *cert;

    if (!cert)
        cert = config_mqtt_file_get("client_cert");

    return cert;
}

const char *config_mqtt_client_key_get(void)
{
    static const char *key;

    if (!key)
        key = config_mqtt_file_get("client_key");

    return key;
}

const char *config_mqtt_client_id_get(void)
{
    return config_mqtt_server_get("client_id");
}

const char *config_mqtt_username_get(void)
{
    return config_mqtt_server_get("username");
}

const char *config_mqtt_password_get(void)
{
    return config_mqtt_server_get("password");
}

uint8_t config_mqtt_qos_get(void)
{
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(config, "mqtt");
    cJSON *publish = cJSON_GetObjectItemCaseSensitive(mqtt, "publish");
    cJSON *qos = cJSON_GetObjectItemCaseSensitive(publish, "qos");

    if (cJSON_IsNumber(qos))
        return qos->valuedouble;

    return 0;
}

uint8_t config_mqtt_retained_get(void)
{
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(config, "mqtt");
    cJSON *publish = cJSON_GetObjectItemCaseSensitive(mqtt, "publish");
    cJSON *retain = cJSON_GetObjectItemCaseSensitive(publish, "retain");

    return cJSON_IsTrue(retain);
}

const char *config_mqtt_topics_get(const char *param_name, const char *def)
{
    cJSON *mqtt = cJSON_GetObjectItemCaseSensitive(config, "mqtt");
    cJSON *topics = cJSON_GetObjectItemCaseSensitive(mqtt, "topics");
    cJSON *param = cJSON_GetObjectItemCaseSensitive(topics, param_name);

    if (cJSON_IsString(param))
        return param->valuestring;

    return def;
}

const char *config_mqtt_prefix_get(void)
{
    return config_mqtt_topics_get("prefix", "");
}

const char *config_mqtt_get_suffix_get(void)
{
    return config_mqtt_topics_get("get_suffix", "/Get");
}

const char *config_mqtt_set_suffix_get(void)
{
    return config_mqtt_topics_get("set_suffix", "/Set");
}

/* Network Configuration */
config_network_type_t config_network_type_get(void)
{
    cJSON *eth = cJSON_GetObjectItemCaseSensitive(config, "eth");

    return eth ? NETWORK_TYPE_ETH : NETWORK_TYPE_WIFI;
}

/* WiFi Configuration */
const char *config_wifi_ssid_get(void)
{
    cJSON *wifi = cJSON_GetObjectItemCaseSensitive(config, "wifi");
    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(wifi, "ssid");

    if (cJSON_IsString(ssid))
        return ssid->valuestring;

    return NULL;
}

const char *config_wifi_password_get(void)
{
    cJSON *wifi = cJSON_GetObjectItemCaseSensitive(config, "wifi");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(wifi, "password");

    if (cJSON_IsString(password))
        return password->valuestring;

    return NULL;
}

const char *config_wifi_eap_get(const char *param_name)
{
    cJSON *wifi = cJSON_GetObjectItemCaseSensitive(config, "wifi");
    cJSON *eap = cJSON_GetObjectItemCaseSensitive(wifi, "eap");
    cJSON *param = cJSON_GetObjectItemCaseSensitive(eap, param_name);

    if (cJSON_IsString(param))
        return param->valuestring;

    return NULL;
}

const char *config_eap_file_get(const char *field)
{
    const char *file = config_wifi_eap_get(field);
    char buf[128];

    if (!file)
        return NULL;

    snprintf(buf, sizeof(buf), "/spiffs%s", file);
    return read_file(buf);
}

const char *config_eap_ca_cert_get(void)
{
    static const char *cert;

    if (!cert)
        cert = config_eap_file_get("ca_cert");

    return cert;
}

const char *config_eap_client_cert_get(void)
{
    static const char *cert;

    if (!cert)
        cert = config_eap_file_get("client_cert");

    return cert;
}

const char *config_eap_client_key_get(void)
{
    static const char *key;

    if (!key)
        key = config_eap_file_get("client_key");

    return key;
}

const char *config_eap_method_get(void)
{
    return config_wifi_eap_get("method");
}

const char *config_eap_identity_get(void)
{
    return config_wifi_eap_get("identity");
}

const char *config_eap_username_get(void)
{
    return config_wifi_eap_get("username");
}

const char *config_eap_password_get(void)
{
    return config_wifi_eap_get("password");
}

/* Remote Logging Configuration */
const char *config_log_host_get(void)
{
    cJSON *log = cJSON_GetObjectItemCaseSensitive(config, "log");
    cJSON *ip = cJSON_GetObjectItemCaseSensitive(log, "host");

    if (cJSON_IsString(ip))
        return ip->valuestring;

    return NULL;
}

uint16_t config_log_port_get(void)
{
    cJSON *log = cJSON_GetObjectItemCaseSensitive(config, "log");
    cJSON *port = cJSON_GetObjectItemCaseSensitive(log, "port");

    if (cJSON_IsNumber(port))
        return port->valuedouble;

    return 0;
}

/* Configuration Update */
static int config_active_partition_get(void)
{
    uint8_t partition = 0;

    nvs_get_u8(nvs, nvs_active_partition, &partition);
    return partition;
}

static int config_active_partition_set(uint8_t partition)
{
    ESP_LOGD(TAG, "Setting active partition to %u", partition);

    if (nvs_set_u8(nvs, nvs_active_partition, partition) != ESP_OK ||
        nvs_commit(nvs) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed setting active partition to: %u", partition);
        return -1;
    }

    return 0;
}

int config_update_begin(config_update_handle_t **handle)
{
    const esp_partition_t *partition;
    char partition_name[5];
    uint8_t partition_id = !config_active_partition_get();

    sprintf(partition_name, "fs_%u", partition_id);
    ESP_LOGI(TAG, "Writing to partition %s", partition_name);
    partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_SPIFFS, partition_name);

    if (!partition)
    {
        ESP_LOGE(TAG, "Failed finding SPIFFS partition");
        return -1;
    }

    ESP_LOGI(TAG, "Writing partition type 0x%0x subtype 0x%0x (offset 0x%08x)",
        partition->type, partition->subtype, partition->address);

    /* Erase partition, needed before writing is allowed */
    if (esp_partition_erase_range(partition, 0, partition->size))
        return -1;

    *handle = malloc(sizeof(**handle));
    (*handle)->partition = partition;
    (*handle)->partition_id = partition_id;
    (*handle)->bytes_written = 0;

    return 0;
}

int config_update_write(config_update_handle_t *handle, uint8_t *data,
    size_t len)
{
    if (esp_partition_write(handle->partition, handle->bytes_written, data,
        len))
    {
        ESP_LOGE(TAG, "Failed writing to SPIFFS partition!");
        free(handle);
        return -1;
    }

    handle->bytes_written += len;
    return 0;
}

int config_update_end(config_update_handle_t *handle)
{
    int ret = -1;

    /* We succeeded only if the entire partition was written */
    if (handle->bytes_written == handle->partition->size)
        ret = config_active_partition_set(handle->partition_id);

    free(handle);
    return ret;
}

static cJSON *load_json(const char *path)
{
    char *str = read_file(path);
    cJSON *json;

    if (!str)
        return NULL;

    json = cJSON_Parse(str);

    free(str);
    return json;
}

char *config_version_get(void)
{
    return config_version;
}

int config_load(uint8_t partition_id)
{
    char *p, partition_name[] = { 'f', 's', '_', 'x', '\0' };
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = partition_name,
        .max_files = 8,
        .format_if_mount_failed = true
    };
    uint8_t i, sha[32];

    partition_name[3] = partition_id + '0';
    ESP_LOGD(TAG, "Loading config from partition %s", partition_name);

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    /* Load config.json from SPIFFS */
    if (!(config = load_json(config_file_name)))
    {
        esp_vfs_spiffs_unregister(partition_name);
        return -1;
    }

    /* Calulate hash of active partition */
    esp_partition_get_sha256(esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_SPIFFS, partition_name), sha);
    for (p = config_version, i = 0; i < sizeof(sha); i++)
        p += sprintf(p, "%02x", sha[i]);

    return 0;
}

int config_initialize(void)
{
    uint8_t partition;

    ESP_LOGI(TAG, "Initializing configuration");
    ESP_ERROR_CHECK(nvs_open(nvs_namespace, NVS_READWRITE, &nvs));

    partition = config_active_partition_get();

    /* Attempt to load configuration from active partition with fall-back */
    if (config_load(partition))
    {
        ESP_LOGE(TAG, "Failed loading partition %d, falling back to %d",
            partition, !partition);
        if (config_load(!partition))
        {
            ESP_LOGE(TAG, "Failed loading partition %d as well", !partition);
            return -1;
        }
        /* Fall-back partition is OK, mark it as active */
        config_active_partition_set(!partition);
    }

    ESP_LOGI(TAG, "version: %s", config_version_get());
    return 0;
}
