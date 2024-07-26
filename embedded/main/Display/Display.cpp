#include "Display.h"
#include "CurrentTime.h"
#include "NvsManager.h"
#include <cstring>
#include <ctime>
#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <ht16k33.h>
#include <i2cdev.h>
#include <regex>
#include <string>

static const char *TAG = "Display";

Display::Display(NvsManager &nvs_manager, CurrentTime &current_time)
    : nvs_manager(nvs_manager), current_time(current_time),
      top_indicator(false), bottom_indicator(false), colon(false),
      apostrophe(false) {
  /**Initialize display structs. */
  memset(&(this->ht16k33), 0, sizeof(i2c_dev_t));
  memset(&(this->ht16k33_ram), 0, sizeof(ht16k33_ram));

  /**I2C communication pin definitions. */
  gpio_num_t sda = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SDA);
  gpio_num_t scl = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SCL);
  i2c_port_t port = static_cast<i2c_port_t>(0);

  /**Configure GPIO2 (I2C power) and set high.
   * https://learn.adafruit.com/adafruit-esp32-feather-v2/pinouts#stemma-qt-connector-3112257
   */
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = (1ULL << GPIO_NUM_2);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);
  gpio_set_level(GPIO_NUM_2, 1);

  /**Initialize I2C and ht16k33 display drivers. */
  ESP_ERROR_CHECK(i2cdev_init());
  ESP_ERROR_CHECK(ht16k33_init_desc(&(this->ht16k33), port, sda, scl,
                                    HT16K33_DEFAULT_ADDR));
  ESP_ERROR_CHECK(ht16k33_init(&(this->ht16k33)));
  ESP_ERROR_CHECK(ht16k33_display_setup(&(this->ht16k33), 1, HTK16K33_F_0HZ));

  int brightness;
  esp_err_t err =
      this->nvs_manager.read_int("display", "brightness", brightness);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Brightness read from NVS: %d", brightness);
    this->set_brightness(brightness);
  } else {
    ESP_LOGW(TAG, "Error reading brightness from NVS: %s. Using default.",
             esp_err_to_name(err));
    this->set_brightness(7);
  }
}

Display::~Display() {
  memset(&(this->ht16k33), 0, sizeof(i2c_dev_t));
  ht16k33_free_desc(&(this->ht16k33));
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
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Set brightness: %d", brightness);
  } else {
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

  if (this->major_interval[1] == '\0') {
    this->ht16k33_ram[0] = 0;
    this->ht16k33_ram[2] = this->value_to_segments[major_interval[0] - '0'];
  } else {
    this->ht16k33_ram[0] = this->value_to_segments[major_interval[0] - '0'];
    this->ht16k33_ram[2] = this->value_to_segments[major_interval[1] - '0'];
  }

  if (this->minor_interval[1] == '\0') {
    this->ht16k33_ram[6] = 0;
    this->ht16k33_ram[8] = this->value_to_segments[minor_interval[0] - '0'];
  } else {
    this->ht16k33_ram[6] = this->value_to_segments[minor_interval[0] - '0'];
    this->ht16k33_ram[8] = this->value_to_segments[minor_interval[1] - '0'];
  }

  uint8_t indicators = 0b00000;

  /**Indicators correspond to the following bits:
   * 0b00000
   *   ││││└─ no effect
   *   │││└── colon
   *   ││└─── top_indicator
   *   │└──── bottom_indicator
   *   └───── apostrophe */

  if (this->apostrophe) {
    indicators += 0b10000;
  }
  if (this->bottom_indicator) {
    indicators += 0b01000;
  }
  if (this->top_indicator) {
    indicators += 0b00100;
  }
  if (this->colon) {
    indicators += 0b00010;
  }

  this->ht16k33_ram[4] = indicators;

  ESP_ERROR_CHECK(ht16k33_ram_write(&(this->ht16k33), this->ht16k33_ram));
}

void Display::print_current_time_task(void *pvParameters) {
  Display *self = static_cast<Display *>(pvParameters);

  std::tm time;
  std::string format;
  std::smatch match;

  const std::regex specifier_regex("%[a-zA-Z]");
  const std::regex colon_regex(":");
  const std::regex pm_regex("%p");

  std::string hour_format, minute_format, pm_format;

  while (true) {
    time = self->current_time.get_time();
    format = self->current_time.get_format();

    /**Find hour format specifier. */
    if (std::regex_search(format, match, specifier_regex)) {
      hour_format = match.str();
      format = match.suffix();
    }

    /**Check for colon. */
    if (std::regex_search(format, match, colon_regex)) {
      self->colon = true;
      format = match.suffix();
    } else {
      self->colon = false;
    }

    /**Find minute format specifier. */
    if (std::regex_search(format, match, specifier_regex)) {
      minute_format = match.str();
      format = match.suffix();
    }

    /**Check for PM indicator. */
    pm_format.clear();
    if (std::regex_search(format, match, pm_regex)) {
      pm_format = match.str();
      format = match.suffix();
    }

    char buffer[10];

    /**Set major interval. */
    if (!hour_format.empty()) {
      strftime(buffer, sizeof(buffer), hour_format.c_str(), &time);
      self->set_major_interval(buffer);
    }

    /**Set minor interval. */
    if (!minute_format.empty()) {
      strftime(buffer, sizeof(buffer), minute_format.c_str(), &time);
      self->set_minor_interval(buffer);
    }

    /**Set bottom_indicator. */
    if (!pm_format.empty()) {
      strftime(buffer, sizeof(buffer), pm_format.c_str(), &time);
      self->bottom_indicator = (std::string(buffer) == "PM");
    } else {
      self->bottom_indicator = false;
    }

    self->print();

    vTaskDelay(pdMS_TO_TICKS(100));
  }

  vTaskDelete(NULL);
}

void Display::print_current_time() {
  xTaskCreate(Display::print_current_time_task, "print_current_time", 4096,
              this, 5, NULL);
}

void Display::print_9_41() {
  this->set_major_interval("9");
  this->colon = true;
  this->set_minor_interval("41");
  this->bottom_indicator = false;
  this->print();
}
