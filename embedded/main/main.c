#include "console_utils.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "nvs_utils.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define MAX_WIFI_SSID_LENGTH 32
#define MAX_WIFI_PASSWORD_LENGTH 64

#define MAX_WIFI_RETRY 5
#define WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK

/**s_wifi_event_group (below) allows multiple bits for each event, but we only
 * care about two: (1) we've connected to the AP with an IP, or (2) we've failed
 * to connect after the maximum number of retries. */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define MAX_APP_API_KEY_LENGTH 256

#define MAX_USER_EMAIL_LENGTH 256
#define MAX_USER_PASSWORD_LENGTH 256

#define MAX_HTTP_RX_BUFFER 1024
#define MAX_HTTP_TX_BUFFER 1024

static const char *TAG = "embedded";

/**FreeRTOS event group to signal when we are connected. */
static EventGroupHandle_t s_wifi_event_group;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  static int s_retry_count = 0;

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_count < MAX_WIFI_RETRY) {
      esp_wifi_connect();
      s_retry_count++;
      ESP_LOGI(TAG, "Trying to connect to access point again.");
    } else {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGI(TAG, "Failed to connect to access point.");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_count = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void initialize_wifi(const char *ssid, const char *password) {
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL,
      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL,
      &instance_got_ip));

  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = "",
              .password = "",
              .threshold.authmode = WIFI_SCAN_AUTH_MODE_THRESHOLD,
              .sae_pwe_h2e = WIFI_SAE_MODE,
          },
  };

  /**Copy SSID and password into wifi_config struct. */
  strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
  strncpy((char *)wifi_config.sta.password, password,
          sizeof(wifi_config.sta.password) - 1);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "initialize_wifi finished.");

  /**Wait until connection is established (WIFI_CONNECTED_BIT) or fails for the
   * maximum number of re-tries (WIFI_FAIL_BIT). Bits set by event_handler()
   * above. */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  /**xEventGroupWaitBits() returns bits before call returned so we can test
   * which event actually happened. */
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Connected to SSID %s with password %s", ssid, password);
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "Failed to connect to SSID %s with password %s", ssid,
             password);
  } else {
    ESP_LOGE(TAG, "Unexpected event connecting to SSID %s with password %s",
             ssid, password);
  }
}

esp_err_t http_event_handler(esp_http_client_event_t *event) {
  static char *output_buffer;
  static int output_len;

  switch (event->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", event->header_key,
             event->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);
    if (output_buffer == NULL) {
      output_buffer = (char *)calloc(MAX_HTTP_TX_BUFFER + 1, sizeof(char));
      if (output_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for output buffer.");
        return ESP_FAIL;
      }
    }

    int copy_len = MIN(event->data_len, (MAX_HTTP_TX_BUFFER - output_len));
    if (copy_len) {
      memcpy(output_buffer + output_len, event->data, copy_len);
      output_len += copy_len;
      output_buffer[output_len] = (char){0};
    }

    if (output_len >= 2 &&
        strcmp(&output_buffer[output_len - 2], "\n\n") == 0) {
      output_len -= 2;
      output_buffer[output_len] = (char){0};
    }

    if (output_len > 0) {
      ESP_LOG_BUFFER_CHAR(TAG, output_buffer, output_len);
      memset(output_buffer, 0, MAX_HTTP_TX_BUFFER);
      output_len = 0;
      output_buffer[output_len] = (char){0};
    }

    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
    if (output_buffer != NULL) {
      ESP_LOG_BUFFER_CHAR(TAG, output_buffer, output_len);
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    int mbedtls_err = 0;
    esp_err_t err = esp_tls_get_and_clear_last_error(
        (esp_tls_error_handle_t)event->data, &mbedtls_err, NULL);
    if (err != 0) {
      ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
      ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
    }
    if (output_buffer != NULL) {
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
    break;
  }
  return ESP_OK;
}

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
