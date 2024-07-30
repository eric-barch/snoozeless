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

Device::Device(NonVolatileStorage &non_volatile_storage, Session &session,
               CurrentTime &current_time, Alarms &alarms, Display &display,
               Buzzer &buzzer)
    : non_volatile_storage(non_volatile_storage), session(session),
      current_time(current_time), alarms(alarms), display(display),
      buzzer(buzzer), id(), is_subscribed(xSemaphoreCreateBinary()) {
  xSemaphoreGive(is_subscribed);

  esp_err_t err = non_volatile_storage.read(TAG, "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    enroll();
  }

  xSemaphoreTake(is_subscribed, 0);
  xTaskCreate(Device::handle_subscribe, "keep_subscribed", 8192, this, 5, NULL);
  xSemaphoreTake(is_subscribed, portMAX_DELAY);

  display.print_current_time();
}

Device::~Device() { ESP_LOGI(TAG, "Destroy."); }

void Device::on_data(const std::string &response) {
  if (response.find("event:") != std::string::npos) {
    parse_sse(response);
  } else {
    parse(response);
  }
}

const char *const Device::TAG = "device";

const std::map<const std::string, const DeviceEvent> Device::events = {
    {"initial-device", INITIAL_DEVICE}, {"device-update", DEVICE_UPDATE},
    {"initial-alarms", INITIAL_ALARMS}, {"alarm-insert", ALARM_INSERT},
    {"alarm-update", ALARM_UPDATE},     {"alarm-delete", ALARM_DELETE},
};

void Device::set_id(const std::string &id) {
  this->id = id;
  non_volatile_storage.write(TAG, "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::handle_subscribe(void *const pvParameters) {
  Device *self = static_cast<Device *>(pvParameters);

  while (true) {
    self->subscribe();
    ESP_LOGI(TAG, "Subscription failed. Will try to reconnect in one minute.");
    vTaskDelay(pdMS_TO_TICKS(60000));
  }

  vTaskDelete(NULL);
}

esp_err_t Device::enroll() {
  ESP_LOGI(TAG, "Enrolling.");
  ApiRequest post_device_enroll = ApiRequest<Device>(
      session, *this, HTTP_METHOD_POST, 60000, "/device/enroll", "");
  esp_err_t err = post_device_enroll.send_request();
  ESP_LOGI(TAG, "Enroll successful.");
  return err;
}

void Device::subscribe() {
  ESP_LOGI(TAG, "Subscribing.");
  const std::string query = "deviceId=" + id;
  ApiRequest get_device_state = ApiRequest<Device>(
      session, *this, HTTP_METHOD_GET, 300000, "/device/state", query);
  get_device_state.send_request();
  ESP_LOGI(TAG, "Subscription successful.");
  xSemaphoreGive(is_subscribed);
}

void Device::parse(const std::string &device_string) {
  cJSON *const device_json = cJSON_Parse(device_string.c_str());
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

void Device::extract_sse_field(const std::string &response,
                               const std::string &field,
                               std::string &out_value) {
  const size_t pos = response.find(field);
  if (pos == std::string::npos) {
    ESP_LOGE(TAG, "Error finding response field %s", field.c_str());
    return;
  }

  const size_t start_pos = pos + field.length();
  const size_t end_pos = response.find('\n', start_pos);

  out_value = response.substr(start_pos, end_pos - start_pos);
}

void Device::parse_sse(const std::string &response) {
  /**Trim trailing newlines. */
  const std::string trimmed_response =
      response.substr(0, response.length() - 2);

  std::string event;
  extract_sse_field(trimmed_response, "event: ", event);
  ESP_LOGI(TAG, "event: %s", event.c_str());

  if (event == "keep-alive") {
    return;
  }

  std::string data;
  extract_sse_field(trimmed_response, "data: ", data);
  ESP_LOGI(TAG, "data: %s", data.c_str());

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
