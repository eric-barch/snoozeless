#include "Session.h"
#include "NvsManager.h"
#include "esp_log.h"
#include <string>

static const char *TAG = "Session";

Session::Session(NvsManager &nvs_manager) : nvs_manager(nvs_manager) {
  esp_err_t err =
      nvs_manager.read_string("session", "auth_bearer", auth_bearer_token);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "Authorization Bearer token not found in NVS. Using config.");
    auth_bearer_token = CONFIG_AUTH_BEARER_TOKEN;
    nvs_manager.write_string("session", "auth_bearer", auth_bearer_token);
  }

  err = nvs_manager.read_string("session", "refresh", refresh_token);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "Refresh token not found in NVS. Using config.");
    refresh_token = CONFIG_REFRESH_TOKEN;
    nvs_manager.write_string("session", "refresh", refresh_token);
  }
};

Session::~Session() {};

std::string Session::get_auth_bearer_token() { return auth_bearer_token; }

std::string Session::get_refresh_token() { return refresh_token; }

void Session::keep_active() {};
