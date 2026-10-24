
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace ufm02 {

class UFM02Component
    : public PollingComponent,
      public spi::SPIDevice<
          spi::BIT_ORDER_MSB_FIRST,
          spi::CLOCK_POLARITY_LOW,
          spi::CLOCK_PHASE_TRAILING,
          spi::DATA_RATE_1MHZ> {
 public:
  UFM02Component() : PollingComponent(100) {}

  void set_int_pin(InternalGPIOPin *pin) {
    int_pin_ = pin;
  }

  void set_flow_sensor(sensor::Sensor *sensor) {
    flow_sensor_ = sensor;
  }

  void set_filtered_flow_sensor(sensor::Sensor *sensor) {
    filtered_flow_sensor_ = sensor;
  }

  void set_total_volume_sensor(sensor::Sensor *sensor) {
    total_volume_sensor_ = sensor;
  }

  void set_temperature_sensor(sensor::Sensor *sensor) {
    temperature_sensor_ = sensor;
  }

  void set_error_flags_sensor(sensor::Sensor *sensor) {
    error_flags_sensor_ = sensor;
  }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  InternalGPIOPin *int_pin_{nullptr};

  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *filtered_flow_sensor_{nullptr};
  sensor::Sensor *total_volume_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *error_flags_sensor_{nullptr};
  bool read_register_(uint8_t address, uint32_t *value);
  bool write_register_(uint8_t address, uint32_t value);
  bool write_opcode_(uint8_t opcode);
};

}  // namespace ufm02
}  // namespace esphome
