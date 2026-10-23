#include "ufm02.h"

#include "esphome/core/log.h"

#include <Arduino.h>
#include <SPI.h>
#include <inttypes.h>

namespace esphome {
namespace ufm02 {

static const char *const TAG = "ufm02";

void UFM02Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Initializing UFM-02");

  pinMode(cs_pin_, OUTPUT);
  digitalWrite(cs_pin_, HIGH);

  pinMode(int_pin_, INPUT_PULLUP);

  SPI.begin(sck_pin_, miso_pin_, mosi_pin_, cs_pin_);

  ESP_LOGCONFIG(TAG, "SPI initialized");
  ESP_LOGCONFIG(TAG, "CS GPIO: %d", cs_pin_);
  ESP_LOGCONFIG(TAG, "SCK GPIO: %d", sck_pin_);
  ESP_LOGCONFIG(TAG, "MOSI GPIO: %d", mosi_pin_);
  ESP_LOGCONFIG(TAG, "MISO GPIO: %d", miso_pin_);
  ESP_LOGCONFIG(TAG, "INT GPIO: %d", int_pin_);
}

void UFM02Sensor::update() {
  const bool interrupt_active = digitalRead(int_pin_) == LOW;

  ESP_LOGD(TAG, "Interrupt active: %s",
           interrupt_active ? "yes" : "no");

  // TODO:
  // Read the UFM-02 measurement registers using the exact API/protocol
  // supported by the installed ScioSense_UFM02 library.
  //
  // Publish values only after obtaining valid measurements:
  //   flow_sensor_->publish_state(flow);
  //   filtered_flow_sensor_->publish_state(filtered_flow);
  //   total_volume_sensor_->publish_state(total_volume);
  //   temperature_sensor_->publish_state(temperature);
  //   error_flags_sensor_->publish_state(error_flags);
}

void UFM02Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "UFM-02:");
  ESP_LOGCONFIG(TAG, "  CS GPIO: %d", cs_pin_);
  ESP_LOGCONFIG(TAG, "  SCK GPIO: %d", sck_pin_);
  ESP_LOGCONFIG(TAG, "  MOSI GPIO: %d", mosi_pin_);
  ESP_LOGCONFIG(TAG, "  MISO GPIO: %d", miso_pin_);
  ESP_LOGCONFIG(TAG, "  INT GPIO: %d", int_pin_);

  LOG_SENSOR("  Flow: ", flow_sensor_);
  LOG_SENSOR("  Filtered flow: ", filtered_flow_sensor_);
  LOG_SENSOR("  Total volume: ", total_volume_sensor_);
  LOG_SENSOR("  Temperature: ", temperature_sensor_);
  LOG_SENSOR("  Error flags: ", error_flags_sensor_);
}

}  // namespace ufm02
}  // namespace esphome