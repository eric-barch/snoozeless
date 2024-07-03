#include "Device.h"
#include "NvsManager.h"
#include "Session.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "nvs.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
  esp_err_t err = nvs_manager.read_string("device", "id", id);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Device ID read from NVS: %s", id.c_str());
    return;
  }

  if (err != ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGE(TAG, "Error reading Device ID from NVS: %s", esp_err_to_name(err));
    return;
  }

  ESP_LOGW(TAG, "Device ID not found in NVS. Enrolling this device.");
  enroll();

  if (id.empty()) {
    ESP_LOGE(TAG, "Could not set Device ID in NVS. ID is empty.");
    return;
  }

  nvs_manager.write_string("device", "id", id);
}

Device::~Device() {}

esp_err_t Device::enroll() {
}

esp_err_t Device::subscribe() {
}
