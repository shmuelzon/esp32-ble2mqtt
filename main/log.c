#include "log.h"
#include "resolve.h"
#include <esp_log.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Constants */
static const char *TAG = "Log";

/* Internal state */
static vprintf_like_t orig_esp_log = NULL;
static int sock = -1;
static struct sockaddr_in dst;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
static int log_vprintf(const char *fmt, va_list l)
{
    static char buf[2048];
    int ret;

    /* Send remote log */
    ret = vsnprintf(buf,sizeof(buf), fmt, l);
	if((ret = sendto(sock, buf, MIN(ret, sizeof(buf) - 1), 0,
        (struct sockaddr *)&dst, sizeof(dst))) < 0)
    {
        printf("Failed sending remote log: %d\n", errno);
    }

    /* Also use existing logging mechanism */
    if (orig_esp_log)
        orig_esp_log(fmt, l);

    return ret;
}

int log_start(const char *host, uint16_t port)
{
    if (!host || !port)
        return -1;

    memset(&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;
    dst.sin_port = htons(port);
    if (!inet_pton(AF_INET, resolve_host(host), &dst.sin_addr))
    {
        ESP_LOGE(TAG, "Failed parsing IP address");
        goto Error;
    }

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        ESP_LOGE(TAG, "Failed creating socket: %d (%m)", errno);
        goto Error;
    }

    if (fcntl(sock, F_SETFL, O_NONBLOCK))
    {
        ESP_LOGE(TAG, "Failed setting socket as non-blocking");
        goto Error;
    }

    if (IN_MULTICAST(ntohl(dst.sin_addr.s_addr)))
    {
        uint8_t ttl = 2;
        if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl,
            sizeof(uint8_t)))
        {
            ESP_LOGE(TAG, "Failed setting multicast TTL");
            goto Error;
        }
    }

    ESP_LOGI(TAG, "Enabling remote logging");

    /* Set our logging function and save the original implementation */
    orig_esp_log = esp_log_set_vprintf(log_vprintf);

    return 0;

Error:
    if (sock >= 0)
    {
        close(sock);
        sock = -1;
    }

    return -1;
}

int log_stop(void)
{
    if (sock < 0)
        return -1;

    /* Restore original logging implementation */
    esp_log_set_vprintf(orig_esp_log);

    close(sock);
    sock = -1;

    ESP_LOGI(TAG, "Disabled remote logging");

    return 0;
}

int log_initialize(void)
{
    ESP_LOGI(TAG, "Initializing remote logging");

    return 0;
}
