
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace ufm02 {

class UFM02Sensor
    : public PollingComponent,
      public spi::SPIDevice<
          spi::BIT_ORDER_MSB_FIRST,
          spi::CLOCK_POLARITY_LOW,
          spi::CLOCK_PHASE_LEADING,
          spi::DATA_RATE_1KHZ>,
      public sensor::Sensor {
 public:
  void set_int_pin(InternalGPIOPin *pin) {
    this->int_pin_ = pin;
  }

  void set_register_address(uint8_t address) {
    this->register_address_ = address;
  }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  InternalGPIOPin *int_pin_{nullptr};
  uint8_t register_address_{0};
};

}  // namespace ufm02
}  // namespace esphome
