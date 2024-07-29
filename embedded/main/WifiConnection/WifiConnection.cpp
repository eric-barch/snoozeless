#include "WifiConnection.h"
#include "NonVolatileStorage.h"
#include <cstring>
#include <esp_err.h>
#include <esp_event_base.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <freertos/idf_additions.h>
#include <sdkconfig.h>
#include <string>

const char *const WifiConnection::TAG = "wifi_conn";

const int WifiConnection::WIFI_CONNECTED_BIT = BIT0;

const int WifiConnection::WIFI_FAIL_BIT = BIT1;

const int WifiConnection::MAX_RETRY = 5;

WifiConnection::WifiConnection(NonVolatileStorage &non_volatile_storage)
    : non_volatile_storage(non_volatile_storage), ssid(), password(),
      wifi_event_group(xEventGroupCreate()), retry_count(0) {
  esp_err_t err = non_volatile_storage.read(TAG, "ssid", ssid);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "SSID read from NVS: %s", ssid.c_str());
    set_ssid(ssid);
  } else {
    ESP_LOGW(TAG, "Error reading SSID from NVS. Using config.");
    set_ssid(CONFIG_WIFI_SSID);
  }

  err = non_volatile_storage.read(TAG, "password", password);
  if (err == ESP_OK) {
    ESP_LOGD(TAG, "Password read from NVS: %s", password.c_str());
    set_password(password);
  } else {
    ESP_LOGW(TAG, "Error reading password from NVS. Using config.");
    set_password(CONFIG_WIFI_PASSWORD);
  }

  connect();
}

WifiConnection::~WifiConnection() {
  disconnect();

  if (wifi_event_group) {
    vEventGroupDelete(wifi_event_group);
  }

  ESP_LOGI(TAG, "Destroy.");
}

void WifiConnection::set_ssid(std::string ssid) {
  this->ssid = ssid;
  non_volatile_storage.write(TAG, "ssid", ssid);
  ESP_LOGI(TAG, "Set SSID: %s", ssid.c_str());
}

void WifiConnection::set_password(std::string password) {
  this->password = password;
  non_volatile_storage.write(TAG, "password", password);
  ESP_LOGI(TAG, "Set Password: %s", ssid.c_str());
}

void WifiConnection::handle_wifi_event(void *arg, esp_event_base_t event_base,
                                       int32_t event_id, void *event_data) {
  WifiConnection *self = static_cast<WifiConnection *>(arg);

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (self->retry_count < MAX_RETRY) {
      esp_wifi_connect();
      self->retry_count++;
      ESP_LOGI(TAG, "Trying to connect to access point again.");
    } else {
      xEventGroupSetBits(self->wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGW(TAG, "Failed to connect to access point.");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    self->retry_count = 0;
    xEventGroupSetBits(self->wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

esp_err_t WifiConnection::connect() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiConnection::handle_wifi_event, this,
      NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiConnection::handle_wifi_event, this,
      NULL));

  wifi_config_t wifi_config = {};
  strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.c_str(),
          sizeof(wifi_config.sta.ssid) - 1);
  strncpy(reinterpret_cast<char *>(wifi_config.sta.password), password.c_str(),
          sizeof(wifi_config.sta.password) - 1);
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  EventBits_t bits = xEventGroupWaitBits(this->wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Connected to SSID %s", this->ssid.c_str());
    return ESP_OK;
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGE(TAG, "Failed to connect to SSID %s", this->ssid.c_str());
    return ESP_FAIL;
  } else {
    ESP_LOGE(TAG, "Unexpected event connecting to SSID %s", this->ssid.c_str());
    return ESP_FAIL;
  }
}

esp_err_t WifiConnection::disconnect() {
  ESP_ERROR_CHECK(esp_wifi_stop());
  ESP_LOGI(TAG, "Disconnected from Wi-Fi.");
  return ESP_OK;
}
