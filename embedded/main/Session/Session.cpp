#include "Session.h"
#include "ApiRequest.h"
#include "NonVolatileStorage.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <sdkconfig.h>
#include <string>

Session::Session(NonVolatileStorage &non_volatile_storage)
    : non_volatile_storage(non_volatile_storage), access_token(),
      refresh_token() {
  esp_err_t err = non_volatile_storage.read(TAG, "access", access_token);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Access token read from NVS.");
    set_access_token(access_token);
  } else {
    ESP_LOGW(TAG, "Error reading access token from NVS: %s. Using config.",
             esp_err_to_name(err));
    set_access_token(CONFIG_ACCESS_TOKEN);
  }

  err = non_volatile_storage.read(TAG, "refresh", refresh_token);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Refresh token read from NVS.");
    set_refresh_token(refresh_token);
  } else {
    ESP_LOGW(TAG, "Error reading refresh token from NVS: %s. Using config.",
             esp_err_to_name(err));
    set_refresh_token(CONFIG_REFRESH_TOKEN);
  }

  xTaskCreate(keep_refreshed, "keep_refreshed", 2048, this, 5, NULL);
};

Session::~Session() { ESP_LOGI(TAG, "Destroy."); }

std::string Session::get_access_token() const { return access_token; }

std::string Session::get_refresh_token() const { return refresh_token; }

void Session::on_data(const std::string &response) { parse(response); }

const char *const Session::TAG = "session";

void Session::set_access_token(const std::string &access_token) {
  this->access_token = access_token;
  non_volatile_storage.write(TAG, "access", access_token);
  ESP_LOGI(TAG, "Set Access Token: %s", access_token.c_str());
}

void Session::set_refresh_token(const std::string &refresh_token) {
  this->refresh_token = refresh_token;
  non_volatile_storage.write(TAG, "refresh", refresh_token);
  ESP_LOGI(TAG, "Set Refresh Token: %s", refresh_token.c_str());
}

void Session::keep_refreshed(void *const pvParameters) {
  Session *self = static_cast<Session *>(pvParameters);

  while (true) {
    ESP_LOGI(TAG, "Refreshing.");
    esp_err_t err = self->refresh();

    while (err != ESP_OK) {
      ESP_LOGI(TAG, "Refresh failed. Will try again in ten seconds.");
      vTaskDelay(pdMS_TO_TICKS(10000));
      err = self->refresh();
    }

    ESP_LOGI(TAG, "Refresh successful.");

    /**Tokens are valid for 1 hour. Refresh every 55 minutes. */
    vTaskDelay(pdMS_TO_TICKS(3300000));
  }

  vTaskDelete(NULL);
}

void Session::parse(const std::string &data) {
  cJSON *const json_response = cJSON_Parse(data.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  const cJSON *const access_token_json =
      cJSON_GetObjectItem(json_response, "access_token");
  if (!cJSON_IsString(access_token_json) ||
      (access_token_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `access_token` from JSON response.");
  } else {
    set_access_token(access_token_json->valuestring);
  }

  const cJSON *const refresh_token_json =
      cJSON_GetObjectItem(json_response, "refresh_token");
  if (!cJSON_IsString(refresh_token_json) ||
      (refresh_token_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `refresh_token` from JSON response.");
  } else {
    set_refresh_token(refresh_token_json->valuestring);
  }

  cJSON_Delete(json_response);
}

esp_err_t Session::refresh() {
  ApiRequest post_auth_refresh = ApiRequest<Session>(
      *this, *this, HTTP_METHOD_POST, 60000, "/auth/refresh", "");
  esp_err_t err = post_auth_refresh.send_request();
  return err;
}
