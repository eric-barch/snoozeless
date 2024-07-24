#include "Alarms.h"
#include "Alarm.h"
#include "NvsManager.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_log.h>

static const char *TAG = "Alarms";

Alarms::Alarms(NvsManager &nvs_manager) : nvs_manager(nvs_manager) {
  this->alarms = std::map<std::string, Alarm *>();

  std::string alarm_ids_string;
  esp_err_t err =
      this->nvs_manager.read_string("alarms", "ids", alarm_ids_string);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Alarm IDs read from NVS: %s", alarm_ids_string.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading alarm IDs from NVS: %s", esp_err_to_name(err));
    return;
  }

  cJSON *alarm_ids_json = cJSON_Parse(alarm_ids_string.c_str());
  if (alarm_ids_json == nullptr) {
    ESP_LOGE(TAG, "Error parsing JSON alarm IDs.");
    cJSON_Delete(alarm_ids_json);
    return;
  }

  if (!cJSON_IsArray(alarm_ids_json)) {
    ESP_LOGE(TAG, "Alarm IDs is not an array.");
    cJSON_Delete(alarm_ids_json);
    return;
  }

  cJSON *alarm_id_json = nullptr;
  cJSON_ArrayForEach(alarm_id_json, alarm_ids_json) {
    if (cJSON_IsString(alarm_id_json)) {
      std::string alarm_id = alarm_id_json->valuestring;
      Alarm *alarm = new Alarm(this->nvs_manager, alarm_id);
      this->alarms.emplace(alarm_id, alarm);
    } else {
      ESP_LOGE(TAG, "Alarm ID is not a string.");
    }
  }

  cJSON_Delete(alarm_id_json);
}

void Alarms::set_alarms(const std::vector<Alarm *> alarms) {
  ESP_LOGW(TAG, "Implement set_alarms.");
}
