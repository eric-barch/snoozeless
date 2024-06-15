#include "console_utils.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "http_utils.h"
#include "nvs_utils.h"
#include "wifi_utils.h"

#define MAX_APP_API_KEY_LENGTH 256

#define MAX_USER_EMAIL_LENGTH 256
#define MAX_USER_PASSWORD_LENGTH 256

static const char *TAG = "embedded";

static void read_device_state_stream(void *pvParameters) {
  char *query_prefix = "deviceId=";
  size_t query_len = strlen(query_prefix) + strlen(CONFIG_DEVICE_ID) + 1;
  char *query = malloc(query_len);
  if (query == NULL) {
    ESP_LOGE(TAG, "Failed to allocate memory for query.");
    vTaskDelete(NULL);
    return;
  }
  snprintf(query, query_len, "%s%s", query_prefix, CONFIG_DEVICE_ID);

  int port = atoi(CONFIG_PORT);

  esp_http_client_config_t config = {
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .host = CONFIG_HOST,
      .port = port,
      .path = "/device/state",
      .query = query,
      .event_handler = http_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .buffer_size = MAX_HTTP_RX_BUFFER,
      .buffer_size_tx = MAX_HTTP_TX_BUFFER,
      .is_async = true,
      .timeout_ms = 300000,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  free(query);

  char *auth_header_prefix = "Bearer ";
  size_t auth_header_len =
      strlen(auth_header_prefix) + strlen(CONFIG_AUTH_TOKEN) + 1;
  char *auth_header = malloc(auth_header_len);
  if (auth_header == NULL) {
    ESP_LOGE(TAG, "Failed to allocate memory for auth header.");
    vTaskDelete(NULL);
    return;
  }
  snprintf(auth_header, auth_header_len, "%s%s", auth_header_prefix,
           CONFIG_AUTH_TOKEN);

  esp_http_client_set_header(client, "Authorization", auth_header);
  free(auth_header);

  esp_http_client_set_header(client, "Refresh-Token", CONFIG_REFRESH_TOKEN);

  esp_err_t err;

  while (1) {
    err = esp_http_client_perform(client);
    if (err != ESP_ERR_HTTP_EAGAIN) {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %" PRId64,
             esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));
  } else {
    ESP_LOGE(TAG, "Error performing http request: %s", esp_err_to_name(err));
  }

  esp_http_client_close(client);
  esp_http_client_cleanup(client);

  vTaskDelete(NULL);
}

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

  xTaskCreate(&read_device_state_stream, "read_device_state_stream", 4096, NULL,
              5, NULL);
}
