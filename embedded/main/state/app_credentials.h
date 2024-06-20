#ifndef APP_CREDENTIALS_STATE
#define APP_CREDENTIALS_STATE

#include "esp_err.h"

esp_err_t initialize_app_credentials(void);

const char *get_api_key(void);

const char *get_auth_token(void);

const char *get_refresh_token(void);

#endif
