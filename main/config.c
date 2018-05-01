#include "config.h"
#include <mbedtls/md5.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_spiffs.h>
#include <cJSON.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Constants */
static const char *TAG = "Config";
static const char *config_file_name = "/spiffs/config.json";
static const char *config_update_file_name = "/spiffs/config.json.update";
static cJSON *config;

/* Internal variables */
static char config_version[33];

/* BLE Configuration*/
static cJSON *config_ble_get_name_by_uuid(uint8_t is_service,
    const char *uuid, const char *field_name)
{
    cJSON *ble = cJSON_GetObjectItemCaseSensitive(config, "ble");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(ble,
        is_service ?  "services" : "characteristics");
    cJSON *list = cJSON_GetObjectItemCaseSensitive(type, "definitions");

    /* Check config.json for override values */
    cJSON *obj = cJSON_GetObjectItemCaseSensitive(list, uuid);
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

cJSON *json_find_in_array(cJSON *arr, const char *item)
{
    cJSON *cur;

    if (!arr)
        return NULL;

    for (cur = arr->child; cur; cur = cur->next)
    {
        if (cJSON_IsString(cur) && !strcmp(item, cur->valuestring))
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
    cJSON *key = cJSON_GetObjectItemCaseSensitive(passkeys, mac);

    if (cJSON_IsNumber(key))
        return key->valuedouble;

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
    return 1883;
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

const char *config_mqtt_get_suffix_get(void)
{
    return config_mqtt_topics_get("get_suffix", "/Get");
}

const char *config_mqtt_set_suffix_get(void)
{
    return config_mqtt_topics_get("set_suffix", "/Set");
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

/* Configuration Update */
int config_update_begin(config_update_handle_t *handle)
{
    int fd;

    if ((fd = open(config_update_file_name, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
        return -1;

    *handle = fd;
    return 0;
}

int config_update_write(config_update_handle_t handle, uint8_t *data,
    size_t len)
{
    return write(handle, data, len) < 0;
}

int config_update_end(config_update_handle_t handle)
{
    struct stat st;

    if (close(handle))
        return -1;

    if (stat(config_update_file_name, &st))
        return -1;

    if (st.st_size == 0)
        return -1;

    if (unlink(config_file_name))
        return -1;

    if (rename(config_update_file_name, config_file_name))
        return -1;

    return 0;
}

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

static cJSON *load_json(const char *path)
{
    char *p, *str = read_file(path);
    uint8_t i, hash[16];
    cJSON *json;

    if (!str)
        return NULL;

    json = cJSON_Parse(str);

    /* Calculate MD5 hash as config version */
    mbedtls_md5((unsigned char *)str, strlen(str), hash);
    for (i = 0, p = config_version; i < 16; i++)
        p += sprintf(p, "%02x", hash[i]);

    free(str);
    return json;
}

char *config_version_get(void)
{
    return config_version;
}

int config_initialize(void)
{
    ESP_LOGI(TAG, "Initializing configuration");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    /* Load config.json from SPIFFS */
    if (!(config = load_json(config_file_name)))
        return -1;

    ESP_LOGI(TAG, "version: %s", config_version_get());
    return 0;
}
