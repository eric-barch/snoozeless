#ifndef BUZZER_H
#define BUZZER_H

#include <driver/ledc.h>

class Buzzer {
public:
  Buzzer();
  ~Buzzer();

  void start_alarm();
  void stop_alarm();

private:
  static const char *const TAG;
  const ledc_timer_t TIMER_NUM;
  const ledc_channel_t CHANNEL;
  const ledc_mode_t SPEED_MODE;
  const int DUTY;
  const uint32_t FREQ_HZ;
  const int GPIO_NUM;
  ledc_timer_config_t timer_config;
  ledc_channel_config_t channel_config;
  bool alarm_is_on;

  static void alarm_task(void *pvParameters);
};

#endif
