#include "state/app_credentials.h"
#include "esp_err.h"
#include "utilities/console.h"
#include "utilities/nvs.h"
#include <string.h>

#define MAX_API_KEY_LENGTH 300
#define MAX_AUTH_TOKEN_LENGTH 1000
#define MAX_REFRESH_TOKEN_LENGTH 30
#define APP_CRED_NAMESPACE "app_cred"

typedef struct {
  char api_key[MAX_API_KEY_LENGTH];
  char auth_token[MAX_AUTH_TOKEN_LENGTH];
  char refresh_token[MAX_REFRESH_TOKEN_LENGTH];
} app_credentials_t;

static app_credentials_t app_credentials;

static esp_err_t initialize_app_credential(const char *key, char *out_value,
                                           size_t length) {
  esp_err_t err = get_nvs_str(APP_CRED_NAMESPACE, key, out_value, length);
  if (err != ESP_OK) {
    char prompt[MAX_PROMPT_LENGTH];
    snprintf(prompt, sizeof(prompt), "Enter %s %s: ", APP_CRED_NAMESPACE, key);
    err = get_console_str(prompt, out_value, length);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str(APP_CRED_NAMESPACE, key, out_value);
  }

  return err;
}

esp_err_t initialize_app_credentials(void) {
  esp_err_t err = initialize_app_credential("api_key", app_credentials.api_key,
                                            MAX_API_KEY_LENGTH);
  if (err != ESP_OK) {
    return err;
  }

  err = initialize_app_credential("auth_token", app_credentials.auth_token,
                                  MAX_AUTH_TOKEN_LENGTH);
  if (err != ESP_OK) {
    return err;
  }

  err = initialize_app_credential(
      "refresh_token", app_credentials.refresh_token, MAX_REFRESH_TOKEN_LENGTH);
  return err;
}

esp_err_t set_api_key(const char *api_key) {
  strncpy(app_credentials.api_key, api_key, MAX_API_KEY_LENGTH);
  app_credentials.api_key[MAX_API_KEY_LENGTH - 1] = '\0';
  printf("\nSet volatile app credentials API key: %s\n", api_key);

  esp_err_t err = set_nvs_str(APP_CRED_NAMESPACE, "api_key", api_key);
  return err;
}

const char *get_api_key(void) { return app_credentials.api_key; }

esp_err_t set_auth_token(const char *auth_token) {
  strncpy(app_credentials.auth_token, auth_token, MAX_AUTH_TOKEN_LENGTH);
  app_credentials.auth_token[MAX_AUTH_TOKEN_LENGTH - 1] = '\0';
  printf("\nSet volatile app credentials auth token: %s\n", auth_token);

  esp_err_t err = set_nvs_str(APP_CRED_NAMESPACE, "auth_token", auth_token);
  return err;
}

const char *get_auth_token(void) { return app_credentials.auth_token; }

esp_err_t set_refresh_token(const char *refresh_token) {
  strncpy(app_credentials.refresh_token, refresh_token,
          MAX_REFRESH_TOKEN_LENGTH);
  app_credentials.refresh_token[MAX_REFRESH_TOKEN_LENGTH - 1] = '\0';
  printf("\nSet volatile app credentials refresh token: %s\n", refresh_token);

  esp_err_t err =
      set_nvs_str(APP_CRED_NAMESPACE, "refresh_token", refresh_token);
  return err;
}

const char *get_refresh_token(void) { return app_credentials.refresh_token; }
