#include "Alarms.h"
#include "Alarm.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_log.h>
#include <memory>

static const char *TAG = "Alarms";

Alarms::Alarms(NonVolatileStorage &non_volatile_storage)
    : non_volatile_storage(non_volatile_storage) {
  alarms = std::map<std::string, std::unique_ptr<Alarm>>();

  std::string alarm_ids_string;
  esp_err_t err =
      this->non_volatile_storage.read_key("alarms", "ids", alarm_ids_string);
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
      Alarm *alarm = new Alarm(this->non_volatile_storage, alarm_id);
      this->alarms.emplace(alarm_id, alarm);
    } else {
      ESP_LOGE(TAG, "Alarm ID is not a string.");
    }
  }

  cJSON_Delete(alarm_id_json);
}

void Alarms::parse_initial_alarms(const std::string &data) {
  cJSON *alarms_json = cJSON_Parse(data.c_str());
  if (!alarms_json) {
    ESP_LOGE(TAG, "Error parsing JSON alarms.");
    return;
  }

  if (!cJSON_IsArray(alarms_json)) {
    ESP_LOGE(TAG, "Alarms is not an array.");
    cJSON_Delete(alarms_json);
    return;
  }

  std::map<std::string, std::unique_ptr<Alarm>> alarms;
  const cJSON *alarm_json = nullptr;
  cJSON_ArrayForEach(alarm_json, alarms_json) {
    const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
    if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
      ESP_LOGE(TAG, "Alarm does not have a valid id.");
      continue;
    }

    std::string id = id_json->valuestring;
    auto new_alarm =
        std::make_unique<Alarm>(this->non_volatile_storage, alarm_json);
    alarms[id] = std::move(new_alarm);
  }

  this->alarms = std::move(alarms);

  cJSON *alarm_ids_array = cJSON_CreateArray();
  for (const auto &pair : this->alarms) {
    cJSON_AddItemToArray(alarm_ids_array,
                         cJSON_CreateString(pair.first.c_str()));
  }

  char *alarm_ids_string = cJSON_PrintUnformatted(alarm_ids_array);

  if (alarm_ids_string) {
    esp_err_t err =
        this->non_volatile_storage.write_key("alarms", "ids", alarm_ids_string);
    if (err == ESP_OK) {
      ESP_LOGI(TAG, "Alarm IDs written to NVS: %s", alarm_ids_string);
    } else {
      ESP_LOGE(TAG, "Error writing alarm IDs to NVS: %s", esp_err_to_name(err));
    }
    free(alarm_ids_string);
  }

  cJSON_Delete(alarms_json);
  cJSON_Delete(alarm_ids_array);
}

void Alarms::parse_alarm_insert(const std::string &data) {
  cJSON *const alarm_json = cJSON_Parse(data.c_str());
  if (!alarm_json) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    return;
  }

  if (!cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "Alarm is not an object.");
    cJSON_Delete(alarm_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
    ESP_LOGE(TAG, "Alarm does not have a valid id.");
    cJSON_Delete(alarm_json);
    return;
  }

  std::string id = id_json->valuestring;
  auto new_alarm =
      std::make_unique<Alarm>(this->non_volatile_storage, alarm_json);
  alarms[id] = std::move(new_alarm);

  cJSON *alarm_ids_array = cJSON_CreateArray();
  for (const auto &pair : this->alarms) {
    cJSON_AddItemToArray(alarm_ids_array,
                         cJSON_CreateString(pair.first.c_str()));
  }

  char *alarm_ids_string = cJSON_PrintUnformatted(alarm_ids_array);

  if (alarm_ids_string) {
    esp_err_t err =
        this->non_volatile_storage.write_key("alarms", "ids", alarm_ids_string);
    if (err == ESP_OK) {
      ESP_LOGI(TAG, "Alarm IDs written to NVS: %s", alarm_ids_string);
    } else {
      ESP_LOGE(TAG, "Error writing alarm IDs to NVS: %s", esp_err_to_name(err));
    }
    free(alarm_ids_string);
  }

  cJSON_Delete(alarm_json);
  cJSON_Delete(alarm_ids_array);
};

void Alarms::parse_alarm_update(const std::string &data) {
  cJSON *const alarm_json = cJSON_Parse(data.c_str());
  if (!alarm_json) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    return;
  }

  if (!cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "Alarm is not an object.");
    cJSON_Delete(alarm_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
    ESP_LOGE(TAG, "Alarm does not have a valid id.");
    cJSON_Delete(alarm_json);
    return;
  }

  std::string id = id_json->valuestring;
  auto new_alarm =
      std::make_unique<Alarm>(this->non_volatile_storage, alarm_json);
  alarms[id] = std::move(new_alarm);

  cJSON_Delete(alarm_json);
};

void Alarms::parse_alarm_remove(const std::string &data) {
  cJSON *const alarm_json = cJSON_Parse(data.c_str());
  if (!alarm_json) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    return;
  }

  if (!cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "Alarm is not an object.");
    cJSON_Delete(alarm_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
    ESP_LOGE(TAG, "Alarm does not have a valid id.");
    cJSON_Delete(alarm_json);
    return;
  }

  std::string id = id_json->valuestring;
  auto it = alarms.find(id);
  if (it != alarms.end()) {
    alarms.erase(it);
    ESP_LOGI(TAG, "Alarm with id: %s removed.", id.c_str());
  } else {
    ESP_LOGW(TAG, "Alarm with id: %s not found.", id.c_str());
  }

  non_volatile_storage.erase_key(id, "name");
  non_volatile_storage.erase_key(id, "schedule");
  non_volatile_storage.erase_key(id, "time_to_abort");

  cJSON_Delete(alarm_json);
};