#include "eth.h"
#include <esp_err.h>
#include <esp_eth.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <mdns.h>
#include <eth_phy/phy_lan8720.h>
#include <eth_phy/phy_tlk110.h>
#include <eth_phy/phy_ip101.h>
#include <tcpip_adapter.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>

static const char *TAG = "Ethernet";

static eth_on_connected_cb_t on_connected_cb = NULL;
static eth_on_disconnected_cb_t on_disconnected_cb = NULL;
static char *eth_hostname = NULL;
static uint8_t eth_phy_gpio_power = 0;
static eth_phy_t eth_phy;

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

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_ETH_START:
        if (eth_hostname)
            tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_ETH, eth_hostname);
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        ESP_LOGD(TAG, "Got IP address: %s",
            inet_ntoa(event->event_info.got_ip.ip_info.ip));
        if (on_connected_cb)
            on_connected_cb();
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        ESP_LOGI(TAG, "Connected");
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        ESP_LOGI(TAG, "Disconnected");
        if (on_disconnected_cb)
            on_disconnected_cb();
        break;
    default:
        ESP_LOGD(TAG, "Unhandled event (%d)", event->event_id);
        break;
    }

    mdns_handle_system_event(ctx, event);
    return ESP_OK;
}

uint8_t eth_clk_mode_atoclk_mode(const char *clk_mode)
{
    struct {
        const char *name;
        int clk_mode;
    } *p, clk_modes[] = {
        { "ETH_CLOCK_GPIO0_IN", ETH_CLOCK_GPIO0_IN },
        { "ETH_CLOCK_GPIO0_OUT", ETH_CLOCK_GPIO0_OUT },
        { "ETH_CLOCK_GPIO16_OUT", ETH_CLOCK_GPIO16_OUT },
        { "ETH_CLOCK_GPIO17_OUT", ETH_CLOCK_GPIO17_OUT }
    };

    for (p = clk_modes; p->name; p++)
    {
        if (!strcmp(p->name, clk_mode))
            break;
    }

    return p->clk_mode;
}

eth_phy_t eth_phy_atophy(const char *phy)
{
    struct {
        const char *name;
        int phy;
    } *p, phys[] = {
        { "LAN8720", PHY_LAN8720 },
        { "TLK110", PHY_TLK110 },
        { "IP101", PHY_IP101 }
    };

    for (p = phys; p->name; p++)
    {
        if (!strcmp(p->name, phy))
            break;
    }

    return p->phy;
}

static void eth_gpio_config_rmii(void)
{
    phy_rmii_configure_data_interface_pins();
    phy_rmii_smi_configure_pins(23, 18);
}

static void eth_phy_power_enable(bool enable)
{
    if (!enable && eth_phy == PHY_LAN8720)
    {
        phy_lan8720_default_ethernet_config.phy_power_enable(false);
        ESP_LOGI(TAG, "PHY power disabled");
    }

    gpio_set_level(eth_phy_gpio_power, enable);
    vTaskDelay(1);

    if (enable && eth_phy == PHY_LAN8720)
    {
        phy_lan8720_default_ethernet_config.phy_power_enable(true);
        ESP_LOGI(TAG, "PHY power enabled");
    }
}

int eth_connect(eth_phy_t phy, uint8_t clk_mode, uint8_t phy_gpio_power)
{
    eth_phy = phy;
    eth_phy_gpio_power = phy_gpio_power;
    eth_config_t config;

    switch (eth_phy)
    {
    case PHY_LAN8720:
        config = phy_lan8720_default_ethernet_config;
        ESP_LOGI(TAG, "PHY config: LAN8720");
        break;
    case PHY_TLK110:
        config = phy_tlk110_default_ethernet_config;
        ESP_LOGI(TAG, "PHY config: TLK110");
        break;
    case PHY_IP101:
        config = phy_ip101_default_ethernet_config;
        ESP_LOGI(TAG, "PHY config: IP101");
        break;
    }

    config.clock_mode = clk_mode;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;

    /* Some boards implement PHY power control through GPIO pins, e.g.
       Olimex ESP32-PoE */
    if (eth_phy_gpio_power > 0)
    {
        gpio_pad_select_gpio(eth_phy_gpio_power);
        gpio_set_direction(eth_phy_gpio_power, GPIO_MODE_OUTPUT);
        config.phy_power_enable = eth_phy_power_enable;
    }

    ESP_ERROR_CHECK(esp_eth_init(&config));
    ESP_ERROR_CHECK(esp_eth_enable());

    return 0;
}

int eth_initialize(void)
{
    ESP_LOGD(TAG, "Initializing Ethernet");
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    return 0;
}
