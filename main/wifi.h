#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>

typedef enum {
    EAP_NONE,
    EAP_TLS,
    EAP_PEAP,
    EAP_TTLS
} eap_method_t;

/* Event callback types */
typedef void (*wifi_on_connected_cb_t)(void);
typedef void (*wifi_on_disconnected_cb_t)(void);

eap_method_t wifi_eap_atomethod(const char *method);

/* Event handlers */
void wifi_set_on_connected_cb(wifi_on_connected_cb_t cb);
void wifi_set_on_disconnected_cb(wifi_on_disconnected_cb_t cb);

int wifi_initialize(void);
int wifi_connect(const char *ssid, const char *password,
    eap_method_t eap_method, const char *eap_identity,
    const char *eap_username, const char *eap_password,
    const char *ca_cert, const char *client_cert, const char *client_key);
uint8_t *wifi_mac_get(void);
void wifi_hostname_set(const char *hostname);

#endif
