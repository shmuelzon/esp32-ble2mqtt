#ifndef ETH_H
#define ETH_H

#include <stdint.h>

typedef enum {
    PHY_IP101,
    PHY_RTL8201,
    PHY_LAN8720,
    PHY_DP83848,
} eth_phy_t;

/* Event callback types */
typedef void (*eth_on_connected_cb_t)(void);
typedef void (*eth_on_disconnected_cb_t)(void);

eth_phy_t eth_phy_atophy(const char *phy);
uint8_t eth_clk_mode_atoclk_mode(const char *clk_mode);

/* Event handlers */
void eth_set_on_connected_cb(eth_on_connected_cb_t cb);
void eth_set_on_disconnected_cb(eth_on_disconnected_cb_t cb);

int eth_initialize(void);
int eth_connect(eth_phy_t eth_phy);
uint8_t *eth_mac_get(void);
void eth_hostname_set(const char *hostname);

#endif
