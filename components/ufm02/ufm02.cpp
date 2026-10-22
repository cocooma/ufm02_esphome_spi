
#include "ufm02.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ufm02 {

static const char *const TAG = "ufm02";

// UFM-02 SPI RAM/register read command.
// Verify the complete protocol against the vendor driver.
static constexpr uint8_t CMD_RD_RAM = 0x7A;

void UFM02Sensor::setup() {
  if (this->int_pin_ == nullptr) {
    ESP_LOGE(TAG, "INT pin is not configured");
    this->mark_failed();
    return;
  }

  this->int_pin_->setup();
  this->int_pin_->pin_mode(gpio::FLAG_INPUT);

  ESP_LOGI(TAG, "UFM-02 SPI component initialized");
  ESP_LOGI(TAG, "Register address: 0x%02X",
           this->register_address_);
}

void UFM02Sensor::update() {
  // INTN is an active-low interrupt signal.
  // Avoid reading the device when no interrupt is pending.
  if (this->int_pin_->digital_read()) {
    ESP_LOGV(TAG, "No active INTN signal");
    return;
  }

  uint8_t data[4] = {0, 0, 0, 0};

  this->enable();

  this->write_byte(CMD_RD_RAM);
  this->write_byte(this->register_address_);
  this->read_array(data, sizeof(data));

  this->disable();

  // Assemble the four returned bytes as a big-endian
  // unsigned 32-bit value. Confirm byte order against
  // the register definition before using this as a
  // physical measurement.
  uint32_t raw =
      (static_cast<uint32_t>(data[0]) << 24) |
      (static_cast<uint32_t>(data[1]) << 16) |
      (static_cast<uint32_t>(data[2]) << 8) |
      static_cast<uint32_t>(data[3]);

  ESP_LOGD(TAG, "Raw register: 0x%08X (%u)",
           raw, static_cast<unsigned int>(raw));

  this->publish_state(static_cast<float>(raw));
}

void UFM02Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "ScioSense UFM-02:");
  LOG_PIN("  INT Pin: ", this->int_pin_);
  ESP_LOGCONFIG(TAG, "  Register: 0x%02X",
                this->register_address_);
  LOG_SENSOR("  ", "Raw Register", this);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
}

}  // namespace ufm02
}  // namespace esphome
