#include "ApiRequest.h"
#include "Session.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "portmacro.h"
#include <string>

#define MAX_HTTP_RX_BUFFER 1024
#define MAX_HTTP_TX_BUFFER 1024

static const char *TAG = "ApiRequest";

ApiRequest::ApiRequest(Session &session, void *caller, OnDataCallback on_data,
                       const esp_http_client_method_t method,
                       const int timeout_ms, const std::string &path,
                       const std::string &query)
    : session(session), caller(caller), on_data(on_data), method(method),
      timeout_ms(timeout_ms), path(path), query(query) {
  this->is_open = xSemaphoreCreateBinary();
  xSemaphoreGive(this->is_open);
}

ApiRequest::~ApiRequest() {
  xSemaphoreTake(this->is_open, portMAX_DELAY);
  vSemaphoreDelete(this->is_open);
  ESP_LOGI(TAG, "Destruct.");
}

esp_err_t ApiRequest::handle_http_event(esp_http_client_event_t *event) {
  ApiRequest *api_request = static_cast<ApiRequest *>(event->user_data);
  void *caller = api_request->caller;
  OnDataCallback on_data = api_request->on_data;

  std::string output_buffer;
  int status_code = esp_http_client_get_status_code(event->client);

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
    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER");
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA");

    if (event->data_len <= 0) {
      ESP_LOGE(TAG, "No data. Exiting event handler.");
      break;
    }

    output_buffer.append(static_cast<const char *>(event->data),
                         event->data_len);

    if (status_code < 200 || status_code >= 300) {
      ESP_LOGW(TAG, "output_buffer: %s", output_buffer.c_str());
      ESP_LOGE(
          TAG,
          "Error response code: %d. output_buffer: %s. Exiting event handler.",
          status_code, output_buffer.c_str());
      break;
    }

    on_data(caller, output_buffer);
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
    output_buffer.clear();
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    output_buffer.clear();
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
    break;
  }

  return ESP_OK;
}

void ApiRequest::send_request_task(void *pvParameters) {
  ApiRequest *self = static_cast<ApiRequest *>(pvParameters);

  esp_http_client_config_t config = {
      .host = "192.168.1.8",
      .port = 3000,
      .path = self->path.c_str(),
      .query = self->query.c_str(),
      .method = self->method,
      .timeout_ms = self->timeout_ms,
      .event_handler = &ApiRequest::handle_http_event,
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .buffer_size = MAX_HTTP_RX_BUFFER,
      .buffer_size_tx = MAX_HTTP_TX_BUFFER,
      .user_data = self,
      .is_async = true,
      .crt_bundle_attach = esp_crt_bundle_attach,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  std::string access_header = "Bearer " + self->session.get_access_token();
  esp_http_client_set_header(client, "Authorization", access_header.c_str());

  std::string refresh_token_header = self->session.get_refresh_token();
  esp_http_client_set_header(client, "Refresh-Token",
                             refresh_token_header.c_str());

  esp_err_t err;

  while (true) {
    err = esp_http_client_perform(client);
    if (err != ESP_ERR_HTTP_EAGAIN) {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  };

  esp_http_client_close(client);
  esp_http_client_cleanup(client);

  xSemaphoreGive(self->is_open);
  vTaskDelete(NULL);
}

void ApiRequest::send_request() {
  xSemaphoreTake(this->is_open, 0);
  BaseType_t err = xTaskCreate(&ApiRequest::send_request_task,
                               "send_request_task", 8192, this, 5, NULL);
  if (err != pdPASS) {
    ESP_LOGE(TAG, "Failed to create send_request_task.");
    xSemaphoreGive(this->is_open);
  }
}
