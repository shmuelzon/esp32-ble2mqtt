#include "wifi.h"
#include <esp_err.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <mdns.h>
#include <arpa/inet.h>
#include <string.h>

static const char *TAG = "WiFi";

static wifi_on_connected_cb_t on_connected_cb = NULL;
static wifi_on_disconnected_cb_t on_disconnected_cb = NULL;
static char *wifi_hostname = NULL;

void wifi_set_on_connected_cb(wifi_on_connected_cb_t cb)
{
    on_connected_cb = cb;
}

void wifi_set_on_disconnected_cb(wifi_on_disconnected_cb_t cb)
{
    on_disconnected_cb = cb;
}

uint8_t *wifi_mac_get(void)
{
    static uint8_t mac[6] = {};

    if (!mac[0])
        esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);

    return mac;
}

void wifi_hostname_set(const char *hostname)
{
    if (wifi_hostname)
        free(wifi_hostname);
    wifi_hostname = strdup(hostname);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        if (wifi_hostname)
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, wifi_hostname);
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGD(TAG, "Got IP address: %s",
            inet_ntoa(event->event_info.got_ip.ip_info.ip));
        if (on_connected_cb)
            on_connected_cb();
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        ESP_LOGD(TAG, "Lost IP address");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "Connected");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "Disconnected");
        if (on_disconnected_cb)
            on_disconnected_cb();
        /* This is a workaround as ESP32 WiFi libs don't currently
         * auto-reassociate. */
        esp_wifi_connect();
        break;
    default:
        ESP_LOGD(TAG, "Unhandled event (%d)", event->event_id);
        break;
    }

    mdns_handle_system_event(ctx, event);
    return ESP_OK;
}

int wifi_connect(const char *ssid, const char *password)
{
    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, 32);
    strncpy((char *)wifi_config.sta.password, password, 64);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "Connecting to SSID %s", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_start());

    return 0;
}

int wifi_initialize(void)
{
    ESP_LOGD(TAG, "Initializing WiFi station");
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    return 0;
}
