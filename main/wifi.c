#include "wifi.h"
#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wpa2.h>
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

static void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch(event_id) {
        case WIFI_EVENT_STA_START:
            if (wifi_hostname)
                tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, wifi_hostname);
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Connected");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected");
            if (on_disconnected_cb)
                on_disconnected_cb();
            /* This is a workaround as ESP32 WiFi libs don't currently
             * auto-reassociate. */
            esp_wifi_connect();
            break;
        default:
            ESP_LOGD(TAG, "Unhandled WiFi event (%d)", event_id);
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch(event_id) {
        case IP_EVENT_STA_GOT_IP:
            {
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

                ESP_LOGD(TAG, "Got IP address: %s",
                    inet_ntoa(event->ip_info.ip));
                if (on_connected_cb)
                    on_connected_cb();
                break;
            }
        case IP_EVENT_STA_LOST_IP:
            ESP_LOGD(TAG, "Lost IP address");
            break;
        default:
            ESP_LOGD(TAG, "Unhandled IP event (%d)", event_id);
            break;
        }
    }
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
        ESP_ERROR_CHECK(esp_wifi_sta_wpa2_ent_enable());
    }
    ESP_LOGI(TAG, "Connecting to SSID %s", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(78));

    return 0;
}

int wifi_initialize(void)
{
    ESP_LOGD(TAG, "Initializing WiFi station");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    return 0;
}
