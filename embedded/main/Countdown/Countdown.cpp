#include "Countdown.h"
#include <algorithm>
#include <esp_log.h>

Countdown::Countdown(const int &duration)
    : duration(duration), ms_at_start(esp_log_timestamp()) {}

Countdown::~Countdown() { ESP_LOGI(TAG, "Destroy."); }

int Countdown::get_seconds_remaining() {
  const int ms_since_start = esp_log_timestamp() - ms_at_start;
  const int seconds_remaining = duration - (ms_since_start / 1000);
  return std::max(seconds_remaining, 0);
}

const char *const Countdown::TAG = "Countdown";
