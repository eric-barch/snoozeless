#include "NvsManager.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <cstring>

static const char *TAG = "NvsManager";

NvsManager::NvsManager() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}

NvsManager::~NvsManager() {
  if (nvs_handle != 0) {
    nvs_close(nvs_handle);
  }
}

esp_err_t NvsManager::read_string(const std::string &nvs_namespace,
                                  const std::string &key,
                                  std::string &out_value) {
  esp_err_t err = nvs_open(nvs_namespace.c_str(), NVS_READONLY, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS namespace: %s", esp_err_to_name(err));
    return err;
  }

  size_t required_size = 0;
  err = nvs_get_str(nvs_handle, key.c_str(), nullptr, &required_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error getting string size from NVS: %s",
             esp_err_to_name(err));
    nvs_close(nvs_handle);
    return err;
  }

  char *buffer = new char[required_size];
  err = nvs_get_str(nvs_handle, key.c_str(), buffer, &required_size);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error reading string from NVS: %s", esp_err_to_name(err));
  } else {
    out_value.assign(buffer);
  }

  delete[] buffer;
  nvs_close(nvs_handle);
  return err;
}

esp_err_t NvsManager::write_string(const std::string &nvs_namespace,
                                   const std::string &key,
                                   const std::string &in_value) {
  esp_err_t err = nvs_open(nvs_namespace.c_str(), NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS namespace: %s", esp_err_to_name(err));
    return err;
  }

  err = nvs_set_str(nvs_handle, key.c_str(), in_value.c_str());
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error setting string in NVS: %s", esp_err_to_name(err));
    nvs_close(nvs_handle);
    return err;
  }

  err = nvs_commit(nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error committing string to NVS: %s", esp_err_to_name(err));
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t NvsManager::read_int(const std::string &nvs_namespace,
                               const std::string &key, int &out_value) {
  esp_err_t err = nvs_open(nvs_namespace.c_str(), NVS_READONLY, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS namespace: %s", esp_err_to_name(err));
    return err;
  }

  int32_t temp_value;
  err = nvs_get_i32(nvs_handle, key.c_str(), &temp_value);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error reading int from NVS: %s", esp_err_to_name(err));
  } else {
    out_value = static_cast<int>(temp_value);
  }

  nvs_close(nvs_handle);
  return err;
}

esp_err_t NvsManager::write_int(const std::string &namespace_name,
                                const std::string &key, const int &in_value) {
  esp_err_t err = nvs_open(namespace_name.c_str(), NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS namespace: %s", esp_err_to_name(err));
    return err;
  }

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
  }

  nvs_close(nvs_handle);
  return err;
}
