#include "Alarms.h"
#include "Alarm.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include <cJSON.h>
#include <esp_log.h>
#include <map>
#include <memory>
#include <sys/unistd.h>

const char *const Alarms::TAG = "alarms";

Alarms::Alarms(NonVolatileStorage &non_volatile_storage)
    : non_volatile_storage(non_volatile_storage),
      alarms(std::map<std::string, std::unique_ptr<Alarm>>()) {
  std::string ids_string;
  esp_err_t err = non_volatile_storage.read(TAG, "ids", ids_string);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Alarm IDs read from NVS: %s", ids_string.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading alarm IDs from NVS: %s", esp_err_to_name(err));
    return;
  }

  cJSON *const ids_json = cJSON_Parse(ids_string.c_str());
  if (ids_json == nullptr || !cJSON_IsArray(ids_json)) {
    ESP_LOGE(TAG, "Error parsing JSON alarm IDs.");
    cJSON_Delete(ids_json);
    return;
  }

  const cJSON *id_json = nullptr;
  cJSON_ArrayForEach(id_json, ids_json) {
    const std::string id_string = id_json->valuestring;
    Alarm *alarm = new Alarm(non_volatile_storage, id_string);
    alarms.emplace(id_string, alarm);
  }

  cJSON_Delete(ids_json);
}

Alarms::~Alarms() { ESP_LOGI(TAG, "Destroy."); }

void Alarms::parse_initial(const std::string &data) {
  cJSON *const alarms_json = cJSON_Parse(data.c_str());
  if (alarms_json == nullptr || !cJSON_IsArray(alarms_json)) {
    ESP_LOGE(TAG, "Error parsing JSON alarms.");
    cJSON_Delete(alarms_json);
    return;
  }

  std::map<std::string, std::unique_ptr<Alarm>> alarms;

  const cJSON *alarm_json = nullptr;
  cJSON_ArrayForEach(alarm_json, alarms_json) {
    const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
    if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
      ESP_LOGE(TAG, "Did not find valid alarm ID.");
      continue;
    }

    const std::string id_string = id_json->valuestring;
    Alarm *alarm = new Alarm(non_volatile_storage, alarm_json);
    alarms.emplace(id_string, alarm);
  }

  this->alarms = std::move(alarms);
  write_ids_to_nvs();

  cJSON_Delete(alarms_json);
}

void Alarms::parse_insert(const std::string &data) {
  cJSON *const alarm_json = cJSON_Parse(data.c_str());
  if (alarm_json == nullptr || !cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    cJSON_Delete(alarm_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
    ESP_LOGE(TAG, "Did not find valid alarm ID.");
    cJSON_Delete(alarm_json);
    return;
  }

  const std::string id_string = id_json->valuestring;
  Alarm *alarm = new Alarm(non_volatile_storage, alarm_json);
  alarms.emplace(id_string, alarm);
  write_ids_to_nvs();

  cJSON_Delete(alarm_json);
};

void Alarms::parse_update(const std::string &data) {
  cJSON *const alarm_json = cJSON_Parse(data.c_str());
  if (alarm_json == nullptr || !cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    cJSON_Delete(alarm_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
    ESP_LOGE(TAG, "Did not find valid alarm ID.");
    cJSON_Delete(alarm_json);
    return;
  }

  const std::string id_string = id_json->valuestring;

  Alarm *alarm = new Alarm(non_volatile_storage, alarm_json);
  alarms.emplace(id_string, alarm);

  cJSON_Delete(alarm_json);
};

void Alarms::parse_remove(const std::string &data) {
  cJSON *const alarm_json = cJSON_Parse(data.c_str());
  if (!alarm_json) {
    ESP_LOGE(TAG, "Error parsing JSON alarm.");
    cJSON_Delete(alarm_json);
    return;
  }

  if (!cJSON_IsObject(alarm_json)) {
    ESP_LOGE(TAG, "Alarm is not an object.");
    cJSON_Delete(alarm_json);
    return;
  }

  const cJSON *const id_json = cJSON_GetObjectItem(alarm_json, "id");
  if (!cJSON_IsString(id_json) || (id_json->valuestring == nullptr)) {
    ESP_LOGE(TAG, "Did not find a valid alarm ID.");
    cJSON_Delete(alarm_json);
    return;
  }

  const std::string id_string = id_json->valuestring;
  alarms.erase(id_string);
  write_ids_to_nvs();

  cJSON_Delete(alarm_json);
};

void Alarms::write_ids_to_nvs() {
  cJSON *const ids_json = cJSON_CreateArray();
  for (const auto &pair : alarms) {
    cJSON *id_json = cJSON_CreateString(pair.first.c_str());
    cJSON_AddItemToArray(ids_json, id_json);
  }

  const std::string ids_string = cJSON_PrintUnformatted(ids_json);

  esp_err_t err = non_volatile_storage.write(TAG, "ids", ids_string);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Alarm IDs written to NVS: %s", ids_string.c_str());
  } else {
    ESP_LOGE(TAG, "Error writing alarm IDs to NVS: %s", esp_err_to_name(err));
  }

  cJSON_Delete(ids_json);
}
