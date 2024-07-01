#include "NvsManager.h"
#include "Session.h"
#include "WifiManager.h"

extern "C" void app_main(void) {
  NvsManager nvs_manager;

  WifiManager wifi_manager(nvs_manager);
  wifi_manager.connect();

  Session session(nvs_manager);
  // Device device(nvs_manager, session);
  //
  // CurrentTime current_time(nvs_manager, session, device);
  // Display display(nvs_manager, current_time);
  //
  // session.keep_active();
  // device.subscribe();
  // current_time.subscribe();
  //
  // display.show_current_time();
}
