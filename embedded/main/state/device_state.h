#ifndef DEVICE_H
#define DEVICE_H

#include "esp_err.h"

#define MAX_ID_LENGTH 50
#define MAX_TIME_FORMAT_LENGTH 10

typedef struct {
  char id[MAX_ID_LENGTH];
  char time_format[MAX_TIME_FORMAT_LENGTH];
  int utc_offset;
  int brightness;
} device_t;

esp_err_t initialize_device_state(void);

esp_err_t set_device_id(const char *device_id);

char *get_device_id(void);

esp_err_t set_device_time_format(char *time_format);

char *get_device_time_format(void);

#endif
