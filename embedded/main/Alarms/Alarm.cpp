#include "Alarm.h"
#include "NvsManager.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string>

static const char *TAG = "Alarm";

Alarm::Alarm(NvsManager &nvs_manager, const std::string &id)
    : nvs_manager(nvs_manager), id(id), name(), schedule(), time_to_abort() {
  esp_err_t err = nvs_manager.read_string(id, "name", name);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Name read from NVS: %s", name.c_str());
    set_name(name);
  } else {
    ESP_LOGW(TAG, "Error reading name from NVS: %s", esp_err_to_name(err));
  }

  err = nvs_manager.read_string(id, "schedule", schedule);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Schedule read from NVS: %s", schedule.c_str());
    set_schedule(schedule);
  } else {
    ESP_LOGW(TAG, "Error reading schedule from NVS: %s", esp_err_to_name(err));
  }

  err = nvs_manager.read_int(id, "time_to_abort", time_to_abort);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Time to abort read from NVS: %d", time_to_abort);
    set_time_to_abort(time_to_abort);
  } else {
    ESP_LOGW(TAG, "Error reading time to abort from NVS: %s",
             esp_err_to_name(err));
  }
};

Alarm::Alarm(NvsManager &nvs_manager, const cJSON *const alarm_json)
    : nvs_manager(nvs_manager), id(), name(), schedule(), time_to_abort() {
  const cJSON *const id_item = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_item) || (id_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `id` from JSON response");
  } else {
    set_id(id_item->valuestring);
  }

  const cJSON *const name_item = cJSON_GetObjectItem(alarm_json, "name");
  if (!cJSON_IsString(name_item) || (name_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `name` from JSON response");
  } else {
    set_name(name_item->valuestring);
  }

  const cJSON *const schedule_item =
      cJSON_GetObjectItem(alarm_json, "schedule");
  if (!cJSON_IsString(schedule_item) || (schedule_item->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `schedule` from JSON response");
  } else {
    set_schedule(schedule_item->valuestring);
  }

  const cJSON *const time_to_abort_item =
      cJSON_GetObjectItem(alarm_json, "time_to_abort");
  if (!cJSON_IsNumber(time_to_abort_item)) {
    ESP_LOGE(TAG, "Failed to extract `time_to_abort` from JSON response");
  } else {
    set_time_to_abort(time_to_abort_item->valueint);
  }
}

void Alarm::set_id(const std::string &id) {
  this->id = id;
  ESP_LOGI(TAG, "Set id: %s", id.c_str());
}

void Alarm::set_name(const std::string &name) {
  this->name = name;
  this->nvs_manager.write_string(this->id, "name", name);
  ESP_LOGI(TAG, "Set name: %s", name.c_str());
}

void Alarm::set_schedule(const std::string &schedule) {
  this->schedule = schedule;
  this->nvs_manager.write_string(this->id, "schedule", schedule);
  ESP_LOGI(TAG, "Set schedule: %s", schedule.c_str());
}

void Alarm::set_time_to_abort(const int time_to_abort) {
  this->time_to_abort = time_to_abort;
  this->nvs_manager.write_int(this->id, "time_to_abort", time_to_abort);
  ESP_LOGI(TAG, "Set time to abort: %d", time_to_abort);
}
