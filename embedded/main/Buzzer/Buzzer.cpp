#include "Buzzer.h"
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/idf_additions.h>

static const char *TAG = "Buzzer";

#define BUZZER_TIMER LEDC_TIMER_0
#define BUZZER_CHANNEL LEDC_CHANNEL_0
#define BUZZER_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_DUTY (4096)
#define BUZZER_FREQUENCY (440)
#define BUZZER_OUTPUT_IO (26)

Buzzer::Buzzer() : is_buzzing(false) {
  this->buzzer_timer = {
      .speed_mode = BUZZER_MODE,
      .duty_resolution = LEDC_TIMER_13_BIT,
      .timer_num = BUZZER_TIMER,
      .freq_hz = BUZZER_FREQUENCY,
      .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&buzzer_timer));

  this->buzzer_channel = {
      .gpio_num = BUZZER_OUTPUT_IO,
      .speed_mode = BUZZER_MODE,
      .channel = BUZZER_CHANNEL,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = BUZZER_TIMER,
      .duty = 0,
      .hpoint = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&buzzer_channel));
};

Buzzer::~Buzzer() {}

void Buzzer::buzz_task(void *pvParameters) {
  Buzzer *self = static_cast<Buzzer *>(pvParameters);

  while (self->is_buzzing) {
    ESP_ERROR_CHECK(ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, BUZZER_DUTY));
    ESP_ERROR_CHECK(ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_ERROR_CHECK(ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0));
    ESP_ERROR_CHECK(ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL));
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  vTaskDelete(NULL);
}

void Buzzer::start_buzzing() {
  this->is_buzzing = true;
  xTaskCreate(buzz_task, "buzz", 2048, this, 5, NULL);
  ESP_LOGI(TAG, "Started buzzing.");
}

void Buzzer::stop_buzzing() {
  this->is_buzzing = false;
  ESP_LOGI(TAG, "Stopped buzzing.");
}
