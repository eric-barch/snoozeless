#include "NonVolatileStorage.h"
#include <esp_err.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <string>

NonVolatileStorage::NonVolatileStorage() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}

NonVolatileStorage::~NonVolatileStorage() {
  if (nvs_handle) {
    nvs_close(nvs_handle);
  }
  ESP_LOGI(TAG, "Destroy.");
}

const char *const NonVolatileStorage::TAG = "non_vol_stor";

esp_err_t NonVolatileStorage::write(const std::string &space,
                                    const std::string &key,
                                    const std::string &in_value) {
  esp_err_t err = open_namespace(space);

  err = nvs_set_str(nvs_handle, key.c_str(), in_value.c_str());
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error setting string in NVS: %s", esp_err_to_name(err));
    nvs_close(nvs_handle);
    return err;
  }

  err = nvs_commit(nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error committing string to NVS: %s", esp_err_to_name(err));
  } else {
    ESP_LOGD(TAG, "Committed string to NVS: %s", in_value.c_str());
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t NonVolatileStorage::read(const std::string &space,
                                   const std::string &key,
                                   std::string &out_value) {
  esp_err_t err = open_namespace(space);

  size_t required_size = 0;
  err = nvs_get_str(nvs_handle, key.c_str(), nullptr, &required_size);
  if (err != ESP_OK) {
    ESP_LOGD(TAG, "Error getting string size from NVS: %s",
             esp_err_to_name(err));
    nvs_close(nvs_handle);
    return err;
  }

  char *buffer = new char[required_size];
  err = nvs_get_str(nvs_handle, key.c_str(), buffer, &required_size);
  if (err != ESP_OK) {
    ESP_LOGD(TAG, "Error reading string from NVS: %s", esp_err_to_name(err));
  } else {
    out_value.assign(buffer);
    ESP_LOGD(TAG, "Read string from NVS: %s", out_value.c_str());
  }

  delete[] buffer;
  nvs_close(nvs_handle);
  return err;
}

esp_err_t NonVolatileStorage::write(const std::string &space,
                                    const std::string &key,
                                    const int &in_value) {
  esp_err_t err = open_namespace(space);

  int32_t temp_value = static_cast<int32_t>(in_value);
  err = nvs_set_i32(nvs_handle, key.c_str(), temp_value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error setting int in NVS: %s", esp_err_to_name(err));
    nvs_close(nvs_handle);
    return err;
  }

  err = nvs_commit(nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error committing int to NVS: %s", esp_err_to_name(err));
  } else {
    ESP_LOGD(TAG, "Committed int to NVS: %d", in_value);
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t NonVolatileStorage::read(const std::string &space,
                                   const std::string &key, int &out_value) {
  esp_err_t err = open_namespace(space);

  int32_t temp_value;
  err = nvs_get_i32(nvs_handle, key.c_str(), &temp_value);
  if (err != ESP_OK) {
    ESP_LOGD(TAG, "Error reading int from NVS: %s", esp_err_to_name(err));
  } else {
    out_value = static_cast<int>(temp_value);
    ESP_LOGD(TAG, "Read int from NVS: %d", out_value);
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t NonVolatileStorage::erase(const std::string &space,
                                    const std::string &key) {
  esp_err_t err = open_namespace(space);

  err = nvs_erase_key(nvs_handle, key.c_str());
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error erasing key in NVS: %s", esp_err_to_name(err));
  } else {
    ESP_LOGD(TAG, "Erased key in NVS: %s", key.c_str());
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t NonVolatileStorage::open_namespace(const std::string &space) {
  esp_err_t err = nvs_open(space.c_str(), NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS namespace: %s", esp_err_to_name(err));
  } else {
    ESP_LOGD(TAG, "Opened NVS namespace: %s", space.c_str());
  }

  return err;
}
