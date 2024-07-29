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

static const char *TAG = "Device";

std::map<std::string, DeviceStateEvent> deviceStateEventMap = {
    {"initial-device", INITIAL_DEVICE}, {"device-update", DEVICE_UPDATE},
    {"initial-alarms", INITIAL_ALARMS}, {"alarm-insert", ALARM_INSERT},
    {"alarm-update", ALARM_UPDATE},     {"alarm-delete", ALARM_DELETE},
};

Device::Device(NonVolatileStorage &non_volatile_storage, Session &session,
               CurrentTime &current_time, Alarms &alarms, Display &display,
               Buzzer &buzzer)
    : non_volatile_storage(non_volatile_storage), session(session),
      current_time(current_time), alarms(alarms), display(display),
      buzzer(buzzer) {
  esp_err_t err = non_volatile_storage.read_key("device", "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    enroll();
  }

  is_subscribed = xSemaphoreCreateBinary();
  xSemaphoreGive(is_subscribed);

  keep_subscribed();
  display.print_current_time();
}

void Device::set_id(std::string &id) {
  this->id = id;
  non_volatile_storage.write_key("device", "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

esp_err_t Device::enroll() {
  ApiRequest post_device_enroll = ApiRequest<Device>(
      session, *this, HTTP_METHOD_POST, 60000, "/device/enroll", "");
  esp_err_t err = post_device_enroll.send();
  return err;
}

void Device::parse_device_state(const std::string &data) {
  cJSON *data_json = cJSON_Parse(data.c_str());
  if (!data_json) {
    ESP_LOGE(TAG, "Failed to parse JSON data.");
    return;
  }

  cJSON *time_zone_item = cJSON_GetObjectItem(data_json, "time_zone");
  if (!cJSON_IsString(time_zone_item) ||
      (time_zone_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_zone` from JSON response.");
  } else {
    current_time.set_time_zone(time_zone_item->valuestring);
  }

  cJSON *time_format_item = cJSON_GetObjectItem(data_json, "time_format");
  if (!cJSON_IsString(time_format_item) ||
      (time_format_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `time_format` from JSON response.");
  } else {
    current_time.set_format(time_format_item->valuestring);
  }

  cJSON *brightness_item = cJSON_GetObjectItem(data_json, "brightness");
  if (!cJSON_IsNumber(brightness_item)) {
    ESP_LOGE(TAG, "Failed to extract `brightness` from JSON response.");
  } else {
    display.set_brightness(brightness_item->valueint);
  }

  cJSON_Delete(data_json);
}

void Device::on_data(const std::string &response) {
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

  DeviceStateEvent event = deviceStateEventMap.count(sse_event)
                               ? deviceStateEventMap[sse_event]
                               : UNKNOWN_EVENT;

  switch (event) {
  case INITIAL_DEVICE:
    parse_device_state(data);
    break;
  case DEVICE_UPDATE:
    parse_device_state(data);
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

void Device::subscribe() {
  std::string query = "deviceId=" + id;
  ApiRequest get_device_state = ApiRequest<Device>(
      session, *this, HTTP_METHOD_GET, 300000, "/device/state", query);
  get_device_state.send();
  ESP_LOGI(TAG, "Subscription successful.");
  xSemaphoreGive(is_subscribed);
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
  xSemaphoreTake(is_subscribed, 0);
  xTaskCreate(Device::subscribe_task, "keep_subscribed", 2048, this, 5, NULL);
  xSemaphoreTake(is_subscribed, portMAX_DELAY);
}
