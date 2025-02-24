#include "Display.h"
#include "CurrentTime.h"
#include "Device.h"
#include "NonVolatileStorage.h"
#include <cstring>
#include <ctime>
#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <ht16k33.h>
#include <i2cdev.h>
#include <regex>
#include <string>

Display::Display(Device &device, CurrentTime &current_time)
    : device(device), current_time(current_time), ht16k33(), ht16k33_ram(0),
      top_indicator(false), bottom_indicator(false), colon(false),
      apostrophe(false), major_interval(), minor_interval() {
  /**Initialize display structs. */
  memset(&ht16k33, 0, sizeof(i2c_dev_t));
  memset(&ht16k33_ram, 0, sizeof(ht16k33_ram));

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
  gpio_config(&io_conf);
  gpio_set_level(GPIO_NUM_2, 1);

  /**Initialize I2C and ht16k33 display drivers. */
  ESP_ERROR_CHECK(i2cdev_init());
  ESP_ERROR_CHECK(
      ht16k33_init_desc(&ht16k33, port, sda, scl, HT16K33_DEFAULT_ADDR));
  ESP_ERROR_CHECK(ht16k33_init(&ht16k33));
  ESP_ERROR_CHECK(ht16k33_display_setup(&ht16k33, 1, HTK16K33_F_0HZ));
}

Display::~Display() {
  memset(&ht16k33, 0, sizeof(i2c_dev_t));
  ht16k33_free_desc(&ht16k33);
  ESP_LOGI(TAG, "Destroy.");
}

void Display::print_9_41() {
  set_major_interval(std::string("9"));
  colon = true;
  set_minor_interval(std::string("41"));
  bottom_indicator = false;
  print();
}

void Display::print_current_time() {
  std::string hour_format, minute_format, pm_format;
  std::smatch match;

  const std::regex specifier_regex("%[a-zA-Z]");
  const std::regex colon_regex(":");
  const std::regex pm_regex("%p");

  std::string time_format = device.get_time_format();
  std::tm time = current_time.get_time();

  /**Find hour format specifier. */
  if (std::regex_search(time_format, match, specifier_regex)) {
    hour_format = match.str();
    time_format = match.suffix();
  }

  /**Check for colon. */
  if (std::regex_search(time_format, match, colon_regex)) {
    colon = true;
    time_format = match.suffix();
  } else {
    colon = false;
  }

  /**Find minute format specifier. */
  if (std::regex_search(time_format, match, specifier_regex)) {
    minute_format = match.str();
    time_format = match.suffix();
  }

  /**Check for PM indicator. */
  pm_format.clear();
  if (std::regex_search(time_format, match, pm_regex)) {
    pm_format = match.str();
    time_format = match.suffix();
  }

  char buffer[10];

  /**Set major interval. */
  if (!hour_format.empty()) {
    strftime(buffer, sizeof(buffer), hour_format.c_str(), &time);
    set_major_interval(buffer);
  }

  /**Set minor interval. */
  if (!minute_format.empty()) {
    strftime(buffer, sizeof(buffer), minute_format.c_str(), &time);
    set_minor_interval(buffer);
  }

  /**Set bottom_indicator. */
  if (!pm_format.empty()) {
    strftime(buffer, sizeof(buffer), pm_format.c_str(), &time);
    bottom_indicator = (std::string(buffer) == "PM");
  } else {
    bottom_indicator = false;
  }

  print();
}

void Display::print_countdown(Countdown &countdown) {}

const char *const Display::TAG = "display";

const std::map<const char, const uint8_t> Display::alphabet = {
    {'0', 0x3F}, {'1', 0x06}, {'2', 0x5B}, {'3', 0x4F}, {'4', 0x66},
    {'5', 0x6D}, {'6', 0x7D}, {'7', 0x07}, {'8', 0x7F}, {'9', 0x6F},
    {' ', 0x00}, {'A', 0x77}, {'B', 0x7C}, {'C', 0x39}, {'D', 0x5E},
    {'E', 0x79}, {'F', 0x71}, {'G', 0x3D}, {'H', 0x76}, {'I', 0x30},
    {'J', 0x1E}, {'K', 0x75}, {'L', 0x38}, {'M', 0x15}, {'N', 0x37},
    {'O', 0x3F}, {'P', 0x73}, {'Q', 0x6B}, {'R', 0x33}, {'S', 0x6D},
    {'T', 0x78}, {'U', 0x3E}, {'V', 0x3E}, {'W', 0x2A}, {'X', 0x76},
    {'Y', 0x6E}, {'Z', 0x5B},
};

void Display::set_major_interval(const std::string &major_interval) {
  int length = major_interval.length();
  if (length < 1 || length > 2) {
    ESP_LOGE(TAG,
             "Did not set major interval. Must be one or two characters long.");
    return;
  }
  this->major_interval = major_interval;
}

void Display::set_minor_interval(const std::string &minor_interval) {
  int length = minor_interval.length();
  if (length < 1 || length > 2) {
    ESP_LOGE(TAG,
             "Did not set minor interval. Must be one or two characters long.");
    return;
  }
  this->minor_interval = minor_interval;
}

void Display::set_brightness(uint8_t brightness) {
  uint8_t cmd = 0xE0 | brightness;
  esp_err_t err = i2c_master_write_to_device(I2C_NUM_0, ht16k33.addr, &cmd, 1,
                                             1000 / portTICK_PERIOD_MS);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error setting brightness: %s", esp_err_to_name(err));
  }
}

void Display::print() {
  memset(ht16k33_ram, 0, sizeof(ht16k33_ram));

  set_brightness(device.get_brightness());

  if (major_interval.length() > 0) {
    if (major_interval[1] == '\0') {
      ht16k33_ram[0] = alphabet.at(' ');
      ht16k33_ram[2] = alphabet.at(major_interval[0]);
    } else {
      ht16k33_ram[0] = alphabet.at(major_interval[0]);
      ht16k33_ram[2] = alphabet.at(major_interval[1]);
    }
  }

  if (minor_interval.length() > 0) {
    if (minor_interval[1] == '\0') {
      ht16k33_ram[6] = alphabet.at(' ');
      ht16k33_ram[8] = alphabet.at(minor_interval[0]);
    } else {
      ht16k33_ram[6] = alphabet.at(minor_interval[0]);
      ht16k33_ram[8] = alphabet.at(minor_interval[1]);
    }
  }

  uint8_t indicators = 0b00000;

  /**Indicators are set by these bits:
   * 0b00000
   *   ││││└─ no effect
   *   │││└── colon
   *   ││└─── top_indicator
   *   │└──── bottom_indicator
   *   └───── apostrophe */

  if (apostrophe) {
    indicators += 0b10000;
  }
  if (bottom_indicator) {
    indicators += 0b01000;
  }
  if (top_indicator) {
    indicators += 0b00100;
  }
  if (colon) {
    indicators += 0b00010;
  }

  ht16k33_ram[4] = indicators;

  ESP_ERROR_CHECK(ht16k33_ram_write(&ht16k33, ht16k33_ram));
}
