#include "esp_crt_bundle.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "snoozeless_embedded";

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
  static char *output_buffer;
  static int output_len;

  switch (evt->event_id) {
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
    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
             evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    /* Clean the buffer in case of a new request. */
    if (output_len == 0 && evt->user_data) {
      /* We are just starting to copy the output data into the use. */
      memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
    }

    if (!esp_http_client_is_chunked_response(evt->client)) {
      /* If user_data buffer is configured, copy response into the buffer. */
      int copy_len = 0;
      if (evt->user_data) {
        /* Last byte in evt->user_data is kept for NULL character in case of
         * out-of-bounds access. */
        copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
        if (copy_len) {
          memcpy(evt->user_data + output_len, evt->data, copy_len);
        }
      } else {
        /* If Content-Length header is not provided, dynamically reallocate
         * buffer. */
        if (output_buffer == NULL) {
          output_buffer = (char *)calloc(evt->data_len + 1, sizeof(char));
          if (output_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
            return ESP_FAIL;
          }
        } else {
          char *new_output_buffer =
              (char *)realloc(output_buffer, output_len + evt->data_len + 1);
          if (new_output_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to reallocate memory for output buffer");
            free(output_buffer);
            return ESP_FAIL;
          }
          output_buffer = new_output_buffer;
        }

        copy_len = evt->data_len;
        memcpy(output_buffer + output_len, evt->data, copy_len);
      }
      output_len += copy_len;
      output_buffer[output_len] = 0;
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
        (esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
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
    esp_http_client_set_header(evt->client, "From", "user@example.com");
    esp_http_client_set_header(evt->client, "Accept", "text/html");
    esp_http_client_set_redirection(evt->client);
    break;
  }
  return ESP_OK;
}

static void https_async(void *pvParameters) {
  esp_http_client_config_t config = {
      .url = "https://192.168.1.6:3000",
      .event_handler = _http_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .is_async = true,
      .timeout_ms = 30000,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_err_t err;

  while (1) {
    err = esp_http_client_perform(client);
    if (err != ESP_ERR_HTTP_EAGAIN) {
      break;
    }
  }

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %" PRId64,
             esp_http_client_get_status_code(client),
             esp_http_client_get_content_length(client));
  } else {
    ESP_LOGE(TAG, "Error performing http request: %s", esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
  vTaskDelete(NULL);
}

void app_main(void) {
  esp_err_t err = nvs_flash_init();

  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  ESP_ERROR_CHECK(err);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(example_connect());
  ESP_LOGI(TAG, "Connected to Access Point.");

  xTaskCreate(&https_async, "https_async", 8192, NULL, 5, NULL);
}
