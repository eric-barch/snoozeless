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

ApiRequest::ApiRequest(Session &session, const esp_http_client_method_t method,
                       const int timeout_ms, const std::string &path,
                       const std::string &query, void *caller_context,
                       OnDataCallback on_data)
    : session(session), method(method), timeout_ms(timeout_ms), path(path),
      query(query), caller_context(caller_context), on_data(on_data) {
  ESP_LOGI(TAG, "ApiRequest created with caller_context: %p and callback: %p",
           caller_context, (void *)on_data);
}

void ApiRequest::set_caller_context(void *caller_context) {
  this->caller_context = caller_context;
}

void *ApiRequest::get_caller_context() { return this->caller_context; }

void ApiRequest::set_on_data(OnDataCallback on_data) {
  this->on_data = on_data;
}

OnDataCallback ApiRequest::get_on_data() { return this->on_data; }

esp_err_t ApiRequest::http_event_handler(esp_http_client_event_t *event) {
  ApiRequest *api_request = static_cast<ApiRequest *>(event->user_data);
  void *caller_context = api_request->get_caller_context();
  OnDataCallback on_data = api_request->get_on_data();

  ESP_LOGI(TAG, "caller_context: %p", caller_context);
  ESP_LOGI(TAG, "on_data: %p", on_data);

  esp_err_t err = ESP_OK;
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
    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA");

    if (status_code < 200 || status_code >= 300) {
      ESP_LOGE(TAG, "Error response code: %d. Exiting event handler.",
               status_code);
      break;
    }

    if (caller_context == nullptr) {
      ESP_LOGW(TAG, "No caller_context. Exiting event handler.");
      break;
    }

    if (on_data == nullptr) {
      ESP_LOGW(TAG, "No on_data. Exiting event handler.");
      break;
    }

    if (event->data_len <= 0) {
      ESP_LOGE(TAG, "No data. Exiting event handler.");
      break;
    }

    output_buffer.append(static_cast<const char *>(event->data),
                         event->data_len);
    ESP_LOGI(TAG, "on_data %p called with caller_context %p", on_data,
             caller_context);
    on_data(caller_context, output_buffer);
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

  return err;
}

void ApiRequest::send_request_task(void *pvParameters) {
  ApiRequest *api_request = static_cast<ApiRequest *>(pvParameters);

  void *caller_context = api_request->caller_context;
  OnDataCallback on_data = api_request->on_data;

  std::string simulated_response = "Data from your path!";
  if (on_data) {
    on_data(caller_context, simulated_response);
  }

  vTaskDelete(NULL);
}

void ApiRequest::send_request() {
  BaseType_t err = xTaskCreate(&ApiRequest::send_request_task,
                               "send_request_task", 8192, this, 5, NULL);
  if (err != pdPASS) {
    ESP_LOGE(TAG, "Failed to create send_request_task.");
  }
}
