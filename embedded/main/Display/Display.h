#ifndef DISPLAY_H
#define DISPLAY_H

#include "CurrentTime.h"
#include "NvsManager.h"
#include "ht16k33.h"
#include "i2cdev.h"

class Display {
public:
  Display(NvsManager &nvs_manager, CurrentTime &current_time);
  ~Display();

  void set_brightness(uint8_t brightness);
  void count();

private:
  const uint8_t value_to_segments[10] = {
      0x3F, // 0
      0x06, // 1
      0x5B, // 2
      0x4F, // 3
      0x66, // 4
      0x6D, // 5
      0x7D, // 6
      0x07, // 7
      0x7F, // 8
      0x6F  // 9
  };

  NvsManager &nvs_manager;
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
  static void count_task(void *pvParameters);
};

#endif
