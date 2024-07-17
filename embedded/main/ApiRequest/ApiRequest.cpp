#include "ApiRequest.h"
#include "Session.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include <string>

#define MAX_HTTP_RX_BUFFER 1024
#define MAX_HTTP_TX_BUFFER 1024

static const char *TAG = "ApiRequest";

ApiRequest::ApiRequest(Session &session, void *calling_object,
                       OnDataCallback on_data,
                       const esp_http_client_method_t method,
                       const int timeout_ms, const std::string &path,
                       const std::string &query)
    : session(session), calling_object(calling_object), on_data(on_data),
      method(method), timeout_ms(timeout_ms), path(path), query(query) {
  this->is_connected = xSemaphoreCreateBinary();
  xSemaphoreGive(this->is_connected);

  this->received_response = xSemaphoreCreateBinary();
  xSemaphoreGive(this->received_response);
}

ApiRequest::~ApiRequest() {
  xSemaphoreTake(this->is_connected, portMAX_DELAY);
  ESP_LOGI(TAG, "Destroy %s.", this->path.c_str());
}

esp_err_t ApiRequest::handle_http_event(esp_http_client_event_t *event) {
  ApiRequest *self = static_cast<ApiRequest *>(event->user_data);

  void *calling_object = self->calling_object;
  OnDataCallback on_data = self->on_data;

  std::string output_buffer;
  int status_code = esp_http_client_get_status_code(event->client);

  switch (event->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
    xSemaphoreTake(self->is_connected, 0);
    xSemaphoreGive(self->received_response);
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
    xSemaphoreTake(self->is_connected, 0);
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA");

    if (event->data_len <= 0) {
      ESP_LOGE(TAG, "No data. Exiting event handler.");
      xSemaphoreGive(self->received_response);
      break;
    }

    output_buffer.append(static_cast<const char *>(event->data),
                         event->data_len);

    if (status_code < 200 || status_code >= 300) {
      ESP_LOGW(TAG, "output_buffer: %s", output_buffer.c_str());
      ESP_LOGE(TAG, "Error response code: %d. Exiting event handler.",
               status_code);
      xSemaphoreGive(self->received_response);
      break;
    }

    on_data(calling_object, output_buffer);

    xSemaphoreGive(self->received_response);
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
    output_buffer.clear();
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    output_buffer.clear();
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
    break;
  }

  return ESP_OK;
}

void ApiRequest::send_task(void *pvParameters) {
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

  self->client = esp_http_client_init(&config);

  std::string access_header = "Bearer " + self->session.get_access_token();
  esp_http_client_set_header(self->client, "Authorization",
                             access_header.c_str());

  std::string refresh_token_header = self->session.get_refresh_token();
  esp_http_client_set_header(self->client, "Refresh-Token",
                             refresh_token_header.c_str());

  esp_err_t err;
  while (true) {
    err = esp_http_client_perform(self->client);
    if (err != ESP_ERR_HTTP_EAGAIN) {
      break;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  };

  esp_http_client_close(self->client);
  esp_http_client_cleanup(self->client);

  xSemaphoreGive(self->is_connected);
  vTaskDelete(NULL);
}

esp_err_t ApiRequest::send() {
  xSemaphoreTake(this->received_response, 0);
  xTaskCreate(&ApiRequest::send_task, "send_request_task", 8192, this, 5,
              nullptr);
  xSemaphoreTake(this->received_response, portMAX_DELAY);

  int status_code = esp_http_client_get_status_code(this->client);
  if (status_code < 200 || status_code >= 300) {
    return ESP_FAIL;
  } else {
    return ESP_OK;
  }
}
