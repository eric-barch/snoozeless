#include "Display.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "ht16k33.h"
#include "i2cdev.h"
#include <cstring>
#include <ctime>

static const char *TAG = "Display";

Display::Display(NvsManager &nvs_manager, CurrentTime &current_time)
    : nvs_manager(nvs_manager), current_time(current_time),
      top_indicator(false), bottom_indicator(false), colon(false),
      apostrophe(false), brightness(7) {
  memset(&(this->ht16k33), 0, sizeof(i2c_dev_t));
  memset(&(this->ht16k33_ram), 0, sizeof(ht16k33_ram));

  gpio_num_t sda = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SDA);
  gpio_num_t scl = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SCL);
  i2c_port_t port = static_cast<i2c_port_t>(0);

  ESP_ERROR_CHECK(i2cdev_init());
  ESP_ERROR_CHECK(ht16k33_init_desc(&(this->ht16k33), port, sda, scl,
                                    HT16K33_DEFAULT_ADDR));
  ESP_ERROR_CHECK(ht16k33_init(&(this->ht16k33)));
  ESP_ERROR_CHECK(ht16k33_display_setup(&(this->ht16k33), 1, HTK16K33_F_0HZ));

  int brightness;
  esp_err_t err =
      this->nvs_manager.read_int("display", "brightness", brightness);
  if (err == ESP_OK) {
    this->set_brightness(brightness);
    ESP_LOGI(TAG, "Brightness read from NVS: %d", brightness);
  } else {
    ESP_LOGW(TAG, "Error reading brightness from NVS: %s",
             esp_err_to_name(err));
  }
}

Display::~Display() {
  memset(&(this->ht16k33), 0, sizeof(i2c_dev_t));
  ht16k33_free_desc(&(this->ht16k33));
  ESP_LOGI(TAG, "Destruct.");
}

void Display::set_brightness(uint8_t brightness) {
  if (brightness > 15) {
    brightness = 15;
  }

  this->brightness = brightness;
  this->nvs_manager.write_int("display", "brightness", brightness);

  uint8_t cmd = 0xE0 | brightness;
  esp_err_t err = i2c_master_write_to_device(
      I2C_NUM_0, this->ht16k33.addr, &cmd, 1, 1000 / portTICK_PERIOD_MS);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Setting brightness failed: %s", esp_err_to_name(err));
  }
}

void Display::set_major_interval(const std::string &major_interval) {
  int length = major_interval.length();
  if (length < 1 || length > 2) {
    ESP_LOGE(TAG,
             "Did not set major interval. Must be one or two characters long.");
    return;
  }
  strncpy(this->major_interval, major_interval.c_str(),
          sizeof(this->major_interval));
  this->major_interval[sizeof(this->major_interval) - 1] = '\0';
}

void Display::set_minor_interval(const std::string &minor_interval) {
  int length = minor_interval.length();
  if (length < 1 || length > 2) {
    ESP_LOGE(
        TAG,
        "Did not set minor interval. Must be one or two characters long..");
    return;
  }
  strncpy(this->minor_interval, minor_interval.c_str(),
          sizeof(this->minor_interval));
  this->minor_interval[sizeof(this->minor_interval) - 1] = '\0';
}

void Display::print() {
  memset(this->ht16k33_ram, 0, sizeof(this->ht16k33_ram));

  if (major_interval[1] == '\0') {
    this->ht16k33_ram[0] = 0;
    this->ht16k33_ram[2] = this->value_to_segments[major_interval[0] - '0'];
  } else {
    this->ht16k33_ram[0] = this->value_to_segments[major_interval[0] - '0'];
    this->ht16k33_ram[2] = this->value_to_segments[major_interval[1] - '0'];
  }

  if (minor_interval[1] == '\0') {
    this->ht16k33_ram[6] = 0;
    this->ht16k33_ram[8] = this->value_to_segments[minor_interval[0] - '0'];
  } else {
    this->ht16k33_ram[6] = this->value_to_segments[minor_interval[0] - '0'];
    this->ht16k33_ram[8] = this->value_to_segments[minor_interval[1] - '0'];
  }

  ESP_ERROR_CHECK(ht16k33_ram_write(&(this->ht16k33), this->ht16k33_ram));
}

void Display::count_task(void *pvParameters) {
  Display *self = static_cast<Display *>(pvParameters);

  int hours;
  int minutes;
  char hours_str[3];
  char minutes_str[3];

  while (true) {
    for (hours = 0; hours < 24; hours++) {
      for (minutes = 0; minutes < 60; minutes++) {
        snprintf(hours_str, sizeof(hours_str), "%d", hours);
        snprintf(minutes_str, sizeof(minutes_str), "%02d", minutes);

        self->set_major_interval(hours_str);
        self->set_minor_interval(minutes_str);
        self->print();

        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
  }
}

void Display::count() {
  xTaskCreate(Display::count_task, "count_task", 1024, this, 5, NULL);
}

void Display::print_current_time_task(void *pvParameters) {
  Display *self = static_cast<Display *>(pvParameters);

  std::tm time;
  char hours_str[3];
  char minutes_str[3];

  while (true) {
    time = self->current_time.get_time();

    snprintf(hours_str, sizeof(hours_str), "%d", time.tm_hour);
    snprintf(minutes_str, sizeof(minutes_str), "%02d", time.tm_min);

    self->set_major_interval(hours_str);
    self->set_minor_interval(minutes_str);
    self->print();

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void Display::print_current_time() {
  xTaskCreate(Display::print_current_time_task, "print_current_time_task", 2048,
              this, 5, NULL);
}
