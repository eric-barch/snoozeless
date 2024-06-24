#ifndef STATE_WIFI_CREDENTIALS_H
#define STATE_WIFI_CREDENTIALS_H

#include "esp_err.h"

esp_err_t initialize_wifi_credentials(void);

esp_err_t set_wifi_ssid(const char *ssid);

const char *get_wifi_ssid(void);

esp_err_t set_wifi_password(const char *password);

const char *get_wifi_password(void);

#endif
