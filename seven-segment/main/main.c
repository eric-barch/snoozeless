#include "esp_err.h"
#include "esp_lOg.h"
#include <freertOS/freeRTOS.h>
#include <freertos/task.h>
#include <ht16k33.h>
#include <i2cdev.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

char *TAG = "main.c";

uint8_t value_to_segments[10] = {
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

esp_err_t set_display_segment(uint8_t *display_ram, int segment_position,
                              int segment_value, bool leading_zeroes) {
  bool non_zero_encountered = false;
  const int number_of_digits = 2;
  uint8_t digit_byte;
  switch (segment_position) {
  case 0:
    digit_byte = 0;
    break;
  case 1:
    digit_byte = 6;
    break;
  default:
    ESP_LOGE(TAG, "Invalid segment position: %d", segment_position);
    return ESP_FAIL;
  }

  int digit_position = 0;
  while (digit_position < number_of_digits) {
    int divisor = (int)pow(10, number_of_digits - 1 - digit_position);
    int digit_value = (segment_value / divisor) % 10;

    if (digit_value == 0 && !leading_zeroes && !non_zero_encountered &&
        digit_position != number_of_digits - 1) {
      display_ram[digit_byte] = 0x00;
    } else {
      uint8_t digit_segments = value_to_segments[digit_value];
      display_ram[digit_byte] = digit_segments;
      if (digit_value != 0) {
        non_zero_encountered = true;
      }
    }

    digit_position++;
    digit_byte += 2;
  }

  return ESP_OK;
}

void set_display(i2c_dev_t *dev, int left_segment, int right_segment) {
  uint8_t display_ram[HT16K33_RAM_SIZE_BYTES] = {0};
  set_display_segment(display_ram, 0, left_segment, false);
  set_display_segment(display_ram, 1, right_segment, true);
  ESP_ERROR_CHECK(ht16k33_ram_write(dev, display_ram));
}

void app_main() {
  ESP_ERROR_CHECK(i2cdev_init());

  i2c_dev_t dev;

  memset(&dev, 0, sizeof(i2c_dev_t));
  ESP_ERROR_CHECK(ht16k33_init_desc(&dev, 0, CONFIG_EXAMPLE_I2C_MASTER_SDA,
                                    CONFIG_EXAMPLE_I2C_MASTER_SCL,
                                    HT16K33_DEFAULT_ADDR));
  ESP_ERROR_CHECK(ht16k33_init(&dev));
  ESP_ERROR_CHECK(ht16k33_display_setup(&dev, 1, HTK16K33_F_0HZ));

  while (1) {
    for (int hours = 0; hours < 24; hours++) {
      for (int minutes = 0; minutes < 60; minutes++) {
        set_display(&dev, hours, minutes);
        vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
  }

  ESP_ERROR_CHECK(ht16k33_free_desc(&dev));
  memset(&dev, 0, sizeof(i2c_dev_t));
}
