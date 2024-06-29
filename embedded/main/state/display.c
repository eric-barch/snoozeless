#include "state/display.h"
#include "esp_err.h"

static display_state_t display_state;

esp_err_t set_display_pm(const bool display_pm) {
  display_state.pm = display_pm;
  return ESP_OK;
}

bool get_display_pm(void) { return display_state.pm; }

esp_err_t set_display_armed(const bool display_armed) {
  display_state.armed = display_armed;
  return ESP_OK;
}

bool get_display_armed(void) { return display_state.armed; }

esp_err_t set_display_colon(const bool display_colon) {
  display_state.colon = display_colon;
  return ESP_OK;
}

bool get_display_colon(void) { return display_state.colon; }

esp_err_t set_display_apostrophe(const bool display_apostrophe) {
  display_state.apostrophe = display_apostrophe;
  return ESP_OK;
}

bool get_display_apostrophe(void) { return display_state.apostrophe; }

esp_err_t set_display_major_interval(const int display_major_interval) {
  display_state.major_interval = display_major_interval;
  return ESP_OK;
}

int get_display_major_interval(void) { return display_state.major_interval; }

esp_err_t set_display_minor_interval(const int display_minor_interval) {
  display_state.minor_interval = display_minor_interval;
  return ESP_OK;
}

int get_display_minor_interval(void) { return display_state.minor_interval; }

esp_err_t set_display_brightness(const int display_brightness) {
  display_state.brightness = display_brightness;
  return ESP_OK;
}

int get_display_brightness(void) { return display_state.brightness; }
