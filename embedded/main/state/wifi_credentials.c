#include "state/wifi_credentials.h"
#include "esp_err.h"
#include "utilities/console.h"
#include "utilities/nvs.h"

#define WIFI_MAX_SSID_LENGTH 32
#define WIFI_MAX_PASSWORD_LENGTH 64

typedef struct {
  char ssid[WIFI_MAX_SSID_LENGTH];
  char password[WIFI_MAX_PASSWORD_LENGTH];
} wifi_credentials_t;

static wifi_credentials_t wifi_credentials;

esp_err_t initialize_wifi_credentials(void) {
  esp_err_t err = open_nvs_namespace("wifi_cred");
  if (err != ESP_OK) {
    return err;
  }

  err = get_nvs_str("ssid", wifi_credentials.ssid, WIFI_MAX_SSID_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter WiFi SSID: ", wifi_credentials.ssid,
                          WIFI_MAX_SSID_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("ssid", wifi_credentials.ssid);
    if (err != ESP_OK) {
      return err;
    }
  }

  err = get_nvs_str("password", wifi_credentials.password,
                    WIFI_MAX_PASSWORD_LENGTH);
  if (err != ESP_OK) {
    err = get_console_str("Enter WiFi password: ", wifi_credentials.password,
                          WIFI_MAX_PASSWORD_LENGTH);
    if (err != ESP_OK) {
      return err;
    }

    err = set_nvs_str("password", wifi_credentials.password);
    if (err != ESP_OK) {
      return err;
    }
  }

  return err;
}

const char *get_wifi_ssid(void) { return wifi_credentials.ssid; }

const char *get_wifi_password(void) { return wifi_credentials.password; }
