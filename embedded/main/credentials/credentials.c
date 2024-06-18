#include "credentials.h"
#include "console_utils.h"
#include "esp_err.h"
#include "nvs_utils.h"

#define MAX_API_KEY_LENGTH 300
#define MAX_AUTH_TOKEN_LENGTH 1000
#define MAX_REFRESH_TOKEN_LENGTH 30

typedef struct {
  char api_key[MAX_API_KEY_LENGTH];
  char auth_token[MAX_AUTH_TOKEN_LENGTH];
  char refresh_token[MAX_REFRESH_TOKEN_LENGTH];
} credentials_t;

static credentials_t credentials;

esp_err_t initialize_credentials(void) {
  esp_err_t err = open_nvs_namespace("credentials");
  if (err != ESP_OK) {
    return err;
  }

  err = get_nvs_str("api_key", credentials.api_key, MAX_API_KEY_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter API key: ", credentials.api_key,
                          MAX_API_KEY_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("api_key", credentials.api_key);
    if (err != ESP_OK) {
      return err;
    }
  }

  err =
      get_nvs_str("auth_token", credentials.auth_token, MAX_AUTH_TOKEN_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter Authorization Bearer token: ",
                          credentials.auth_token, MAX_AUTH_TOKEN_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("auth_token", credentials.auth_token);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("refresh_token", credentials.refresh_token,
                    MAX_REFRESH_TOKEN_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter refresh_token: ", credentials.refresh_token,
                          MAX_REFRESH_TOKEN_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("refresh_token", credentials.refresh_token);
    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
}

const char *get_api_key(void) { return credentials.api_key; }

const char *get_auth_token(void) { return credentials.auth_token; }

const char *get_refresh_token(void) { return credentials.refresh_token; }
