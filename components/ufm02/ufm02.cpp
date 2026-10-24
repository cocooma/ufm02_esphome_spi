
#include "ufm02.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ufm02 {

static const char *const TAG = "ufm02";

void UFM02Component::setup() {
  pinMode(this->cs_pin_, OUTPUT);
  digitalWrite(this->cs_pin_, HIGH);
  pinMode(this->int_pin_, INPUT);

  SPI.begin(
      this->clk_pin_,
      this->miso_pin_,
      this->mosi_pin_,
      this->cs_pin_
  );

  this->ufm02_.begin(
      &SPI,
      this->cs_pin_,
      SPISettings(14000000, MSBFIRST, SPI_MODE1)
  );

  if (!this->ufm02_.init()) {
    ESP_LOGE(TAG, "UFM02 initialization failed: sensor type not recognized");
    this->mark_failed();
    return;
  }

  this->initialized_ = true;

  ESP_LOGI(
      TAG,
      "UFM02 initialized; material ID: %u",
      this->ufm02_.getMaterialId()
  );
}

void UFM02Component::update() {
  if (!this->initialized_) {
    return;
  }

  // The official example updates the sensor when INT is LOW.
  if (digitalRead(this->int_pin_) != LOW) {
    return;
  }

  this->ufm02_.update();

  // Do not publish measurements flagged as invalid or containing bubbles.
  const bool measurement_error =
      this->ufm02_.hasErrorFlag(UFM02_ERROR_FLAG_MEAS_NOT_OK);

  const bool bubble_error =
      this->ufm02_.hasErrorFlag(UFM02_ERROR_FLAG_BUBBLES);

  if (measurement_error || bubble_error) {
    ESP_LOGW(TAG, "UFM02 reported a measurement error or bubbles");
    return;
  }

  if (this->flow_sensor_ != nullptr) {
    this->flow_sensor_->publish_state(
        this->ufm02_.getInstantFlowLPerHr()
    );
  }

  if (this->total_volume_sensor_ != nullptr) {
    this->total_volume_sensor_->publish_state(
        this->ufm02_.getVolumeM3()
    );
  }

  if (this->temperature_sensor_ != nullptr) {
    this->temperature_sensor_->publish_state(
        this->ufm02_.getTemperatureDegC()
    );
  }
}

void UFM02Component::dump_config() {
  ESP_LOGCONFIG(TAG, "UFM02 flow meter:");
  ESP_LOGCONFIG(TAG, "  CS pin: GPIO%d", this->cs_pin_);
  ESP_LOGCONFIG(TAG, "  INT pin: GPIO%d", this->int_pin_);
  ESP_LOGCONFIG(TAG, "  CLK pin: GPIO%d", this->clk_pin_);
  ESP_LOGCONFIG(TAG, "  MISO pin: GPIO%d", this->miso_pin_);
  ESP_LOGCONFIG(TAG, "  MOSI pin: GPIO%d", this->mosi_pin_);
  LOG_SENSOR("  ", "Flow rate", this->flow_sensor_);
  LOG_SENSOR("  ", "Total volume", this->total_volume_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

}  // namespace ufm02
}  // namespace esphome
