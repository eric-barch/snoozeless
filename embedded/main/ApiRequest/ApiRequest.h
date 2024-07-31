#ifndef API_REQUEST_H
#define API_REQUEST_H

#include "Session.h"
#include <esp_err.h>
#include <esp_http_client.h>
#include <string>

template <typename CallerType> class ApiRequest {
public:
  ApiRequest(Session &session, CallerType &caller,
             const esp_http_client_method_t &method, const int &timeout_ms,
             const std::string &path, const std::string &query = "");

  ~ApiRequest();

  esp_err_t send_request();

private:
  static const char *const TAG;
  static const int MAX_HTTP_RX_BUFFER;
  static const int MAX_HTTP_TX_BUFFER;

  Session &session;
  CallerType &caller;
  esp_http_client_method_t method;
  int timeout_ms;
  std::string path;
  std::string query;
  esp_http_client_handle_t client;
  SemaphoreHandle_t is_connected;

  static esp_err_t handle_event(esp_http_client_event_t *const event);
  static void handle_request(void *const pvParameters);
};

#include "ApiRequest.cpp"

#endif // API_REQUEST_H
