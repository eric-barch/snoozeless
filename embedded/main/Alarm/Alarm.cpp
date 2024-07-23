#include "Alarm.h"
#include "NvsManager.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string>

static const char *TAG = "Alarm";

Alarm::Alarm(NvsManager &nvs_manager, std::string id)
    : nvs_manager(nvs_manager), id(id) {
  std::string alarm_string;
  esp_err_t err =
      this->nvs_manager.read_string("alarms", this->id, alarm_string);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error reading alarm from NVS: %s.", esp_err_to_name(err));
    return;
  }

  cJSON *alarm_json = cJSON_Parse(alarm_string.c_str());
  if (alarm_json == nullptr) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    return;
  }

  if (!cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "JSON alarm is not an object.");
    cJSON_Delete(alarm_json);
    return;
  }

  cJSON *name_json = cJSON_GetObjectItem(alarm_json, "name");
  if (cJSON_IsString(name_json)) {
    this->set_name(name_json->valuestring);
  } else {
    ESP_LOGE(TAG, "Name is not a string.");
  }

  cJSON *time_of_day_json = cJSON_GetObjectItem(alarm_json, "time_of_day");
  if (cJSON_IsNumber(time_of_day_json)) {
    this->set_time_of_day(time_of_day_json->valueint);
  } else {
    ESP_LOGE(TAG, "Time of day is not a number.");
  }

  cJSON *time_to_abort_json = cJSON_GetObjectItem(alarm_json, "time_to_abort");
  if (cJSON_IsNumber(time_to_abort_json)) {
    this->set_time_to_abort(time_to_abort_json->valueint);
  } else {
    ESP_LOGE(TAG, "Time to abort is not a number.");
  }

  cJSON *recurrence_json = cJSON_GetObjectItem(alarm_json, "recurrence");
  if (cJSON_IsString(recurrence_json)) {
    this->set_recurrence(recurrence_json->valuestring);
  } else {
    ESP_LOGE(TAG, "Recurrence is not a string.");
  }

  cJSON_Delete(alarm_json);
};

Alarm::~Alarm() {}

void Alarm::set_name(std::string name) {
  this->name = name;
  this->nvs_manager.write_string("alarms", this->id, name);
  ESP_LOGI(TAG, "Set name: %s", name.c_str());
}

void Alarm::set_time_of_day(int time_of_day) {
  this->time_of_day = time_of_day;
  this->nvs_manager.write_int("alarms", this->id, time_of_day);
  ESP_LOGI(TAG, "Set time of day: %d", time_of_day);
}

void Alarm::set_time_to_abort(int time_to_abort) {
  this->time_to_abort = time_to_abort;
  this->nvs_manager.write_int("alarms", this->id, time_to_abort);
  ESP_LOGI(TAG, "Set time to abort: %d", time_to_abort);
}

void Alarm::set_recurrence(std::string recurrence) {
  this->recurrence = recurrence;
  this->nvs_manager.write_string("alarms", this->id, recurrence);
  ESP_LOGI(TAG, "Set recurrence: %s", recurrence.c_str());
}
