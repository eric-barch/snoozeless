#include "WifiManager.h"
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

static const char *TAG = "WifiManager";

WifiManager::WifiManager(NonVolatileStorage &non_volatile_storage)
    : non_volatile_storage(non_volatile_storage), wifi_event_group(nullptr),
      retry_count(0) {
  std::string ssid;
  esp_err_t err =
      this->non_volatile_storage.read_key("wifi_cred", "ssid", ssid);
  if (err == ESP_OK) {
    this->set_ssid(ssid);
    ESP_LOGI(TAG, "SSID read from NVS: %s", ssid.c_str());
  } else {
    this->set_ssid(CONFIG_WIFI_SSID);
    ESP_LOGW(TAG, "Error reading SSID from NVS. Used config.");
  }

  std::string password;
  err = this->non_volatile_storage.read_key("wifi_cred", "password", password);
  if (err == ESP_OK) {
    this->set_password(password);
    ESP_LOGI(TAG, "Password read from NVS: %s", password.c_str());
  } else {
    this->set_password(CONFIG_WIFI_PASSWORD);
    ESP_LOGW(TAG, "Error reading password from NVS. Used config.");
  }

  this->wifi_event_group = xEventGroupCreate();
  this->connect();
}

WifiManager::~WifiManager() {
  if (wifi_event_group) {
    vEventGroupDelete(wifi_event_group);
  }
  ESP_LOGI(TAG, "Destruct.");
}

void WifiManager::set_ssid(std::string ssid) {
  this->ssid = ssid;
  this->non_volatile_storage.write_key("wifi_cred", "ssid", ssid);
  ESP_LOGI(TAG, "Set SSID: %s", ssid.c_str());
}

void WifiManager::set_password(std::string password) {
  this->password = password;
  this->non_volatile_storage.write_key("wifi_cred", "password", password);
  ESP_LOGI(TAG, "Set Password: %s", ssid.c_str());
}

void WifiManager::handle_wifi_event(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data) {
  WifiManager *wifi_manager = static_cast<WifiManager *>(arg);

  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (wifi_manager->retry_count < MAX_RETRY) {
      esp_wifi_connect();
      wifi_manager->retry_count++;
      ESP_LOGI(TAG, "Trying to connect to access point again.");
    } else {
      xEventGroupSetBits(wifi_manager->wifi_event_group, WIFI_FAIL_BIT);
    }
    ESP_LOGW(TAG, "Failed to connect to access point.");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    wifi_manager->retry_count = 0;
    xEventGroupSetBits(wifi_manager->wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

esp_err_t WifiManager::connect() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiManager::handle_wifi_event, this,
      NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiManager::handle_wifi_event, this,
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

esp_err_t WifiManager::disconnect() {
  ESP_ERROR_CHECK(esp_wifi_stop());
  ESP_LOGI(TAG, "Disconnected from Wi-Fi.");
  return ESP_OK;
}
