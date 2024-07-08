#ifndef API_REQUEST_H
#define API_REQUEST_H

#include "Session.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include <string>

typedef void (*OnDataCallback)(void *caller_context,
                               const std::string &response);

class ApiRequest {
public:
  ApiRequest(Session &session, const esp_http_client_method_t method,
             const int timeout_ms, const std::string &path,
             const std::string &query = "", void *caller_context = nullptr,
             OnDataCallback on_data = nullptr);

  void send_request();

private:
  Session &session;
  esp_http_client_method_t method;
  int timeout_ms;
  std::string path;
  std::string query;
  void *caller_context;
  OnDataCallback on_data;

  static esp_err_t http_event_handler(esp_http_client_event_t *event);
  static void send_request_task(void *pvParameters);
};

#endif // API_REQUEST_H
