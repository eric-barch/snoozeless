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
      buzzer(buzzer), id(), state(DISPLAY_TIME), run_handle() {
  esp_err_t err = non_volatile_storage.read(TAG, "id", id);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "ID read from NVS: %s", id.c_str());
    set_id(id);
  } else {
    ESP_LOGW(TAG, "Error reading ID from NVS: %s.", esp_err_to_name(err));
    enroll();
  }

  xTaskCreate(Device::run, "run", 4096, this, 5, &run_handle);
}

Device::~Device() { ESP_LOGI(TAG, "Destroy."); }

void Device::set_state(const DeviceState &state) {
  this->state = state;
  ESP_LOGI(TAG, "Set state: %s", states.at(state).c_str());
}

std::string Device::get_id() { return id; }

DeviceState Device::get_state() { return state; }

TaskHandle_t Device::get_run_handle() { return run_handle; }

void Device::on_data(const std::string &response) { parse(response); }

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

void Device::display_current_time() { display.print_current_time(); }

void Device::start_alarm() { buzzer.start_alarm(); }

void Device::stop_alarm() { buzzer.stop_alarm(); }

void Device::display_countdown() {
  ESP_LOGW(TAG, "Implement `display_countdown`.");
}

const char *const Device::TAG = "device";

const std::map<const DeviceState, const std::string> states = {
    {DISPLAY_TIME, "DISPLAY_TIME"},
    {ALARM_1, "ALARM_1"},
    {COUNTDOWN, "COUNTDOWN"},
    {ALARM_2, "ALARM_2"},
};

const std::map<const DeviceEvent, const std::string> events = {
    {START_ALARM, "START_ALARM"},
    {PRESS_BUTTON, "PRESS_BUTTON"},
    {END_COUNTDOWN, "END_COUNTDOWN"},
    {ABORT, "ABORT"},
};

void Device::set_id(const std::string &id) {
  this->id = id;
  non_volatile_storage.write(TAG, "id", id);
  ESP_LOGI(TAG, "Set ID: %s", id.c_str());
}

void Device::run(void *const pvParameters) {
  Device *self = static_cast<Device *>(pvParameters);

  self->display_current_time();

  DeviceState state;
  DeviceEvent event;

  while (true) {
    xTaskNotifyWait(0, 0, (uint32_t *)&event, portMAX_DELAY);
    state = self->get_state();

    switch (state) {
    case DISPLAY_TIME:
      if (event == START_ALARM) {
        self->start_alarm();
      }
      break;
    case ALARM_1:
      if (event == PRESS_BUTTON) {
        self->stop_alarm();
        self->display_countdown();
      }
      break;
    case COUNTDOWN:
      if (event == END_COUNTDOWN) {
        self->start_alarm();
      } else if (event == ABORT) {
        self->display_current_time();
      }
      break;
    case ALARM_2:
      if (event == ABORT) {
        self->stop_alarm();
        self->display_current_time();
      }
      break;
    }
  }

  vTaskDelete(NULL);
}

esp_err_t Device::enroll() {
  ESP_LOGI(TAG, "Enrolling.");
  ApiRequest<Device> post_device_enroll = ApiRequest<Device>(
      session, *this, HTTP_METHOD_POST, 60000, "/device/enroll", "");
  esp_err_t err = post_device_enroll.send_request();
  ESP_LOGI(TAG, "Enroll successful.");
  return err;
}
