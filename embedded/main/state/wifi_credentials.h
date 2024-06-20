#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

#include "esp_err.h"

esp_err_t initialize_wifi_credentials(void);

const char *get_wifi_ssid(void);

const char *get_wifi_password(void);

#endif
