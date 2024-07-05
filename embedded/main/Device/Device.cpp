#include "Device.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "Session.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "Device";

Device::Device(NvsManager &nvs_manager, Session &session)
    : nvs_manager(nvs_manager), session(session) {
  this->init();
}

void Device::init() {
  esp_err_t err = this->nvs_manager.read_string("device", "id", this->id);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Device ID read from NVS: %s", this->id.c_str());
  } else {
    ESP_LOGW(TAG, "Error reading Device ID from NVS: %s. Enrolling device.",
             esp_err_to_name(err));
    this->enroll();
  }
}

void Device::enroll() {
  ApiRequest post_device_register(session, HTTP_METHOD_POST, 60000,
                                  "/device/register");
  post_device_register.call();
}

void Device::subscribe() {}
