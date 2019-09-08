#include "wifi.h"
#include <esp_err.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wpa2.h>
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

eap_method_t wifi_eap_atomethod(const char *method)
{
    if (method == NULL)
        return EAP_NONE;

    struct {
        const char *name;
        int method;
    } *p, methods[] = {
        { "TLS", EAP_TLS },
        { "PEAP", EAP_PEAP },
        { "TTLS", EAP_TTLS },
        { NULL, EAP_NONE }
    };

    for (p = methods; p->name; p++)
    {
        if (!strcmp(p->name, method))
            break;
    }

    return p->method;
}

int wifi_connect(const char *ssid, const char *password,
    eap_method_t eap_method, const char *eap_identity,
    const char *eap_username, const char *eap_password,
    const char *ca_cert, const char *client_cert, const char *client_key)
{
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, 32);
    if (password)
        strncpy((char *)wifi_config.sta.password, password, 64);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    if (eap_method)
    {
        if (ca_cert)
        {
            ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)ca_cert,
                strlen(ca_cert)));
        }
        if (client_cert)
        {
            ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_cert_key((uint8_t *)client_cert,
                strlen(client_cert), (uint8_t *)client_key,
                client_key ? strlen(client_key) : 0, NULL, 0));
        }
        if (eap_identity)
        {
            ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)eap_identity,
                strlen(eap_identity)));
        }
        if (eap_method == EAP_PEAP || eap_method == EAP_TTLS)
        {
            if (eap_username || eap_password)
            {
                ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_username((uint8_t *)eap_username,
                    strlen(eap_username)));
                ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_set_password((uint8_t *)eap_password,
                    strlen(eap_password)));
            }
            else
            {
                ESP_LOGE(TAG, "Username and password are required for "
                    "Tunneled TLS or Protected EAP");
            }
        }
        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable(&config));
    }
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
