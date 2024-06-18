#include "nvs.h"
#include "nvs_flash.h"

static nvs_handle_t handle;

esp_err_t initialize_nvs(void) {
  esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    return err;
}

esp_err_t open_nvs_namespace(const char *namespace) {
  esp_err_t err = nvs_open(namespace, NVS_READWRITE, &handle);

  switch (err) {
  case ESP_OK:
    printf("Opened NVS namespace: %s\n", namespace);
    break;
  default:
    printf("Error opening NVS namespace: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

esp_err_t get_nvs_str(const char *key, char *out_value, size_t max_length) {
  esp_err_t err = nvs_get_str(handle, key, out_value, &max_length);

  switch (err) {
  case ESP_OK:
    printf("Got string from NVS\n%s: %s\n", key, out_value);
    break;
  case ESP_ERR_NVS_NOT_FOUND:
    printf("Key not found in NVS: %s\n", key);
    break;
  default:
    printf("Error getting string from NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

esp_err_t set_nvs_str(const char *key, const char *in_value) {
  esp_err_t err = nvs_set_str(handle, key, in_value);

  if (err == ESP_OK) {
    err = nvs_commit(handle);
  }

  switch (err) {
  case ESP_OK:
    printf("Set string in NVS\n%s: %s\n", key, in_value);
    break;
  default:
    printf("Error setting string in NVS: %s\n", esp_err_to_name(err));
    break;
  }

  return err;
}

void close_nvs(void) {
  nvs_close(handle);
  printf("Closed NVS handle.\n");
}
