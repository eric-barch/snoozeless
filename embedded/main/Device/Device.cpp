#include "Device.h"
#include "Alarm.h"
#include "ApiRequest.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "NvsManager.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <map>
#include <string>
#include <vector>

static const char *TAG = "Device";

std::map<std::string, DeviceStateEvent> deviceStateEventMap = {
    {"initial-device", INITIAL_DEVICE}, {"device-update", DEVICE_UPDATE},
    {"initial-alarms", INITIAL_ALARMS}, {"alarm-insert", ALARM_INSERT},
    {"alarm-delete", ALARM_DELETE},     {"alarm-update", ALARM_UPDATE},
};

Device::Device(NvsManager &nvs_manager, Session &session,
               CurrentTime &current_time, Display &display, Buzzer &buzzer)
    : nvs_manager(nvs_manager), session(session), current_time(current_time),
      display(display), buzzer(buzzer) {
  std::string id;
  esp_err_t err = this->nvs_manager.read_string("device", "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    this->set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    this->enroll();
  }

  this->is_subscribed = xSemaphoreCreateBinary();
  xSemaphoreGive(this->is_subscribed);

  this->initialize_alarms();

  this->keep_subscribed();
  this->display.print_current_time();
}

Device::~Device() {}

void Device::set_id(std::string id) {
  this->id = id;
  this->nvs_manager.write_string("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::initialize_alarms() {
  this->alarms = {};

  std::string alarm_ids_string;
  esp_err_t err =
      this->nvs_manager.read_string("alarms", "ids", alarm_ids_string);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error reading alarm IDs from NVS: %s.",
             esp_err_to_name(err));
    return;
  }

  cJSON *alarm_ids_json = cJSON_Parse(alarm_ids_string.c_str());
  if (alarm_ids_json == nullptr) {
    ESP_LOGE(TAG, "Error parsing JSON alarm IDs.");
    return;
  }

  if (!cJSON_IsArray(alarm_ids_json)) {
    ESP_LOGE(TAG, "JSON alarm IDs is not an array.");
    cJSON_Delete(alarm_ids_json);
    return;
  }

  cJSON *alarm_id_json = nullptr;
  cJSON_ArrayForEach(alarm_id_json, alarm_ids_json) {
    if (cJSON_IsString(alarm_id_json)) {
      std::string id = cJSON_GetStringValue(alarm_id_json);
      Alarm *alarm = new Alarm(this->nvs_manager, id);
      this->alarms.push_back(alarm);
    } else {
      ESP_LOGE(TAG, "Invalid alarm ID format.");
    }
  }

  cJSON_Delete(alarm_ids_json);
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

esp_err_t Device::enroll() {
  ApiRequest post_device_enroll =
      ApiRequest(this->session, this, enroll_on_data, HTTP_METHOD_POST, 60000,
                 "/device/enroll");
  esp_err_t err = post_device_enroll.send();
  return err;
}

void Device::parse_device(void *device, const std::string &data) {
  Device *self = static_cast<Device *>(device);

  cJSON *json_data = cJSON_Parse(data.c_str());
  if (!json_data) {
    ESP_LOGE(TAG, "Failed to parse JSON data.");
    return;
  }

  cJSON *time_zone_item = cJSON_GetObjectItem(json_data, "time_zone");
  if (!cJSON_IsString(time_zone_item) ||
      (time_zone_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    self->current_time.set_time_zone(time_zone_item->valuestring);
  }

  cJSON *time_format_item = cJSON_GetObjectItem(json_data, "time_format");
  if (!cJSON_IsString(time_format_item) ||
      (time_format_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    self->current_time.set_format(time_format_item->valuestring);
  }

  cJSON *brightness_item = cJSON_GetObjectItem(json_data, "brightness");
  if (!cJSON_IsNumber(brightness_item)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    self->display.set_brightness(brightness_item->valueint);
  }

  cJSON_Delete(json_data);
}

void Device::parse_initial_alarms(void *device, const std::string &data) {
  ESP_LOGW(TAG, "Implement parse_initial_alarms.");
}

void Device::parse_alarm_insert(void *device, const std::string &data) {
  ESP_LOGW(TAG, "Implement parse_alarm_insert.");
}

void Device::parse_alarm_delete(void *device, const std::string &data) {
  ESP_LOGW(TAG, "Implement parse_alarm_delete.");
}

void Device::parse_alarm_update(void *device, const std::string &data) {
  ESP_LOGW(TAG, "Implement parse_alarm_update.");
}

void Device::subscribe_on_data(void *device, const std::string &response) {
  std::string response_without_line_breaks =
      response.substr(0, response.length() - 2);
  ESP_LOGI(TAG, "%s", response_without_line_breaks.c_str());

  std::string event_label = "event: ";
  std::string data_label = "data: ";

  size_t event_pos = response.find(event_label);
  if (event_pos == std::string::npos) {
    ESP_LOGE(TAG, "No valid event field found in Server Sent Event.");
    return;
  }

  size_t event_start_pos = event_pos + event_label.length();
  size_t event_end_pos = response.find('\n', event_start_pos);
  std::string sse_event =
      response.substr(event_start_pos, event_end_pos - event_start_pos);

  if (sse_event == "keep-alive") {
    return;
  }

  size_t data_pos = response.find(data_label);
  if (data_pos == std::string::npos) {
    ESP_LOGE(TAG, "No valid data field found in Server Sent Event.");
    return;
  }

  size_t data_start_pos = data_pos + data_label.length();
  size_t data_end_pos = response.find('\n', data_start_pos);
  std::string data =
      response.substr(data_start_pos, data_end_pos - data_start_pos);

  Device *self = static_cast<Device *>(device);

  DeviceStateEvent event = deviceStateEventMap.count(sse_event)
                               ? deviceStateEventMap[sse_event]
                               : UNKNOWN_EVENT;

  switch (event) {
  case INITIAL_DEVICE:
    self->parse_device(self, data);
    break;
  case DEVICE_UPDATE:
    self->parse_device(self, data);
    break;
  case INITIAL_ALARMS:
    self->parse_initial_alarms(self, data);
    break;
  case ALARM_INSERT:
    self->parse_alarm_insert(self, data);
    break;
  case ALARM_DELETE:
    self->parse_alarm_delete(self, data);
    break;
  case ALARM_UPDATE:
    self->parse_alarm_update(self, data);
    break;
  default:
    ESP_LOGE(TAG, "Unknown device state event: %s", data.c_str());
    break;
  }
}

void Device::subscribe() {
  std::string query = "deviceId=" + this->id;
  ApiRequest get_device_state =
      ApiRequest(this->session, this, subscribe_on_data, HTTP_METHOD_GET,
                 300000, "/device/state", query);
  get_device_state.send();
  ESP_LOGI(TAG, "Subscription successful.");
  xSemaphoreGive(this->is_subscribed);
}

void Device::subscribe_task(void *pvParameters) {
  Device *self = static_cast<Device *>(pvParameters);

  ESP_LOGI(TAG, "Subscribing.");

  while (true) {
    self->subscribe();
    ESP_LOGI(TAG, "Subscription failed. Will try to reconnect in one minute.");
    vTaskDelay(pdMS_TO_TICKS(60000));
  }

  vTaskDelete(NULL);
}

void Device::keep_subscribed() {
  xSemaphoreTake(this->is_subscribed, 0);
  xTaskCreate(Device::subscribe_task, "keep_subscribed", 2048, this, 5, NULL);
  xSemaphoreTake(this->is_subscribed, portMAX_DELAY);
}
