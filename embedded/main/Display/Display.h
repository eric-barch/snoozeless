#ifndef DISPLAY_H
#define DISPLAY_H

#include "CurrentTime.h"
#include "NonVolatileStorage.h"
#include <ht16k33.h>
#include <i2cdev.h>

class Display {
public:
  Display(NonVolatileStorage &non_volatile_storage, CurrentTime &current_time);
  ~Display();

  void set_brightness(uint8_t brightness);

  void print_current_time();
  void print_9_41();

private:
  static const char *const TAG;
  static const std::unordered_map<char, uint8_t> alphabet;
  NonVolatileStorage &non_volatile_storage;
  CurrentTime &current_time;
  i2c_dev_t ht16k33;
  uint8_t ht16k33_ram[HT16K33_RAM_SIZE_BYTES];
  bool top_indicator;
  bool bottom_indicator;
  bool colon;
  bool apostrophe;
  int brightness;
  /** Each interval (e.g. hours "place", minutes "place") is two characters
   * plus a null terminator. */
  char major_interval[3];
  char minor_interval[3];

  void set_major_interval(const std::string &major_interval);
  void set_minor_interval(const std::string &minor_interval);

  void print();
  static void print_current_time_task(void *pvParameters);
};

#endif // DISPLAY_H
