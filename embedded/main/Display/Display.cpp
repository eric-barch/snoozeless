#include "Display.h"
#include "esp_log.h"
#include <cstring>

static const char *TAG = "Display";

Display::Display(CurrentTime &current_time)
    : current_time(current_time), top_indicator(false), bottom_indicator(false),
      colon(false), apostrophe(false), brightness(0) {
  this->init();
  ESP_LOGI(TAG, "Initialized Display: %s %s", this->major_interval,
           this->minor_interval);
}

void Display::set_major_interval(const std::string &major_interval) {
  if (major_interval.length() != 2) {
    ESP_LOGE(TAG, "Did not set major interval. Must be two characters long.");
    return;
  }
  strcpy(this->major_interval, major_interval.c_str());
}

void Display::set_minor_interval(const std::string &minor_interval) {
  if (minor_interval.length() != 2) {
    ESP_LOGE(TAG, "Did not set minor interval. Must be two characters long.");
    return;
  }
  strcpy(this->minor_interval, minor_interval.c_str());
}

void Display::init() {
  this->set_major_interval("TE");
  this->set_minor_interval("ST");
}
