#ifndef CURRENT_TIME_H
#define CURRENT_TIME_H

#include "NvsManager.h"
#include "Session.h"
#include <ctime>
#include <string>

class CurrentTime {
public:
  CurrentTime(NvsManager &nvs_manager, Session &session);
  ~CurrentTime();

  void set_utc_offset(int utc_offset);
  void set_format(std::string);
  std::tm get_time();

private:
  NvsManager &nvs_manager;
  Session &session;
  int unix_at_calibration;
  int ms_at_calibration;
  int utc_offset;
  std::string format;

  void set_unix_at_calibration(int unix_at_calibration);
  void set_ms_at_calibration(int ms_at_calibration);

  static void calibrate_on_data(void *current_time_instance,
                                const std::string &response);
  void calibrate();
};

#endif
