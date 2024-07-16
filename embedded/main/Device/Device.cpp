#include "Device.h"
#include "ApiRequest.h"
#include "CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session,
               CurrentTime &current_time, Display &display)
    : nvs_manager(nvs_manager), session(session), current_time(current_time),
      display(display) {
  this->is_blocked = xSemaphoreCreateBinary();
  xSemaphoreGive(this->is_blocked);

  std::string id;
  esp_err_t err = this->nvs_manager.read_string("device", "id", id);
  if (err == ESP_OK) {
    this->set_id(id);
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    this->enroll();
  }
}

Device::~Device() {}

void Device::set_id(std::string id) {
  this->id = id;
  this->nvs_manager.write_string("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::enroll_on_data(void *device, const std::string &response) {
  Device *self = static_cast<Device *>(device);

  cJSON *json_response = cJSON_Parse(response.c_str());
  if (!json_response) {
    ESP_LOGE(TAG, "Failed to parse JSON response.");
    return;
  }

  cJSON *id_item = cJSON_GetObjectItem(json_response, "id");
  if (!cJSON_IsString(id_item) || (id_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `id` from JSON response");
  } else {
    self->set_id(id_item->valuestring);
  }

  cJSON_Delete(json_response);
}

void Device::enroll() {
  xSemaphoreTake(this->is_blocked, 0);
  ApiRequest post_device_register =
      ApiRequest(this->session, this, enroll_on_data, HTTP_METHOD_POST, 60000,
                 "/device/register", "", this->is_blocked);
  post_device_register.send_request();
  xSemaphoreTake(this->is_blocked, portMAX_DELAY);
}
  cJSON *time_zone_item = cJSON_GetObjectItem(json_response, "time_zone");
  if (!cJSON_IsString(time_zone_item) ||
      (time_zone_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    self->current_time.set_time_zone(time_zone_item->valuestring);
  }

  cJSON *time_format_item = cJSON_GetObjectItem(json_response, "time_format");
  if (!cJSON_IsString(time_format_item) ||
      (time_format_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    self->current_time.set_format(time_format_item->valuestring);
  }

  cJSON *brightness_item = cJSON_GetObjectItem(json_response, "brightness");
  if (!cJSON_IsNumber(brightness_item)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    self->display.set_brightness(brightness_item->valueint);
  }

  cJSON_Delete(json_response);
}

}
