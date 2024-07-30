#ifndef DISPLAY_H
#define DISPLAY_H

#include "CurrentTime.h"
#include "NonVolatileStorage.h"
#include <ht16k33.h>
#include <i2cdev.h>
#include <map>

class Display {
public:
  Display(NonVolatileStorage &non_volatile_storage, CurrentTime &current_time);

  ~Display();

  void set_brightness(uint8_t brightness);

  void print_current_time();
  void print_9_41();

private:
  static const char *const TAG;
  static const std::map<const char, const uint8_t> alphabet;

  NonVolatileStorage &non_volatile_storage;
  CurrentTime &current_time;
  i2c_dev_t ht16k33;
  uint8_t ht16k33_ram[HT16K33_RAM_SIZE_BYTES];
  bool top_indicator;
  bool bottom_indicator;
  bool colon;
  bool apostrophe;
  int brightness;
  std::string major_interval;
  std::string minor_interval;

  void set_major_interval(const std::string &major_interval);
  void set_minor_interval(const std::string &minor_interval);

  static void handle_print(void *const pvParameters);

  void print();
};

#endif // DISPLAY_H
