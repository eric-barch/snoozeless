#include "Device.h"
#include "Alarms.h"
#include "ApiRequest.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string>

Device::Device(NonVolatileStorage &non_volatile_storage, Session &session,
               CurrentTime &current_time, Alarms &alarms, Display &display,
               Buzzer &buzzer)
    : non_volatile_storage(non_volatile_storage), session(session),
      current_time(current_time), alarms(alarms), display(display),
      buzzer(buzzer), id() {
  esp_err_t err = non_volatile_storage.read(TAG, "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    enroll();
  }

  display.print_current_time();
}

Device::~Device() { ESP_LOGI(TAG, "Destroy."); }

std::string Device::get_id() { return id; }

void Device::on_data(const std::string &response) { parse(response); }

void Device::parse(const std::string &data) {
  cJSON *const device_json = cJSON_Parse(data.c_str());
  if (device_json == nullptr) {
    ESP_LOGE(TAG, "Failed to parse JSON device.");
    cJSON_Delete(device_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(device_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `id` from JSON response.");
  } else {
    set_id(id_json->valuestring);
  }

  const cJSON *const time_zone_json =
      cJSON_GetObjectItem(device_json, "time_zone");
  if (!cJSON_IsString(time_zone_json) ||
      (time_zone_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    current_time.set_time_zone(time_zone_json->valuestring);
  }

  const cJSON *const time_format_json =
      cJSON_GetObjectItem(device_json, "time_format");
  if (!cJSON_IsString(time_format_json) ||
      (time_format_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    current_time.set_format(time_format_json->valuestring);
  }

  const cJSON *const brightness_json =
      cJSON_GetObjectItem(device_json, "brightness");
  if (!cJSON_IsNumber(brightness_json)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    display.set_brightness(brightness_json->valueint);
  }

  cJSON_Delete(device_json);
}

const char *const Device::TAG = "device";

void Device::set_id(const std::string &id) {
  this->id = id;
  non_volatile_storage.write(TAG, "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

esp_err_t Device::enroll() {
  ESP_LOGI(TAG, "Enrolling.");
  ApiRequest<Device> post_device_enroll = ApiRequest<Device>(
      session, *this, HTTP_METHOD_POST, 60000, "/device/enroll", "");
  esp_err_t err = post_device_enroll.send_request();
  ESP_LOGI(TAG, "Enroll successful.");
  return err;
}
