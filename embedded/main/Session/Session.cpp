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
  esp_err_t err =
      this->nvs_manager.read_string("session", "access", this->access_token);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Access token read from NVS: %s", this->access_token.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading access token from NVS: %s. Using config.",
             esp_err_to_name(err));
    this->set_access_token(CONFIG_ACCESS_TOKEN);
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
};

Session::~Session() { ESP_LOGI(TAG, "Destruct."); }

void Session::set_access_token(std::string access_token) {
  this->access_token = access_token;
  this->nvs_manager.write_string("session", "access", access_token);
  ESP_LOGI(TAG, "Set Access Token: %s", access_token.c_str());
}

std::string Session::get_access_token() { return this->access_token; }

void Session::set_refresh_token(std::string refresh_token) {
  this->refresh_token = refresh_token;
  this->nvs_manager.write_string("session", "refresh", refresh_token);
  ESP_LOGI(TAG, "Set Refresh Token: %s", refresh_token.c_str());
}

std::string Session::get_refresh_token() { return this->refresh_token; }

void Session::refresh_on_data(void *session_instance,
                              const std::string &response) {
  Session *self = static_cast<Session *>(session_instance);

  cJSON *json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  cJSON *access_token_item = cJSON_GetObjectItem(json_response, "access_token");
  if (!cJSON_IsString(access_token_item) ||
      (access_token_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `access_token` from JSON response.");
  } else {
    self->set_access_token(access_token_item->valuestring);
  }

  cJSON *refresh_token_item =
      cJSON_GetObjectItem(json_response, "refresh_token");
  if (!cJSON_IsString(refresh_token_item) ||
      (refresh_token_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `refresh_token` from JSON response.");
  } else {
    self->set_refresh_token(refresh_token_item->valuestring);
  }

  cJSON_Delete(json_response);
}

/**NOTE: Does not return until `post_auth_refresh` destructs. */
void Session::refresh() {
  ApiRequest post_auth_refresh = ApiRequest(
      *this, this, refresh_on_data, HTTP_METHOD_POST, 60000, "/auth/refresh");
  post_auth_refresh.send_request();
}
