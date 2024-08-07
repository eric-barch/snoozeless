#include "StateMachine.h"
#include "CurrentTime.h"
#include "freertos/projdefs.h"
#include <esp_log.h>
#include <freertos/idf_additions.h>

StateMachine::StateMachine(Device &device, Alarms &alarms,
                           CurrentTime &current_time, Button &button,
                           Display &display, Buzzer &buzzer)
    : device(device), alarms(alarms), current_time(current_time),
      button(button), display(display), buzzer(buzzer), state(DISPLAY_TIME) {
  xTaskCreate(run, "run", 4096, this, 5, NULL);
}

StateMachine::~StateMachine() { ESP_LOGI(TAG, "Destroy."); }

const char *const StateMachine::TAG = "state_machine";

void StateMachine::run(void *const pvParameters) {
  StateMachine *self = static_cast<StateMachine *>(pvParameters);

  while (true) {
    switch (self->state) {
    case DISPLAY_TIME:
      self->display.print_current_time();
      vTaskDelay(pdMS_TO_TICKS(100));
      break;
    case WAKE_UP:
      break;
    case COUNT_DOWN:
      break;
    case SOUND_FOREVER:
      break;
    default:
      ESP_LOGE(TAG, "Unrecognized StateMachine state.");
      break;
    }
  }

  vTaskDelete(NULL);
}
