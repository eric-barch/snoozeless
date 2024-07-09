#include "Session.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_client.h"
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

void Session::refresh_on_data(void *session_instance,
                              const std::string &response) {
  Session *self = static_cast<Session *>(session_instance);
  ESP_LOGI(TAG, "Response: %s", response.c_str());

  cJSON *json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  cJSON *access_token_item = cJSON_GetObjectItem(json_response, "accessToken");
  if (!cJSON_IsString(access_token_item) ||
      (access_token_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `accessToken` from JSON response");
    return;
  }

  self->set_auth_bearer_token(access_token_item->valuestring);

  cJSON *refresh_token_item =
      cJSON_GetObjectItem(json_response, "refreshToken");
  if (!cJSON_IsString(refresh_token_item) ||
      (refresh_token_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `refreshToken` from JSON response");
    return;
  }

  self->set_refresh_token(refresh_token_item->valuestring);

  cJSON_Delete(json_response);
}

/**NOTE: Does not return until `post_auth_refresh` destructs. */
void Session::refresh() {
  ApiRequest post_auth_refresh =
      ApiRequest(*this, HTTP_METHOD_POST, 60000, "/auth/refresh", "", this,
                 refresh_on_data);
  post_auth_refresh.send_request();
}

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

  this->refresh();
}
