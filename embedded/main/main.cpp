#include "NvsManager.h"
#include "WifiManager.h"

extern "C" void app_main(void) {
  NvsManager nvs_manager;

  WifiManager wifi_manager(nvs_manager);
  wifi_manager.connect();

  // User user(nvs_manager);
  // Device device(nvs_manager, user);
  //
  // CurrentTime current_time(nvs_manager, user, device);
  // Display display(nvs_manager, current_time);
  //
  // user.maintain_session();
  // device.subscribe();
  // current_time.subscribe();
  //
  // display.show_current_time();
}
