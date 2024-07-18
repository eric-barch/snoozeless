#ifndef BUZZER_H
#define BUZZER_H

#include "driver/ledc.h"
class Buzzer {
public:
  Buzzer();
  ~Buzzer();

  void start_buzzing();
  void stop_buzzing();

private:
  ledc_timer_config_t buzzer_timer;
  ledc_channel_config_t buzzer_channel;
  bool is_buzzing;

  static void buzz_task(void *pvParameters);
};

#endif
