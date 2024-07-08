#include "Device.h"
#include "ApiRequest.h"
#include "NvsManager.h"
#include "Session.h"
#include "esp_err.h"
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

void Device::enroll_on_data(void *device_context, const std::string &response) {
  Device *self = static_cast<Device *>(device_context);
  ESP_LOGI(TAG, "Device memory address in enroll_on_data: %p", self);
  ESP_LOGI(TAG, "enroll_on_data memory address in enroll_on_data: %p",
           self->enroll_on_data);
  ESP_LOGI(TAG, "Enroll API Response: %s", response.c_str());
}

void Device::enroll() {
  ESP_LOGI(TAG, "Device memory address in enroll: %p", this);
  ESP_LOGI(TAG, "enroll_on_data memory address in enroll: %p", enroll_on_data);
  ApiRequest post_device_register(session, HTTP_METHOD_POST, 60000,
                                  "/device/register", "", this, enroll_on_data);
  post_device_register.send_request();
}

void Device::subscribe() {}
