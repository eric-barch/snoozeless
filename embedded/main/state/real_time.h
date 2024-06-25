#ifndef STATE_REAL_TIME_H
#define STATE_REAL_TIME_H

#include "esp_err.h"

typedef struct {
  int unix;
  int timestamp;
} real_time_t;

esp_err_t initialize_real_time(void);

esp_err_t set_real_time_unix(const int unix);

int get_real_time_unix(void);

esp_err_t set_real_time_timestamp(const int timestamp);

int get_real_time_timestamp(void);

#endif
