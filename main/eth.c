#include "eth.h"
#include <esp_err.h>
#include <esp_eth.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_netif.h>
#include <esp_rom_gpio.h>
#include <driver/gpio.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>

static const char *TAG = "Ethernet";

static eth_on_connected_cb_t on_connected_cb = NULL;
static eth_on_disconnected_cb_t on_disconnected_cb = NULL;
static char *eth_hostname = NULL;
#if CONFIG_ETH_USE_ESP32_EMAC
static esp_netif_t *eth_netif = NULL;
#endif

void eth_set_on_connected_cb(eth_on_connected_cb_t cb)
{
    on_connected_cb = cb;
}

void eth_set_on_disconnected_cb(eth_on_disconnected_cb_t cb)
{
    on_disconnected_cb = cb;
}

uint8_t *eth_mac_get(void)
{
    static uint8_t mac[6] = {};

    if (!mac[0])
        esp_read_mac(mac, ESP_MAC_ETH);

    return mac;
}

void eth_hostname_set(const char *hostname)
{
    if (eth_hostname)
        free(eth_hostname);
    eth_hostname = strdup(hostname);
}

#if CONFIG_ETH_USE_ESP32_EMAC
static void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    if (event_base == ETH_EVENT)
    {
        switch (event_id) {
        case ETHERNET_EVENT_START:
            if (eth_hostname)
                esp_netif_set_hostname(eth_netif, eth_hostname);
            break;
        case ETHERNET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected");
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected");
            if (on_disconnected_cb)
                on_disconnected_cb();
            break;
        default:
            ESP_LOGD(TAG, "Unhandled Ethernet event (%" PRId32 ")", event_id);
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id) {
        case IP_EVENT_ETH_GOT_IP:
            {
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

                ESP_LOGD(TAG, "Got IP address: %s",
                    inet_ntoa(event->ip_info.ip));
                if (on_connected_cb)
                    on_connected_cb();
                break;
            }
        default:
            ESP_LOGD(TAG, "Unhandled IP event (%" PRId32 ")", event_id);
            break;
        }
    }
}
#endif

eth_phy_t eth_phy_atophy(const char *phy)
{
    struct {
        const char *name;
        int phy;
    } *p, phys[] = {
        { "IP101", PHY_IP101 },
        { "RTL8201", PHY_RTL8201 },
        { "LAN8720", PHY_LAN8720 },
        { "DP83848", PHY_DP83848 },
    };

    for (p = phys; p->name; p++)
    {
        if (!strcmp(p->name, phy))
            break;
    }

    return p->phy;
}

int eth_connect(eth_phy_t eth_phy, int8_t eth_phy_power_pin)
{
#if CONFIG_ETH_USE_ESP32_EMAC
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_esp32_emac_config_t esp32_mac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_mac_config, &mac_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, NULL);
    esp_eth_handle_t eth_handle = NULL;

    eth_netif = esp_netif_new(&cfg);
    if (eth_phy_power_pin >= 0)
    {
        esp_rom_gpio_pad_select_gpio(eth_phy_power_pin);
        gpio_set_direction(eth_phy_power_pin, GPIO_MODE_OUTPUT);
        gpio_set_level(eth_phy_power_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    switch (eth_phy)
    {
    case PHY_IP101:
        ESP_LOGI(TAG, "PHY config: IP101");
        config.phy = esp_eth_phy_new_ip101(&phy_config);
        break;
    case PHY_RTL8201:
        ESP_LOGI(TAG, "PHY config: RTL8201");
        config.phy = esp_eth_phy_new_rtl8201(&phy_config);
        break;
    case PHY_LAN8720:
        ESP_LOGI(TAG, "PHY config: LAN8720");
        config.phy = esp_eth_phy_new_lan87xx(&phy_config);
        break;
    case PHY_DP83848:
        ESP_LOGI(TAG, "PHY config: DP83848");
        config.phy = esp_eth_phy_new_dp83848(&phy_config);
        break;
    }

    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));

    return 0;
#else
    return -1;
#endif
}

int eth_initialize(void)
{
#if CONFIG_ETH_USE_ESP32_EMAC
    ESP_LOGD(TAG, "Initializing Ethernet");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    return 0;
#else
    ESP_LOGE(TAG, "Ethernet not supported!");
    return -1;
#endif
}
