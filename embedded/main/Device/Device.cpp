#include "Device.h"
#include "ApiRequest.h"
#include "CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session,
               CurrentTime &current_time, Display &display)
    : nvs_manager(nvs_manager), session(session), current_time(current_time),
      display(display) {
  this->init();
}

void Device::set_id(std::string id) {
  this->id = id;
  this->nvs_manager.write_string("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::enroll_on_data(void *device_instance,
                            const std::string &response) {
  Device *self = static_cast<Device *>(device_instance);
  ESP_LOGI(TAG, "Response: %s", response.c_str());

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

  cJSON *utc_offset_item = cJSON_GetObjectItem(json_response, "utc_offset");
  if (!cJSON_IsNumber(utc_offset_item)) {
    ESP_LOGE(TAG, "Failed to extract `utc_offset` from JSON response.");
  } else {
    self->current_time.set_utc_offset(utc_offset_item->valueint);
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

/**NOTE: Does not return until `post_device_register` destructs. */
void Device::enroll() {
  ApiRequest post_device_register =
      ApiRequest(session, HTTP_METHOD_POST, 60000, "/device/register", "", this,
                 enroll_on_data);
  post_device_register.send_request();
}

void Device::init() {
  esp_err_t err = this->nvs_manager.read_string("device", "id", this->id);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Device ID read from NVS: %s", this->id.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading Device ID from NVS: %s. Enrolling device.",
             esp_err_to_name(err));
    this->enroll();
  }
}
