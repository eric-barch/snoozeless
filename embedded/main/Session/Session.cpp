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

void Session::set_auth_bearer_token(std::string auth_bearer_token) {
  this->auth_bearer_token = auth_bearer_token;
  this->nvs_manager.write_string("session", "auth_bearer", auth_bearer_token);
  ESP_LOGI(TAG, "Set Auth Bearer Token: %s", auth_bearer_token.c_str());
}

std::string Session::get_auth_bearer_token() { return this->auth_bearer_token; }

void Session::set_refresh_token(std::string refresh_token) {
  this->refresh_token = refresh_token;
  this->nvs_manager.write_string("session", "refresh", refresh_token);
  ESP_LOGI(TAG, "Set Refresh Token: %s", refresh_token.c_str());
}

std::string Session::get_refresh_token() { return this->refresh_token; }

void Session::init() {
  esp_err_t err = this->nvs_manager.read_string("session", "auth_bearer",
                                                this->auth_bearer_token);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Auth bearer token read from NVS: %s",
             this->auth_bearer_token.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading auth bearer token from NVS: %s. Using config.",
             esp_err_to_name(err));
    this->set_auth_bearer_token(CONFIG_AUTH_BEARER_TOKEN);
  }

  err =
      this->nvs_manager.read_string("session", "refresh", this->refresh_token);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Refresh token read from NVS: %s",
             this->refresh_token.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading refresh token from NVS: %s. Using config.",
             esp_err_to_name(err));
    this->set_refresh_token(CONFIG_REFRESH_TOKEN);
  }
}
