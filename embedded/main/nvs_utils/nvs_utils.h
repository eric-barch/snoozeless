#ifndef NVS_UTILS_H
#define NVS_UTILS_H

#include "esp_err.h"
#include "nvs.h"

void initialize_nvs(void);

esp_err_t initialize_nvs_str(nvs_handle_t nvs_handle, const char *key,
                             char *out_value, size_t max_length);

#endif
