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

char *get_device_id(void);

#endif
