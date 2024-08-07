#ifndef DEVICE_H
#define DEVICE_H

#include "NonVolatileStorage.h"
#include <esp_err.h>
#include <string>

class Device {
public:
  Device(NonVolatileStorage &non_volatile_storage);

  ~Device();

  std::string get_id();
  int get_brightness();
  std::string get_time_zone();
  std::string get_time_format();

  void parse(const std::string &data);

private:
  static const char *const TAG;

  NonVolatileStorage &non_volatile_storage;
  std::string id;
  int brightness;
  std::string time_zone;
  std::string time_format;

  void set_id(const std::string &id);
  void set_brightness(const int &brightness);
  void set_time_zone(const std::string &time_zone);
  void set_time_format(const std::string &time_format);
};

#endif // DEVICE_H
