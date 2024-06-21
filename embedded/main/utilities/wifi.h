#ifndef WIFI_UTILITY_H
#define WIFI_UTILITY_H

#ifndef MAX_HTTP_RX_BUFFER
#define MAX_HTTP_RX_BUFFER 1024
#endif

#ifndef MAX_HTTP_TX_BUFFER
#define MAX_HTTP_TX_BUFFER 1024
#endif

#include "esp_err.h"

esp_err_t connect_to_wifi(void);

#endif
