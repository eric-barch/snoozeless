#ifndef STATE_REAL_TIME_H
#define STATE_REAL_TIME_H

#include "esp_err.h"

typedef struct {
  int unix;
  int ticks;
} real_time_t;

esp_err_t set_real_time_unix(const int unix);

int get_real_time_unix(void);

esp_err_t set_real_time_ticks(const int ticks);

int get_real_time_ticks(void);

#endif
