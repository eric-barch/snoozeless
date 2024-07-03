#include "Device.h"
#include "ApiRequest.h"
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
}

esp_err_t Device::enroll() {
  ApiRequest post_device_register(session, HTTP_METHOD_POST, 60000,
                                  "/device/register");
  post_device_register.call();
  return ESP_OK;
}

esp_err_t Device::subscribe() {
  if (id.empty()) {
    ESP_LOGE(TAG, "Cannot subscribe. Device ID is empty.");
    return ESP_FAIL;
  }

  std::string query = "deviceId=" + id;
  ApiRequest get_device_state(session, HTTP_METHOD_GET, 300000, "device/state",
                              query);

  get_device_state.set_on_data_callback([](const std::string &data) {
    ESP_LOGI(TAG, "Received update: %s", data.c_str());
  });

  get_device_state.call();

  return ESP_OK;
}
