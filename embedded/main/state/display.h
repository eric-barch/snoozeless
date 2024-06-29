#ifndef STATE_DISPLAY_H
#define STATE_DISPLAY_H

#include "esp_err.h"
#include <stdbool.h>

typedef struct {
  bool pm;
  bool armed;
  bool colon;
  bool apostrophe;
  int major_interval;
  int minor_interval;
  int brightness;
} display_state_t;

esp_err_t set_display_pm(const bool display_pm);

bool get_display_pm(void);

esp_err_t set_display_armed(const bool display_armed);

bool get_display_armed(void);

esp_err_t set_display_colon(const bool display_colon);

bool get_display_colon(void);

esp_err_t set_display_apostrophe(const bool display_apostrophe);

bool get_display_apostrophe(void);

esp_err_t set_display_major_interval(const int display_major_interval);

int get_display_major_interval(void);

esp_err_t set_display_minor_interval(const int display_minor_interval);

int get_display_minor_interval(void);

esp_err_t set_display_brightness(const int display_brightness);

int get_display_brightness(void);

#endif
