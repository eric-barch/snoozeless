#include "Session.h"
#include "NvsManager.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <string>

static const char *TAG = "Session";

Session::Session(NvsManager &nvs_manager) : nvs_manager(nvs_manager) {
  this->init();
};

void Session::init() {
  esp_err_t err = this->nvs_manager.read_string("session", "auth_bearer",
                                                this->auth_bearer_token);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Auth bearer token read from NVS: %s",
             this->auth_bearer_token.c_str());
  } else {
    this->auth_bearer_token = CONFIG_AUTH_BEARER_TOKEN;
    this->nvs_manager.write_string("session", "auth_bearer",
                                   this->auth_bearer_token);
    ESP_LOGW(TAG,
             "Error reading auth bearer token from NVS: %s. \nUsing config: %s",
             esp_err_to_name(err), this->auth_bearer_token.c_str());
  }

  err =
      this->nvs_manager.read_string("session", "refresh", this->refresh_token);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Refresh token read from NVS: %s",
             this->refresh_token.c_str());
  } else {
    this->refresh_token = CONFIG_REFRESH_TOKEN;
    this->nvs_manager.write_string("session", "refresh", this->refresh_token);
    ESP_LOGW(TAG,
             "Error reading refresh token from NVS: %s. \nUsing config: %s",
             esp_err_to_name(err), this->refresh_token.c_str());
  }
}

std::string Session::get_auth_bearer_token() { return this->auth_bearer_token; }

std::string Session::get_refresh_token() { return this->refresh_token; }

void Session::keep_active() {};
