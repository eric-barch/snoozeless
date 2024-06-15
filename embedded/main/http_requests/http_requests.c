#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "http_utils.h"
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "http_requests";

void get_device_state(void *pvParameters) {
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
