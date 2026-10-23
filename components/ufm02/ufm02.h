#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include <cstdint>

namespace esphome {
namespace ufm02 {

class UFM02Sensor : public PollingComponent {
 public:
  explicit UFM02Sensor() : PollingComponent(100) {}

  void set_cs_pin(int pin) { cs_pin_ = pin; }
  void set_sck_pin(int pin) { sck_pin_ = pin; }
  void set_mosi_pin(int pin) { mosi_pin_ = pin; }
  void set_miso_pin(int pin) { miso_pin_ = pin; }
  void set_int_pin(int pin) { int_pin_ = pin; }

  void set_flow_sensor(sensor::Sensor *sensor) { flow_sensor_ = sensor; }
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
  int cs_pin_{5};
  int sck_pin_{18};
  int mosi_pin_{23};
  int miso_pin_{19};
  int int_pin_{2};

  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *filtered_flow_sensor_{nullptr};
  sensor::Sensor *total_volume_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *error_flags_sensor_{nullptr};
};

}  // namespace ufm02
}  // namespace esphome