#ifndef CURRENT_TIME_H
#define CURRENT_TIME_H

#include "NvsManager.h"
#include "Session.h"
#include <string>

class CurrentTime {
public:
  CurrentTime(NvsManager &nvs_manager, Session &session);

  void set_utc_offset(int utc_offset);
  void set_format(std::string);

private:
  NvsManager &nvs_manager;
  Session &session;
  int unix_at_calibration;
  int ms_at_calibration;
  int utc_offset;
  std::string format;

  static void calibrate_on_data(void *current_time_instance,
                                const std::string &response);
  void calibrate();
  void init();
};

#endif
