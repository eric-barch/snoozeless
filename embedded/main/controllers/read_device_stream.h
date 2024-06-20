#ifndef READ_DEVICE_STREAM_CONTROLLER
#define READ_DEVICE_STREAM_CONTROLLER

#include "esp_err.h"
#include "esp_http_client.h"

#ifndef MAX_HTTP_RX_BUFFER
#define MAX_HTTP_RX_BUFFER 1024
#endif

#ifndef MAX_HTTP_TX_BUFFER
#define MAX_HTTP_TX_BUFFER 1024
#endif

esp_err_t read_device_stream_event_handler(esp_http_client_event_t *event);

#endif
