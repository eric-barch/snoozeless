#include "controllers/get_real_time.h"
#include "app_credentials.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"
#include "state/app_credentials.h"
#include "state/real_time.h"
#include "utilities/wifi.h"
#include <string.h>
#include <sys/param.h>

static const char *TAG = "controllers/get_real_time";

static esp_err_t parse_response(const char *response) {
  cJSON *json = cJSON_Parse(response);
  if (json == NULL) {
    ESP_LOGE(TAG, "Failed to parse JSON data.");
    return ESP_FAIL;
  }

  if (!cJSON_IsObject(json)) {
    ESP_LOGE(TAG, "JSON data is not an object.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON *unix_time_item = cJSON_GetObjectItem(json, "unix_time");
  if (cJSON_IsNumber(unix_time_item)) {
    set_real_time_unix(unix_time_item->valueint);
    set_real_time_ticks(xTaskGetTickCount());
  } else {
    ESP_LOGE(TAG, "Failed to extract 'unix_time' from JSON data.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON_Delete(json);
  return ESP_OK;
}

static esp_err_t get_real_time_event_handler(esp_http_client_event_t *event) {
  static char *output_buffer;
  static int output_length;

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

    int copy_len = MIN(event->data_len, (MAX_HTTP_TX_BUFFER - output_length));
    if (copy_len) {
      memcpy(output_buffer + output_length, event->data, copy_len);
      output_length += copy_len;
      output_buffer[output_length] = (char){0};
    }

    if (output_length >= 2 &&
        strcmp(&output_buffer[output_length - 2], "\n\n") == 0) {
      output_length -= 2;
      output_buffer[output_length] = (char){0};
    }

    if (output_length > 0) {
      ESP_LOG_BUFFER_CHAR(TAG, output_buffer, output_length);
      parse_response(output_buffer);
      memset(output_buffer, 0, MAX_HTTP_TX_BUFFER);
      output_length = 0;
      output_buffer[output_length] = (char){0};
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
    if (output_buffer != NULL) {
      ESP_LOG_BUFFER_CHAR(TAG, output_buffer, output_length);
      free(output_buffer);
      output_buffer = NULL;
    }
    output_length = 0;
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
    output_length = 0;
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
    break;
  }
  return ESP_OK;
}

esp_err_t get_real_time(void) {
  int port = atoi(CONFIG_PORT);

  esp_http_client_config_t config = {
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .host = CONFIG_HOST,
      .port = port,
      .path = "/real-time",
      .event_handler = get_real_time_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .buffer_size = MAX_HTTP_RX_BUFFER,
      .buffer_size_tx = MAX_HTTP_TX_BUFFER,
      .is_async = true,
      .timeout_ms = 300000,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  size_t auth_header_length = strlen("Bearer ") + strlen(get_auth_token()) + 1;
  char *auth_header = malloc(auth_header_length);
  if (auth_header == NULL) {
    ESP_LOGE(TAG, "Failed to allocate memory for auth header.");
    vTaskDelete(NULL);
    return ESP_FAIL;
  }
  snprintf(auth_header, auth_header_length, "Bearer %s", get_auth_token());

  esp_http_client_set_header(client, "Authorization", auth_header);
  esp_http_client_set_header(client, "Refresh-Token", get_refresh_token());

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

  return err;
}
