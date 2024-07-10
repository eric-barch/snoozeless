#include "Display.h"
#include "esp_log.h"
#include <cstring>
// extern "C" {
#include "ht16k33.h"
#include "i2cdev.h"
// }

static const char *TAG = "Display";

Display::Display(CurrentTime &current_time)
    : current_time(current_time), top_indicator(false), bottom_indicator(false),
      colon(false), apostrophe(false), brightness(7) {
  this->init();
  ESP_LOGI(TAG, "Initialized Display: %s %s", this->major_interval,
           this->minor_interval);
}

void Display::set_brightness(int brightness) {
  if (brightness < 0 || brightness > 15) {
    ESP_LOGE(TAG,
             "Did not set brightness. Must be between 0 and 15 inclusive.");
    return;
  }
  this->brightness = brightness;
  ESP_LOGI(TAG, "Set brightness: %d", brightness);
}

void Display::set_major_interval(const std::string &major_interval) {
  if (major_interval.length() != 2) {
    ESP_LOGE(TAG, "Did not set major interval. Must be two characters long.");
    return;
  }
  strcpy(this->major_interval, major_interval.c_str());
  ESP_LOGI(TAG, "Set major interval: %s", major_interval.c_str());
}

void Display::set_minor_interval(const std::string &minor_interval) {
  if (minor_interval.length() != 2) {
    ESP_LOGE(TAG, "Did not set minor interval. Must be two characters long.");
    return;
  }
  strcpy(this->minor_interval, minor_interval.c_str());
  ESP_LOGI(TAG, "Set minor interval: %s", minor_interval.c_str());
}

void Display::init() {
  this->set_major_interval("TE");
  this->set_minor_interval("ST");
}

void Display::show_current_time() {
  ESP_ERROR_CHECK(i2cdev_init());

  i2c_dev_t dev;
  memset(&dev, 0, sizeof(i2c_dev_t));

  gpio_num_t sda = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SDA);
  gpio_num_t scl = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SCL);
  i2c_port_t port = static_cast<i2c_port_t>(0);

  ESP_ERROR_CHECK(
      ht16k33_init_desc(&dev, port, sda, scl, HT16K33_DEFAULT_ADDR));
  ESP_ERROR_CHECK(ht16k33_init(&dev));
  ESP_ERROR_CHECK(ht16k33_display_setup(&dev, 1, HTK16K33_F_0HZ));

  // while (1) {
  //   for (int hours = 0; hours < 24; hours++) {
  //     for (int minutes = 0; minutes < 60; minutes++) {
  //       // set_display(&dev, hours, minutes);
  //       vTaskDelay(pdMS_TO_TICKS(100));
  //     }
  //   }
  // }

  ESP_ERROR_CHECK(ht16k33_free_desc(&dev));
  memset(&dev, 0, sizeof(i2c_dev_t));

  ESP_LOGI(TAG, "Completed `show_current_time`");
}
