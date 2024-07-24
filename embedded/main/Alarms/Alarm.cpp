#include "Alarm.h"
#include "NvsManager.h"
#include <cJSON.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string>

static const char *TAG = "Alarm";

Alarm::Alarm(NvsManager &nvs_manager, std::string id)
    : nvs_manager(nvs_manager), id(id) {
  std::string name;
  esp_err_t err = this->nvs_manager.read_string(this->id, "name", name);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Name read from NVS: %s", name.c_str());
    this->set_name(name);
  } else {
    ESP_LOGW(TAG, "Error reading name from NVS: %s", esp_err_to_name(err));
  }

  std::string schedule;
  err = this->nvs_manager.read_string(this->id, "schedule", schedule);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Schedule read from NVS: %s", schedule.c_str());
    this->set_schedule(schedule);
  } else {
    ESP_LOGW(TAG, "Error reading schedule from NVS: %s", esp_err_to_name(err));
  }

  int time_to_abort;
  err = this->nvs_manager.read_int(this->id, "time_to_abort", time_to_abort);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Time to abort read from NVS: %d", time_to_abort);
    this->set_time_to_abort(time_to_abort);
  } else {
    ESP_LOGW(TAG, "Error reading time to abort from NVS: %s",
             esp_err_to_name(err));
  }
};

void Alarm::set_name(std::string name) {
  this->name = name;
  this->nvs_manager.write_string(this->id, "name", name);
  ESP_LOGI(TAG, "Set name: %s", name.c_str());
}

void Alarm::set_schedule(std::string schedule) {
  this->schedule = schedule;
  this->nvs_manager.write_string(this->id, "schedule", schedule);
  ESP_LOGI(TAG, "Set schedule: %s", schedule.c_str());
}

void Alarm::set_time_to_abort(int time_to_abort) {
  this->time_to_abort = time_to_abort;
  this->nvs_manager.write_int(this->id, "time_to_abort", time_to_abort);
  ESP_LOGI(TAG, "Set time to abort: %d", time_to_abort);
}
