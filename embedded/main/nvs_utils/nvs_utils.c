#include "esp_err.h"
#include "linenoise/linenoise.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdio.h>

#define MAX_PROMPT_LENGTH 64

void initialize_nvs(void) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}

esp_err_t initialize_nvs_str(nvs_handle_t nvs_handle, const char *key,
                             char *out_value, size_t max_length) {
  size_t length = max_length;
  esp_err_t err = nvs_get_str(nvs_handle, key, out_value, &length);

  switch (err) {
  case ESP_OK:
    printf("%s read from NVS: %s\n", key, out_value);
    return ESP_OK;
  case ESP_ERR_NVS_NOT_FOUND:
    printf("%s not initialized in NVS.\n", key);

    char prompt[MAX_PROMPT_LENGTH];
    snprintf(prompt, sizeof(prompt), "Enter %s: ", key);

    char *input = linenoise(prompt);
    if (input != NULL) {
      err = nvs_set_str(nvs_handle, key, input);
      if (err != ESP_OK) {
        printf("Error initializing %s in NVS: %s\n", key, esp_err_to_name(err));
        linenoiseFree(input);
        return err;
      } else {
        err = nvs_get_str(nvs_handle, key, out_value, &length);
        if (err == ESP_OK) {
          printf("Initialized %s in NVS: %s\n", key, out_value);
          linenoiseFree(input);
          return ESP_OK;
        } else {
          printf("Error reading initialized %s from NVS: %s\n", key,
                 esp_err_to_name(err));
          linenoiseFree(input);
          return err;
        }
      }
    } else {
      return ESP_ERR_NVS_NOT_FOUND;
    }
  default:
    printf("Error reading %s from NVS: %s\n", key, esp_err_to_name(err));
    return err;
  }
}
