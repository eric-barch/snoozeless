#ifndef API_REQUEST_H
#define API_REQUEST_H

#include "Session.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include <string>

class ApiRequest {
public:
  typedef void (*OnDataCallback)(const std::string &);

  ApiRequest(Session &session, esp_http_client_method_t method, int timeout_ms,
             const std::string &path, const std::string &query = "",
             OnDataCallback on_data_callback = nullptr);

  void set_on_data_callback(OnDataCallback on_data_callback);
  OnDataCallback get_on_data_callback();
  void call();

private:
  Session &session;
  esp_http_client_method_t method;
  int timeout_ms;
  std::string path;
  std::string query;
  OnDataCallback on_data_callback;

  static esp_err_t http_event_handler(esp_http_client_event_t *event);
  static void call_task(void *pvParameters);
};

#endif // API_REQUEST_H
