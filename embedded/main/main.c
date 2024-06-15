#include "console_utils.h"
#include "freertos/idf_additions.h"
#include "http_requests.h"
#include "nvs_utils.h"
#include "wifi_utils.h"

#define MAX_APP_API_KEY_LENGTH 256

#define MAX_USER_EMAIL_LENGTH 256
#define MAX_USER_PASSWORD_LENGTH 256

void app_main(void) {
  initialize_nvs();
  initialize_console();

  esp_err_t err;

  nvs_handle_t nvs_handle;
  err = nvs_open("wifi_cred", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    printf("Error opening wifi_cred: %s\n", esp_err_to_name(err));
    return;
  } else {
    printf("Opened wifi_cred.\n");
  }

  char wifi_ssid[MAX_WIFI_SSID_LENGTH];
  err = initialize_nvs_str(nvs_handle, "ssid", wifi_ssid, MAX_WIFI_SSID_LENGTH);
  if (err != ESP_OK) {
    printf("Failed to read or initialize NVS WiFi SSID: %s\n",
           esp_err_to_name(err));
  }

  char wifi_password[MAX_WIFI_PASSWORD_LENGTH];
  err = initialize_nvs_str(nvs_handle, "password", wifi_password,
                           MAX_WIFI_PASSWORD_LENGTH);
  if (err != ESP_OK) {
    printf("Failed to read or initialize NVS WiFi password: %s\n",
           esp_err_to_name(err));
  }

  initialize_wifi(wifi_ssid, wifi_password);

  err = nvs_open("app_cred", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    printf("Error opening app_cred: %s\n", esp_err_to_name(err));
    return;
  } else {
    printf("Opened app_cred.\n");
  }

  char api_key[MAX_APP_API_KEY_LENGTH];
  err = initialize_nvs_str(nvs_handle, "api_key", api_key,
                           MAX_APP_API_KEY_LENGTH);
  if (err != ESP_OK) {
    printf("Failed to read or initialize NVS app API key: %s\n",
           esp_err_to_name(err));
  }

  err = nvs_open("user_cred", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    printf("Error opening user_cred: %s\n", esp_err_to_name(err));
    return;
  } else {
    printf("Opened user_cred.\n");
  }

  char user_email[MAX_USER_EMAIL_LENGTH];
  err = initialize_nvs_str(nvs_handle, "email", user_email,
                           MAX_USER_EMAIL_LENGTH);
  if (err != ESP_OK) {
    printf("Failed to read or initialize NVS user email: %s\n",
           esp_err_to_name(err));
  }

  char user_password[MAX_USER_PASSWORD_LENGTH];
  err = initialize_nvs_str(nvs_handle, "password", user_password,
                           MAX_USER_PASSWORD_LENGTH);
  if (err != ESP_OK) {
    printf("Failed to read or initialize NVS user password: %s\n",
           esp_err_to_name(err));
  }

  nvs_close(nvs_handle);

  xTaskCreate(&get_device_state, "get_device_state", 4096, NULL, 5, NULL);
}
