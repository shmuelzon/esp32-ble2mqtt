#include "resolve.h"
#include <esp_log.h>
#include <mdns.h>
#include <lwip/dns.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <string.h>

/* Constants */
static const char *TAG = "Resolve";

const char *resolve_host(const char *hostname)
{
    static char buf[64];
    struct hostent *host;

    if (!(host = gethostbyname(hostname)) || host->h_addr_list[0] == NULL)
    {
        ESP_LOGE(TAG, "Failed resolving %s", hostname);
        return hostname;
    }

    sprintf(buf, "%s", inet_ntoa(*(struct ip4_addr *)host->h_addr_list[0]));
    ESP_LOGD(TAG, "DNS resolved %s to %s", hostname, buf);
    return buf;
}

int resolve_initialize(void)
{
    ESP_LOGI(TAG, "Initializing resolver");

    return 0;
}
