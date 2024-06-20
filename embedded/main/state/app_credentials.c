#include "state/app_credentials.h"
#include "esp_err.h"
#include "utilities/console.h"
#include "utilities/nvs.h"

#define MAX_API_KEY_LENGTH 300
#define MAX_AUTH_TOKEN_LENGTH 1000
#define MAX_REFRESH_TOKEN_LENGTH 30

typedef struct {
  char api_key[MAX_API_KEY_LENGTH];
  char auth_token[MAX_AUTH_TOKEN_LENGTH];
  char refresh_token[MAX_REFRESH_TOKEN_LENGTH];
} app_credentials_t;

static app_credentials_t app_credentials;

esp_err_t initialize_app_credentials(void) {
  esp_err_t err = open_nvs_namespace("app_cred");
  if (err != ESP_OK) {
    return err;
  }

  err = get_nvs_str("api_key", app_credentials.api_key, MAX_API_KEY_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter API key: ", app_credentials.api_key,
                          MAX_API_KEY_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("api_key", app_credentials.api_key);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("auth_token", app_credentials.auth_token,
                    MAX_AUTH_TOKEN_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter Authorization Bearer token: ",
                          app_credentials.auth_token, MAX_AUTH_TOKEN_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("auth_token", app_credentials.auth_token);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("refresh_token", app_credentials.refresh_token,
                    MAX_REFRESH_TOKEN_LENGTH);
  if (err != ESP_OK) {
    err =
        get_console_str("Enter refresh_token: ", app_credentials.refresh_token,
                        MAX_REFRESH_TOKEN_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("refresh_token", app_credentials.refresh_token);
    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
}

const char *get_api_key(void) { return app_credentials.api_key; }

const char *get_auth_token(void) { return app_credentials.auth_token; }

const char *get_refresh_token(void) { return app_credentials.refresh_token; }
