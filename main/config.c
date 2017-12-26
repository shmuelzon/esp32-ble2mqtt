#include "config.h"
#include <stdio.h>
#include <string.h>

/* BLE Configuration*/
uint8_t config_ble_should_connect(const char *mac)
{
    char *whitelist[] = {
        "01:02:03:04:05:06",
        /* EOL */
        NULL,
    }, **p;

    for (p = whitelist; *p; p++)
    {
        if (!strcmp(mac, *p))
            return 1;
    }
    
    return 0;
}

/* MQTT Configuration*/
const char *config_mqtt_host_get(void)
{
    return "192.168.1.1";
}

uint16_t config_mqtt_port_get(void)
{
    return 1883;
}

const char *config_mqtt_client_id_get(void)
{
    return NULL;
}

const char *config_mqtt_username_get(void)
{
    return NULL;
}

const char *config_mqtt_password_get(void)
{
    return NULL;
}

uint8_t config_mqtt_qos_get(void)
{
    return 0;
}

uint8_t config_mqtt_retained_get(void)
{
    return 0;
}

const char *config_mqtt_get_suffix_get(void)
{
    return "/Get";
}

const char *config_mqtt_set_suffix_get(void)
{
    return "/Set";
}

/* WiFi Configuration */
const char *config_wifi_ssid_get(void)
{
    return "MY_SSID";
}

const char *config_wifi_password_get(void)
{
    return "MY_PASSWORD";
}

int config_initialize(void)
{
    /* TODO: Load config.json from SPIFFS */
    return 0;
}
