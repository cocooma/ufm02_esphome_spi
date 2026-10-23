
#include "ufm02.h"

#include "esphome/core/log.h"

namespace esphome {
namespace ufm02 {

static const char *const TAG = "ufm02";

void UFM02Component::setup() {
  ESP_LOGCONFIG(TAG, "Initializing ScioSense UFM-02");

  if (int_pin_ == nullptr) {
    ESP_LOGE(TAG, "INT pin is not configured");
    this->mark_failed();
    return;
  }

  int_pin_->setup();
  int_pin_->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);

  ESP_LOGCONFIG(TAG, "UFM-02 interrupt pin initialized");
  ESP_LOGCONFIG(TAG, "SPI mode: 1");
  ESP_LOGCONFIG(TAG, "SPI clock: 1 MHz");
}

void UFM02Component::update() {
  if (this->is_failed()) {
    return;
  }

  const bool interrupt_active = !int_pin_->digital_read();

  ESP_LOGD(TAG, "Interrupt active: %s",
           interrupt_active ? "yes" : "no");

  /*
   * TODO: Implement UFM-02 register reads and decoding using the
   * ScioSense UFM-02 protocol/library.
   *
   * Once valid measurements have been decoded, publish them:
   *
   * if (flow_sensor_ != nullptr)
   *   flow_sensor_->publish_state(flow);
   *
   * if (filtered_flow_sensor_ != nullptr)
   *   filtered_flow_sensor_->publish_state(filtered_flow);
   *
   * if (total_volume_sensor_ != nullptr)
   *   total_volume_sensor_->publish_state(total_volume);
   *
   * if (temperature_sensor_ != nullptr)
   *   temperature_sensor_->publish_state(temperature);
   *
   * if (error_flags_sensor_ != nullptr)
   *   error_flags_sensor_->publish_state(error_flags);
   */
}

void UFM02Component::dump_config() {
  ESP_LOGCONFIG(TAG, "ScioSense UFM-02:");
  ESP_LOGCONFIG(TAG, "  SPI mode: 1");
  ESP_LOGCONFIG(TAG, "  SPI clock: 1 MHz");

  LOG_PIN("  INT pin: ", int_pin_);

  LOG_SENSOR("  Flow: ", "Flow", flow_sensor_);
  LOG_SENSOR("  Filtered flow: ", "Filtered Flow", filtered_flow_sensor_);
  LOG_SENSOR("  Total volume: ", "Total Volume", total_volume_sensor_);
  LOG_SENSOR("  Temperature: ", "Temperature", temperature_sensor_);
  LOG_SENSOR("  Error flags: ", "Error Flags", error_flags_sensor_);
}

}  // namespace ufm02
}  // namespace esphome
