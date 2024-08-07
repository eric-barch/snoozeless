#ifndef CURRENT_TIME_H
#define CURRENT_TIME_H

#include "NonVolatileStorage.h"
#include "Session.h"
#include <ctime>
#include <esp_err.h>
#include <string>

class CurrentTime {
public:
  CurrentTime(NonVolatileStorage &non_volatile_storage, Session &session);

  ~CurrentTime();

  std::tm get_time();
  void on_data(const std::string &response);

private:
  static const char *const TAG;

  NonVolatileStorage &non_volatile_storage;
  Session &session;
  int unix_at_calibration;
  int ms_at_calibration;

  void set_unix_at_calibration(const int &unix_at_calibration);
  void set_ms_at_calibration(const int &ms_at_calibration);

  static void keep_calibrated(void *const pvParameters);

  esp_err_t calibrate();
};

#endif // CURRENT_TIME_H
