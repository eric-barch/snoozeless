#include "Alarms.h"
#include "Button.h"
#include "Buzzer.h"
#include "CurrentTime.h"
#include "Device.h"
#include "Display.h"
#include "NonVolatileStorage.h"
#include "Session.h"
#include "StateMachine.h"
#include "StateStream.h"
#include "WifiConnection.h"
#include <freertos/idf_additions.h>

extern "C" void app_main(void) {
  /**ESP system utilities. */
  NonVolatileStorage non_volatile_storage;
  WifiConnection wifi_connection(non_volatile_storage);

  /**Auth. */
  Session session(non_volatile_storage);

  /**State. */
  Device device(non_volatile_storage);
  Alarms alarms(non_volatile_storage);

  /**Realtime state updates from backend. */
  StateStream state_stream(session, device, alarms);

  /**UNIX time. */
  CurrentTime current_time(non_volatile_storage, session);

  /**Peripherals. */
  Button button;
  Buzzer buzzer;
  Display display(device, current_time);

  /**Orchestrate processes. */
  StateMachine state_machine(device, alarms, current_time, button, display,
                             buzzer);

  vTaskDelay(portMAX_DELAY);
}
