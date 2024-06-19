#ifndef S_APP_CREDENTIALS_H
#define S_APP_CREDENTIALS_H

#include "esp_err.h"

esp_err_t initialize_app_credentials(void);

const char *get_api_key(void);

const char *get_auth_token(void);

const char *get_refresh_token(void);

#endif
