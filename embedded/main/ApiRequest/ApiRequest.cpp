#include "ApiRequest.h"
#include "Session.h"
#include "esp_crt_bundle.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include <string>

#define MAX_HTTP_RX_BUFFER 1024
#define MAX_HTTP_TX_BUFFER 1024

static const char *TAG = "ApiRequest";

ApiRequest::ApiRequest(Session &session, esp_http_client_method_t method,
                       int timeout_ms, const std::string &path,
                       const std::string &query,
                       OnDataCallback on_data_callback)
    : session(session), client(nullptr), method(method), timeout_ms(timeout_ms),
      path(path), query(query), on_data_callback(on_data_callback) {}

void ApiRequest::set_on_data_callback(OnDataCallback on_data_callback) {
  this->on_data_callback = on_data_callback;
}

esp_err_t ApiRequest::event_handler(esp_http_client_event_t *event) {
  ApiRequest *api_request = static_cast<ApiRequest *>(event->user_data);

  static std::string output_buffer;

  int mbedtls_err = 0;
  esp_err_t err = ESP_OK;

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
    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);

    if (event->data_len > 0) {
      output_buffer.append(static_cast<const char *>(event->data),
                           event->data_len);
    }

    if (!output_buffer.empty()) {
      ESP_LOG_BUFFER_CHAR(TAG, output_buffer.c_str(), output_buffer.size());

      ESP_LOGI(TAG, "foo1");
      if (api_request != nullptr) {
        ESP_LOGI(TAG, "foo2");
        api_request->on_data_callback(output_buffer);
      }

      output_buffer.clear();
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
    output_buffer.clear();
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)event->data,
                                           &mbedtls_err, nullptr);
    if (err != 0) {
      ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
      ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
    }
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

  esp_err_t err;

  while (true) {
    err = esp_http_client_perform(api_request->client);
    if (err != ESP_ERR_HTTP_EAGAIN) {
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  };

  int status_code = esp_http_client_get_status_code(api_request->client);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTP request successful. Status code: %d", status_code);
  } else {
    ESP_LOGE(TAG, "HTTP request failed. Status code: %d", status_code);
  }

  esp_http_client_close(api_request->client);
  esp_http_client_cleanup(api_request->client);
}

void ApiRequest::call() {
  esp_http_client_config_t config = {
      .host = "192.168.1.6",
      .port = 3000,
      .path = this->path.c_str(),
      .query = this->query.c_str(),
      .method = this->method,
      .timeout_ms = this->timeout_ms,
      .event_handler = this->event_handler,
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .buffer_size = MAX_HTTP_RX_BUFFER,
      .buffer_size_tx = MAX_HTTP_TX_BUFFER,
      .is_async = true,
      .crt_bundle_attach = esp_crt_bundle_attach,
  };

  this->client = esp_http_client_init(&config);

  std::string auth_bearer_header = "Bearer " + session.get_auth_bearer_token();
  esp_http_client_set_header(client, "Authorization",
                             auth_bearer_header.c_str());
  std::string refresh_token_header = session.get_refresh_token();
  esp_http_client_set_header(client, "Refresh-Token",
                             refresh_token_header.c_str());

  ApiRequest::call_task(this);
}
