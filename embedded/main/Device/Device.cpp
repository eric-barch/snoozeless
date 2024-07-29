#include "Device.h"
#include "Alarm.h"
#include "ApiRequest.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <map>
#include <string>
#include <vector>

const char *const Device::TAG = "Device";

const std::map<const std::string, const DeviceEvent> Device::events = {
    {"initial-device", INITIAL_DEVICE}, {"device-update", DEVICE_UPDATE},
    {"initial-alarms", INITIAL_ALARMS}, {"alarm-insert", ALARM_INSERT},
    {"alarm-update", ALARM_UPDATE},     {"alarm-delete", ALARM_DELETE},
};

Device::Device(NonVolatileStorage &non_volatile_storage, Session &session,
               CurrentTime &current_time, Alarms &alarms, Display &display,
               Buzzer &buzzer)
    : non_volatile_storage(non_volatile_storage), session(session),
      current_time(current_time), alarms(alarms), display(display),
      buzzer(buzzer), id(), is_subscribed(xSemaphoreCreateBinary()) {
  xSemaphoreGive(is_subscribed);

  esp_err_t err = non_volatile_storage.read("device", "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    enroll();
  }

  xSemaphoreTake(is_subscribed, 0);
  xTaskCreate(Device::keep_subscribed, "keep_subscribed", 2048, this, 5, NULL);
  xSemaphoreTake(is_subscribed, portMAX_DELAY);

  display.print_current_time();
}

Device::~Device() {}

void Device::on_data(const std::string &response) {
  /**Trim trailing newlines from SSE response. */
  const std::string trimmed_response =
      response.substr(0, response.length() - 2);
  ESP_LOGI(TAG, "%s", trimmed_response.c_str());

  std::string event;
  esp_err_t err = extract_response_field(trimmed_response, "event: ", event);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Did not find valid event field.");
    return;
  }

  if (event == "keep-alive") {
    return;
  }

  std::string data;
  err = extract_response_field(trimmed_response, "data: ", data);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Did not find valid data field.");
    return;
  }

  DeviceEvent device_event = events.at(event);

  switch (device_event) {
  case INITIAL_DEVICE:
    parse(data);
    break;
  case DEVICE_UPDATE:
    parse(data);
    break;
  case INITIAL_ALARMS:
    alarms.parse_initial(data);
    break;
  case ALARM_INSERT:
    alarms.parse_insert(data);
    break;
  case ALARM_UPDATE:
    alarms.parse_update(data);
    break;
  case ALARM_DELETE:
    alarms.parse_remove(data);
    break;
  default:
    ESP_LOGE(TAG, "Unknown device state event: %s", data.c_str());
    break;
  }
}

void Device::set_id(std::string &id) {
  this->id = id;
  non_volatile_storage.write("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

esp_err_t Device::extract_response_field(const std::string &response,
                                         const std::string &field,
                                         std::string &out_value) {
  const size_t pos = response.find(field);
  if (pos == std::string::npos) {
    return ESP_FAIL;
  }

  const size_t start_pos = pos + field.length();
  const size_t end_pos = response.find('\n', start_pos);

  out_value = response.substr(start_pos, end_pos - start_pos);
  return ESP_OK;
}

void Device::parse(const std::string &data) {
  cJSON *const data_json = cJSON_Parse(data.c_str());
  if (data_json == nullptr) {
    ESP_LOGE(TAG, "Failed to parse JSON data.");
    cJSON_Delete(data_json);
    return;
  }

  const cJSON *const time_zone_json =
      cJSON_GetObjectItem(data_json, "time_zone");
  if (!cJSON_IsString(time_zone_json) ||
      (time_zone_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    current_time.set_time_zone(time_zone_json->valuestring);
  }

  const cJSON *const time_format_json =
      cJSON_GetObjectItem(data_json, "time_format");
  if (!cJSON_IsString(time_format_json) ||
      (time_format_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    current_time.set_format(time_format_json->valuestring);
  }

  const cJSON *const brightness_json =
      cJSON_GetObjectItem(data_json, "brightness");
  if (!cJSON_IsNumber(brightness_json)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    display.set_brightness(brightness_json->valueint);
  }

  cJSON_Delete(data_json);
}

esp_err_t Device::enroll() {
  ApiRequest post_device_enroll = ApiRequest<Device>(
      session, *this, HTTP_METHOD_POST, 60000, "/device/enroll", "");
  esp_err_t err = post_device_enroll.send();
  ESP_LOGI(TAG, "Enroll successful.");
  return err;
}

void Device::subscribe() {
  const std::string query = "deviceId=" + id;
  ApiRequest get_device_state = ApiRequest<Device>(
      session, *this, HTTP_METHOD_GET, 300000, "/device/state", query);
  get_device_state.send();
  ESP_LOGI(TAG, "Subscription successful.");
  xSemaphoreGive(is_subscribed);
}

void Device::keep_subscribed(void *pvParameters) {
  Device *self = static_cast<Device *>(pvParameters);

  ESP_LOGI(TAG, "Subscribing.");

  while (true) {
    self->subscribe();
    ESP_LOGI(TAG, "Subscription failed. Will try to reconnect in one minute.");
    vTaskDelay(pdMS_TO_TICKS(60000));
  }

  vTaskDelete(NULL);
}
