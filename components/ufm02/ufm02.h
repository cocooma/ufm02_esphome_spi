
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

#include <Arduino.h>
#include <SPI.h>
#include <ScioSense_UFM02.h>

namespace esphome {
namespace ufm02 {

class UFM02Component : public PollingComponent {
 public:
  void set_cs_pin(int pin) { cs_pin_ = pin; }
  void set_int_pin(int pin) { int_pin_ = pin; }
  void set_clk_pin(int pin) { clk_pin_ = pin; }
  void set_miso_pin(int pin) { miso_pin_ = pin; }
  void set_mosi_pin(int pin) { mosi_pin_ = pin; }

  void set_flow_sensor(sensor::Sensor *sensor) {
    flow_sensor_ = sensor;
  }

  void set_total_volume_sensor(sensor::Sensor *sensor) {
    total_volume_sensor_ = sensor;
  }

  void set_temperature_sensor(sensor::Sensor *sensor) {
    temperature_sensor_ = sensor;
  }

  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  int cs_pin_{5};
  int int_pin_{2};
  int clk_pin_{18};
  int miso_pin_{19};
  int mosi_pin_{23};

  bool initialized_{false};

  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *total_volume_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};

  UFM02 ufm02_;
};

}  // namespace ufm02
}  // namespace esphome
