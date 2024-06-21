#include "controllers/post_device_register.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "state/app_credentials.h"
#include "state/device.h"
#include <string.h>
#include <sys/param.h>

static const char *TAG = "controllers/post_device_register";

static esp_err_t parse_response(const char *json_response, device_t *device) {
  cJSON *json = cJSON_Parse(json_response);
  if (json == NULL) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return ESP_FAIL;
  }

  if (!cJSON_IsArray(json) || cJSON_GetArraySize(json) != 1) {
    ESP_LOGE(TAG, "JSON response is not a single-element array.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON *item = cJSON_GetArrayItem(json, 0);
  if (!cJSON_IsObject(item)) {
    ESP_LOGE(TAG, "The array item is not a JSON object.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON *id_item = cJSON_GetObjectItem(item, "id");
  if (cJSON_IsString(id_item) && (id_item->valuestring != NULL)) {
    strncpy(device->id, id_item->valuestring, MAX_ID_LENGTH - 1);
    device->id[MAX_ID_LENGTH - 1] = '\0';
  } else {
    ESP_LOGE(TAG, "Failed to extract 'id' from JSON response.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON *time_format_item = cJSON_GetObjectItem(item, "time_format");
  if (cJSON_IsString(time_format_item) &&
      (time_format_item->valuestring != NULL)) {
    strncpy(device->time_format, time_format_item->valuestring,
            MAX_TIME_FORMAT_LENGTH - 1);
    device->time_format[MAX_TIME_FORMAT_LENGTH - 1] = '\0';
  } else {
    ESP_LOGE(TAG, "Failed to extract 'time_format' from JSON response.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON *utc_offset_item = cJSON_GetObjectItem(item, "utc_offset");
  if (cJSON_IsNumber(utc_offset_item)) {
    device->utc_offset = utc_offset_item->valueint;
  } else {
    ESP_LOGE(TAG, "Failed to extract 'utc_offset' from JSON response.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON *brightness_item = cJSON_GetObjectItem(item, "brightness");
  if (cJSON_IsNumber(brightness_item)) {
    device->brightness = brightness_item->valueint;
  } else {
    ESP_LOGE(TAG, "Failed to extract 'brightness' from JSON response.");
    cJSON_Delete(json);
    return ESP_FAIL;
  }

  cJSON_Delete(json);
  return ESP_OK;
}

esp_err_t post_device_register_event_handler(esp_http_client_event_t *event) {
  static device_t *device = NULL;
  if (event->user_data) {
    device = (device_t *)event->user_data;
  }
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

      esp_err_t err = parse_response(output_buffer, device);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to parse device info.");
      }

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

esp_err_t post_device_register(device_t *device) {
  int port = atoi(CONFIG_PORT);

  esp_http_client_config_t config = {
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .host = CONFIG_HOST,
      .port = port,
      .path = "/device/register",
      .event_handler = post_device_register_event_handler,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .buffer_size = MAX_HTTP_RX_BUFFER,
      .buffer_size_tx = MAX_HTTP_TX_BUFFER,
      .is_async = true,
      .method = HTTP_METHOD_POST,
      .timeout_ms = 60000,
      .user_data = device,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  size_t auth_header_length = strlen("Bearer ") + strlen(get_auth_token()) + 1;
  char *auth_header = malloc(auth_header_length);
  if (auth_header == NULL) {
    ESP_LOGE(TAG, "Failed to allocate memory for auth header.");
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

  free(auth_header);
  esp_http_client_cleanup(client);

  return err;
}
