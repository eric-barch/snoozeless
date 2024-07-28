#ifndef API_REQUEST_H
#define API_REQUEST_H

#include "Session.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "freertos/idf_additions.h"
#include <string>

template <typename CallerType> class ApiRequest {
public:
  ApiRequest(Session &session, CallerType &caller,
             const esp_http_client_method_t method, const int timeout_ms,
             const std::string &path, const std::string &query);
  ~ApiRequest();

  esp_err_t send();

private:
  static constexpr const char *const TAG = "ApiRequest";
  Session &session;
  CallerType &caller;
  esp_http_client_method_t method;
  int timeout_ms;
  std::string path;
  std::string query;
  esp_http_client_handle_t client;
  SemaphoreHandle_t is_connected;
  SemaphoreHandle_t received_response;

  static esp_err_t handle_http_event(esp_http_client_event_t *event);
  static void send_task(void *pvParameters);
};

#include "ApiRequest.cpp"

#endif // API_REQUEST_H
