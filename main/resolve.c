#include "resolve.h"
#include <esp_log.h>
#include <mdns.h>
#include <lwip/dns.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <string.h>

/* Constants */
static const char *TAG = "Resolve";

static int is_in_domain(const char *hostname, const char *domain, size_t *len)
{
    int hostname_len = strlen(hostname);
    int domain_len = strlen(domain);

    if (len)
        *len = hostname_len - domain_len;

    return (hostname_len >= domain_len) &&
        !strcmp(hostname + (hostname_len - domain_len), domain) ? 1 : 0;
}

const char *resolve_host(const char *hostname)
{
    static char buf[64];
    struct sockaddr_in ip;
    struct hostent *host;
    size_t len;

    /* Check if it's an IP address */
    if (inet_pton(AF_INET, hostname, &ip.sin_addr) == 1)
        return hostname;

    ESP_LOGD(TAG, "Trying to resolve %s", hostname);

    /* Check if it's an mDNS host name */
    if (is_in_domain(hostname, ".local", &len))
    {
        strncpy(buf, hostname, len);
        buf[len] = '\0';
        if (!mdns_query_a(buf, 5000, (struct ip4_addr *)&ip.sin_addr))
        {
            sprintf(buf, IPSTR, IP2STR((struct ip4_addr *)&ip.sin_addr));
            ESP_LOGD(TAG, "mDNS resolved %s to %s", hostname, buf);
            return buf;
        }
        ESP_LOGE(TAG, "Failed resolving %s via mDNS, trying DNS.", buf);
    }

    /* Try DNS resolving */
    if (!(host = gethostbyname(hostname)) || host->h_addr_list[0] == NULL)
    {
        ESP_LOGE(TAG, "Failed resolving %s via DNS", hostname);
        return hostname;
    }

    sprintf(buf, IPSTR, IP2STR((struct ip4_addr *)host->h_addr_list[0]));
    ESP_LOGD(TAG, "DNS resolved %s to %s", hostname, buf);
    return buf;
}

int resolve_initialize(void)
{
    ESP_LOGI(TAG, "Initializing resolver");

    return 0;
}
