#ifndef API_REQUEST_H
#define API_REQUEST_H

#include "Session.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "freertos/idf_additions.h"
#include <string>

typedef void (*OnDataCallback)(void *calling_object, const std::string &data);

class ApiRequest {
public:
  ApiRequest(Session &session, void *calling_object, OnDataCallback on_data,
             const esp_http_client_method_t method, const int timeout_ms,
             const std::string &path, const std::string &query,
             SemaphoreHandle_t calling_semaphore = nullptr);
  ~ApiRequest();

  void send_request();

private:
  Session &session;
  void *calling_object;
  OnDataCallback on_data;
  esp_http_client_method_t method;
  int timeout_ms;
  std::string path;
  std::string query;
  SemaphoreHandle_t calling_semaphore;

  static esp_err_t handle_http_event(esp_http_client_event_t *event);
  static void send_request_task(void *pvParameters);
};

#endif // API_REQUEST_H
