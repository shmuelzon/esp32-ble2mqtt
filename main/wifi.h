#ifndef WIFI_H
#define WIFI_H

/* Event callback types */
typedef void (*wifi_on_connected_cb_t)(void);
typedef void (*wifi_on_disconnected_cb_t)(void);

/* Event handlers */
void wifi_set_on_connected_cb(wifi_on_connected_cb_t cb);
void wifi_set_on_disconnected_cb(wifi_on_disconnected_cb_t cb);

int wifi_initialize(void);
int wifi_connect(const char *ssid, const char *password);

#endif
