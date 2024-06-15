#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include "esp_event_base.h"

#ifndef MAX_WIFI_SSID_LENGTH
#define MAX_WIFI_SSID_LENGTH 32
#endif

#ifndef MAX_WIFI_PASSWORD_LENGTH
#define MAX_WIFI_PASSWORD_LENGTH 64
#endif

#ifndef MAX_WIFI_RETRY
#define MAX_WIFI_RETRY 5
#endif

#ifndef WIFI_SAE_MODE
#define WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#endif

#ifndef WIFI_SCAN_AUTH_MODE_THRESHOLD
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#endif

/**s_wifi_event_group (see source) allows multiple bits for each event, but we
 * only care about two: (1) we've connected to the AP with an IP, or (2) we've
 * failed to connect after the maximum number of retries. */
#ifndef WIFI_CONNECTED_BIT
#define WIFI_CONNECTED_BIT BIT0
#endif

#ifndef WIFI_FAIL_BIT
#define WIFI_FAIL_BIT BIT1
#endif

void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

void initialize_wifi(const char *ssid, const char *password);

#endif
