#include "Alarm.h"
#include "NonVolatileStorage.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string>

const char *const Alarm::TAG = "Alarm";

Alarm::Alarm(NonVolatileStorage &non_volatile_storage, const std::string &id)
    : non_volatile_storage(non_volatile_storage), id(id), name(), schedule(),
      time_to_abort() {
  esp_err_t err = non_volatile_storage.read(id, "name", name);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Name read from NVS: %s", name.c_str());
    set_name(name);
  } else {
    ESP_LOGW(TAG, "Error reading name from NVS: %s", esp_err_to_name(err));
  }

  err = non_volatile_storage.read(id, "schedule", schedule);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Schedule read from NVS: %s", schedule.c_str());
    set_schedule(schedule);
  } else {
    ESP_LOGW(TAG, "Error reading schedule from NVS: %s", esp_err_to_name(err));
  }

  err = non_volatile_storage.read(id, "time_to_abort", time_to_abort);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Time to abort read from NVS: %d", time_to_abort);
    set_time_to_abort(time_to_abort);
  } else {
    ESP_LOGW(TAG, "Error reading time to abort from NVS: %s",
             esp_err_to_name(err));
  }
};

Alarm::Alarm(NonVolatileStorage &non_volatile_storage,
             const cJSON *const alarm_json)
    : non_volatile_storage(non_volatile_storage), id(), name(), schedule(),
      time_to_abort() {
  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `id` from JSON response");
  } else {
    set_id(id_json->valuestring);
  }

  const cJSON *const name_json = cJSON_GetObjectItem(alarm_json, "name");
  if (!cJSON_IsString(name_json) || (name_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `name` from JSON response");
  } else {
    set_name(name_json->valuestring);
  }

  const cJSON *const schedule_json =
      cJSON_GetObjectItem(alarm_json, "schedule");
  if (!cJSON_IsString(schedule_json) || (schedule_json->valuestring == NULL)) {
    ESP_LOGE(TAG, "Failed to extract `schedule` from JSON response");
  } else {
    set_schedule(schedule_json->valuestring);
  }

  const cJSON *const time_to_abort_json =
      cJSON_GetObjectItem(alarm_json, "time_to_abort");
  if (!cJSON_IsNumber(time_to_abort_json)) {
    ESP_LOGE(TAG, "Failed to extract `time_to_abort` from JSON response");
  } else {
    set_time_to_abort(time_to_abort_json->valueint);
  }
}

Alarm::~Alarm() { ESP_LOGI(TAG, "Destroy."); }

void Alarm::set_id(const std::string &id) {
  this->id = id;
  ESP_LOGI(TAG, "Set id: %s", id.c_str());
}

void Alarm::set_name(const std::string &name) {
  this->name = name;
  non_volatile_storage.write(this->id, "name", name);
  ESP_LOGI(TAG, "Set name: %s", name.c_str());
}

void Alarm::set_schedule(const std::string &schedule) {
  this->schedule = schedule;
  non_volatile_storage.write(this->id, "schedule", schedule);
  ESP_LOGI(TAG, "Set schedule: %s", schedule.c_str());
}

void Alarm::set_time_to_abort(const int time_to_abort) {
  this->time_to_abort = time_to_abort;
  non_volatile_storage.write(this->id, "time_to_abort", time_to_abort);
  ESP_LOGI(TAG, "Set time to abort: %d", time_to_abort);
}
