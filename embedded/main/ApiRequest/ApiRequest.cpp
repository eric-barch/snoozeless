#include "ApiRequest.h"
#include "Session.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include <string>

#define MAX_HTTP_RX_BUFFER 1024
#define MAX_HTTP_TX_BUFFER 1024

static const char *TAG = "ApiRequest";

ApiRequest::ApiRequest(Session &session, esp_http_client_method_t method,
                       int timeout_ms, const std::string &path,
                       const std::string &query,
                       OnDataCallback on_data_callback)
    : session(session), method(method), timeout_ms(timeout_ms), path(path),
      query(query), on_data_callback(on_data_callback) {}

void ApiRequest::set_on_data_callback(OnDataCallback on_data_callback) {
  this->on_data_callback = on_data_callback;
}

ApiRequest::OnDataCallback ApiRequest::get_on_data_callback() {
  return this->on_data_callback;
}

esp_err_t ApiRequest::http_event_handler(esp_http_client_event_t *event) {
  ApiRequest *api_request = static_cast<ApiRequest *>(event->user_data);
  ApiRequest::OnDataCallback on_data_callback =
      api_request->get_on_data_callback();

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

    if (event->data_len <= 0) {
      ESP_LOGE(TAG, "No data. Exiting event handler.");
      break;
    }

    output_buffer.append(static_cast<const char *>(event->data),
                         event->data_len);
    ESP_LOG_BUFFER_CHAR(TAG, output_buffer.c_str(), output_buffer.size());

    if (status_code < 200 || status_code >= 300) {
      ESP_LOGE(TAG, "Error response code: %d. Exiting event handler.",
               status_code);
      break;
    }

    if (on_data_callback == nullptr) {
      ESP_LOGW(TAG, "No on_data_callback. Exiting event handler.");
      break;
    }

    on_data_callback(output_buffer);
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

void ApiRequest::call_task(void *pvParameters) {
  ApiRequest *api_request = static_cast<ApiRequest *>(pvParameters);

  esp_http_client_config_t config = {
      .host = "192.168.1.6",
      .port = 3000,
      .path = api_request->path.c_str(),
      .query = api_request->query.c_str(),
      .method = api_request->method,
      .timeout_ms = api_request->timeout_ms,
      .event_handler = &ApiRequest::http_event_handler,
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .buffer_size = MAX_HTTP_RX_BUFFER,
      .buffer_size_tx = MAX_HTTP_TX_BUFFER,
      .user_data = api_request,
      .is_async = true,
      .crt_bundle_attach = esp_crt_bundle_attach,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  std::string auth_bearer_header =
      "Bearer " + api_request->session.get_auth_bearer_token();
  ESP_LOGD(TAG, "auth_bearer_header: %s", auth_bearer_header.c_str());
  esp_http_client_set_header(client, "Authorization",
                             auth_bearer_header.c_str());

  std::string refresh_token_header = api_request->session.get_refresh_token();
  ESP_LOGD(TAG, "refresh_token_header: %s", refresh_token_header.c_str());
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

  vTaskDelete(NULL);
}

void ApiRequest::call() {
  xTaskCreate(&ApiRequest::call_task, "call_task", 4096, this, 5, NULL);
}
