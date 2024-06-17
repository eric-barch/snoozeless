#ifndef NVS_UTILS_H
#define NVS_UTILS_H

#include "esp_err.h"

esp_err_t initialize_nvs(void);

esp_err_t open_nvs_namespace(char *namespace);

esp_err_t get_nvs_str(char *key, char *out_value, size_t max_length);

esp_err_t set_nvs_str(char *key, char *in_value, size_t max_length);

esp_err_t close_nvs(void);

#endif
