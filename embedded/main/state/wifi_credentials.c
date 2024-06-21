#include "state/wifi_credentials.h"
#include "esp_err.h"
#include "utilities/console.h"
#include "utilities/nvs.h"
#include <string.h>

#define WIFI_MAX_SSID_LENGTH 32
#define WIFI_MAX_PASSWORD_LENGTH 64
#define WIFI_CRED_NAMESPACE "wifi_cred"

typedef struct {
  char ssid[WIFI_MAX_SSID_LENGTH];
  char password[WIFI_MAX_PASSWORD_LENGTH];
} wifi_credentials_t;

static wifi_credentials_t wifi_credentials;

static esp_err_t initialize_wifi_credential(const char *key, char *out_value,
                                            size_t length) {
  esp_err_t err = get_nvs_str(WIFI_CRED_NAMESPACE, key, out_value, length);
  if (err != ESP_OK) {
    char prompt[MAX_PROMPT_LENGTH];
    snprintf(prompt, sizeof(prompt), "Enter %s %s: ", WIFI_CRED_NAMESPACE, key);
    err = get_console_str(prompt, out_value, length);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str(WIFI_CRED_NAMESPACE, key, out_value);
  }

  return err;
}

esp_err_t initialize_wifi_credentials(void) {
  esp_err_t err = initialize_wifi_credential("ssid", wifi_credentials.ssid,
                                             WIFI_MAX_SSID_LENGTH);
  if (err != ESP_OK) {
    return err;
  }

  err = initialize_wifi_credential("password", wifi_credentials.password,
                                   WIFI_MAX_PASSWORD_LENGTH);

  return err;
}

esp_err_t set_wifi_ssid(const char *ssid) {
  strncpy(wifi_credentials.ssid, ssid, WIFI_MAX_SSID_LENGTH);
  wifi_credentials.ssid[WIFI_MAX_SSID_LENGTH - 1] = '\0';
  printf("\nSet volatile WiFi SSID: %s\n", ssid);

  esp_err_t err = set_nvs_str(WIFI_CRED_NAMESPACE, "ssid", ssid);
  return err;
}

const char *get_wifi_ssid(void) { return wifi_credentials.ssid; }

esp_err_t set_wifi_password(const char *password) {
  strncpy(wifi_credentials.password, password, WIFI_MAX_PASSWORD_LENGTH);
  wifi_credentials.password[WIFI_MAX_PASSWORD_LENGTH - 1] = '\0';
  printf("\nSet volatile WiFi password: %s\n", password);

  esp_err_t err = set_nvs_str(WIFI_CRED_NAMESPACE, "password", password);
  return err;
}

const char *get_wifi_password(void) { return wifi_credentials.password; }
