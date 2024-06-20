#ifndef POST_DEVICE_REGISTER_H
#define POST_DEVICE_REGISTER_H

#include "esp_err.h"
#include "state/device.h"

#ifndef MAX_HTTP_RX_BUFFER
#define MAX_HTTP_RX_BUFFER 1024
#endif

#ifndef MAX_HTTP_TX_BUFFER
#define MAX_HTTP_TX_BUFFER 1024
#endif

esp_err_t post_device_register(device_t *device);

#endif
