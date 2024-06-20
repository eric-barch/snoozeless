#ifndef GET_DEVICE_STATE_H
#define GET_DEVICE_STATE_H

#include "esp_err.h"

#ifndef MAX_HTTP_RX_BUFFER
#define MAX_HTTP_RX_BUFFER 1024
#endif

#ifndef MAX_HTTP_TX_BUFFER
#define MAX_HTTP_TX_BUFFER 1024
#endif

esp_err_t get_device_state(const char *deviceId);

#endif
