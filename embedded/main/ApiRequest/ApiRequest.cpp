#ifndef API_REQUEST_CPP
#define API_REQUEST_CPP

#include "ApiRequest.h"
#include "Session.h"
#include <esp_crt_bundle.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <string>

template <typename CallerType>
ApiRequest<CallerType>::ApiRequest(Session &session, CallerType &caller,
                                   const esp_http_client_method_t &method,
                                   const int &timeout_ms,
                                   const std::string &path,
                                   const std::string &query)
    : session(session), caller(caller), method(method), timeout_ms(timeout_ms),
      path(path), query(query), client(),
      is_connected(xSemaphoreCreateBinary()) {
  xSemaphoreGive(is_connected);
}

template <typename CallerType> ApiRequest<CallerType>::~ApiRequest() {
  ESP_LOGI(TAG, "Destroy.");
}

template <typename ClientType>
esp_err_t ApiRequest<ClientType>::send_request() {
  TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
  const char *taskName = pcTaskGetName(currentTask);

  ESP_LOGE(TAG, "%s", taskName);

  xSemaphoreTake(is_connected, 0);
  xTaskCreate(&ApiRequest::handle_request, "request", 8192, this, 5, nullptr);
  xSemaphoreTake(is_connected, portMAX_DELAY);

  int status_code = esp_http_client_get_status_code(this->client);
  if (status_code < 200 || status_code >= 300) {
    return ESP_FAIL;
  } else {
    return ESP_OK;
  }
}

template <typename CallerType>
const char *const ApiRequest<CallerType>::TAG = "api_req";

template <typename CallerType>
const int ApiRequest<CallerType>::MAX_HTTP_RX_BUFFER = 1024;

template <typename CallerType>
const int ApiRequest<CallerType>::MAX_HTTP_TX_BUFFER = 1024;

template <typename CallerType>
esp_err_t
ApiRequest<CallerType>::handle_event(esp_http_client_event_t *const event) {
  ApiRequest *self = static_cast<ApiRequest *>(event->user_data);
  CallerType &caller = self->caller;

  std::string output_buffer;
  int status_code = esp_http_client_get_status_code(event->client);

  switch (event->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER");
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA");

    if (event->data_len <= 0) {
      ESP_LOGE(TAG, "No data. Exiting event handler.");
      break;
    }

    output_buffer.append(static_cast<const char *>(event->data),
                         event->data_len);

    if (status_code < 200 || status_code >= 300) {
      ESP_LOGW(TAG, "output_buffer: %s", output_buffer.c_str());
      ESP_LOGE(TAG, "Error response code: %d. Exiting event handler.",
               status_code);
      break;
    }

    caller.on_data(output_buffer);
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

template <typename ClientType>
void ApiRequest<ClientType>::handle_request(void *const pvParameters) {
  ApiRequest *self = static_cast<ApiRequest *>(pvParameters);

  esp_http_client_config_t config = {
      .host = "192.168.1.4",
      .port = 3000,
      .path = self->path.c_str(),
      .query = self->query.c_str(),
      .method = self->method,
      .timeout_ms = self->timeout_ms,
      .event_handler = handle_event,
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

#endif // API_REQUEST_CPP
