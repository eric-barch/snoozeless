#include "Device.h"
#include "ApiRequest.h"
#include "NonVolatileStorage.h"
#include <cJSON.h>
#include <ctime>
#include <esp_err.h>
#include <esp_log.h>
#include <stdlib.h>
#include <string>

Device::Device(NonVolatileStorage &non_volatile_storage)
    : non_volatile_storage(non_volatile_storage), id(), brightness(),
      time_zone(), time_format() {
  esp_err_t err = non_volatile_storage.read(TAG, "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
  }

  err = non_volatile_storage.read(TAG, "brightness", brightness);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Brightness read from NVS: %d", brightness);
    set_brightness(brightness);
  } else {
    ESP_LOGW(TAG, "Error reading brightness from NVS: %s. Using config.",
             esp_err_to_name(err));
    set_brightness(CONFIG_DEFAULT_BRIGHTNESS);
  }

  err = non_volatile_storage.read(TAG, "time_zone", time_zone);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Time zone read from NVS: %s", time_zone.c_str());
    set_time_zone(time_zone);
  } else {
    ESP_LOGW(TAG, "Error reading time zone from NVS: %s. Using config.",
             esp_err_to_name(err));
    set_time_zone(CONFIG_DEFAULT_TIME_ZONE);
  }

  err = non_volatile_storage.read(TAG, "time_format", time_format);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Time format read from NVS: %s", time_format.c_str());
    set_time_format(time_format);
  } else {
    ESP_LOGW(TAG, "Error reading time format from NVS: %s. Using config.",
             esp_err_to_name(err));
    set_time_zone(CONFIG_DEFAULT_TIME_FORMAT);
  }
}

Device::~Device() { ESP_LOGI(TAG, "Destroy."); }

std::string Device::get_id() { return id; }

int Device::get_brightness() { return brightness; }

std::string Device::get_time_zone() { return time_zone; }

std::string Device::get_time_format() { return time_format; }

void Device::parse(const std::string &data) {
  cJSON *const device_json = cJSON_Parse(data.c_str());
  if (device_json == nullptr) {
    ESP_LOGE(TAG, "Failed to parse JSON device.");
    cJSON_Delete(device_json);
    return;
  }

  const cJSON *const time_zone_json =
      cJSON_GetObjectItem(device_json, "time_zone");
  if (!cJSON_IsString(time_zone_json) ||
      (time_zone_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    set_time_zone(time_zone_json->valuestring);
  }

  const cJSON *const time_format_json =
      cJSON_GetObjectItem(device_json, "time_format");
  if (!cJSON_IsString(time_format_json) ||
      (time_format_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    set_time_format(time_format_json->valuestring);
  }

  const cJSON *const brightness_json =
      cJSON_GetObjectItem(device_json, "brightness");
  if (!cJSON_IsNumber(brightness_json)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    set_brightness(brightness_json->valueint);
  }

  cJSON_Delete(device_json);
}

const char *const Device::TAG = "device";

void Device::set_id(const std::string &id) {
  this->id = id;
  non_volatile_storage.write(TAG, "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::set_brightness(const int &brightness) {
  this->brightness = brightness;
  non_volatile_storage.write(TAG, "brightness", brightness);
  ESP_LOGI(TAG, "Set brightness: %d", brightness);
}

void Device::set_time_zone(const std::string &time_zone) {
  this->time_zone = time_zone;
  non_volatile_storage.write(TAG, "time_zone", time_zone);
  setenv("TZ", time_zone.c_str(), true);
  tzset();
  ESP_LOGI(TAG, "Set time zone: %s", time_zone.c_str());
}

void Device::set_time_format(const std::string &time_format) {
  this->time_format = time_format;
  non_volatile_storage.write(TAG, "time_format", time_format);
  ESP_LOGI(TAG, "Set time format: %s", time_format.c_str());
}
