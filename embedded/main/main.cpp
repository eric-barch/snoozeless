#include "Alarm.h"
#include "Alarms.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Device.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include "WifiConnection.h"
#include <freertos/idf_additions.h>

/**TODO:
 * - Refactor semaphores to direct-to-task notifications. */

extern "C" void app_main(void) {
  NonVolatileStorage non_volatile_storage;
  WifiConnection wifi_connection(non_volatile_storage);

  Session session(non_volatile_storage);
  CurrentTime current_time(non_volatile_storage, session);
  Alarms alarms(non_volatile_storage);
  Display display(non_volatile_storage, current_time);
  Buzzer buzzer;

  // Device device(non_volatile_storage, session, current_time, alarms, display,
  //               buzzer);

  vTaskDelay(portMAX_DELAY);
}
