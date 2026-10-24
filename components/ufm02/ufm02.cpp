
#include "ufm02.h"

#include "esphome/core/log.h"

#include <cstdint>

namespace esphome {
namespace ufm02 {

static const char *const TAG = "ufm02";

static constexpr uint8_t CMD_SYS_INIT = 0x9A;
static constexpr uint8_t CMD_RD_RAM = 0x7A;
static constexpr uint8_t CMD_WR_RAM = 0x5A;

static constexpr uint8_t REG_VOLUME_INT = 0x00;
static constexpr uint8_t REG_VOLUME_FRAC = 0x01;
static constexpr uint8_t REG_FLOW_LPH = 0x03;
static constexpr uint8_t REG_TEMPERATURE = 0x04;
static constexpr uint8_t REG_ERROR_FLAGS = 0x27;
static constexpr uint8_t REG_INTERRUPT_CLEAR = 0xDD;

static constexpr uint32_t INTERRUPT_CLEAR_ALL = 0x00000007;
static constexpr float Q16_SCALE = 65536.0f;
static constexpr double Q32_SCALE = 4294967296.0;

void UFM02Component::setup() {
  ESP_LOGCONFIG(TAG, "Initializing UFM-02");

  if (int_pin_ == nullptr) {
    ESP_LOGE(TAG, "INT pin is not configured");
    this->mark_failed();
    return;
  }

  int_pin_->setup();
  int_pin_->pin_mode(gpio::FLAG_INPUT | gpio::FLAG_PULLUP);

  this->spi_setup();

  if (!this->write_opcode_(CMD_SYS_INIT)) {
    ESP_LOGE(TAG, "Failed to send system initialization opcode");
    this->mark_failed();
    this->spi_teardown();
    return;
  }

  // Allow the sensor time to initialize.
  delay(200);

  if (!this->write_register_(REG_INTERRUPT_CLEAR, INTERRUPT_CLEAR_ALL)) {
    ESP_LOGW(TAG, "Failed to send initial interrupt-clear command");
  }

  ESP_LOGCONFIG(TAG, "UFM-02 SPI configured (mode 1, 1 MHz)");
}

bool UFM02Component::write_opcode_(uint8_t opcode) {
  this->enable();
  this->write_byte(opcode);
  this->disable();

  // ESPHome's SPI write API does not report a device acknowledgement.
  return true;
}

bool UFM02Component::read_register_(uint8_t address, uint32_t *value) {
  if (value == nullptr) {
    ESP_LOGE(TAG, "read_register_ called with a null output pointer");
    return false;
  }

  uint8_t data[4] = {0, 0, 0, 0};

this->enable();
this->write_byte(CMD_RD_RAM);
this->write_byte(address);
this->read_array(data, sizeof(data));

ESP_LOGD(TAG,
         "SPI read reg=0x%02X -> %02X %02X %02X %02X",
         address, data[0], data[1], data[2], data[3]);

this->disable();

  this->disable();

  // Assemble the four bytes in big-endian order.
  *value = (static_cast<uint32_t>(data[0]) << 24) |
           (static_cast<uint32_t>(data[1]) << 16) |
           (static_cast<uint32_t>(data[2]) << 8) |
           static_cast<uint32_t>(data[3]);

  return true;
}

bool UFM02Component::write_register_(uint8_t address, uint32_t value) {
  const uint8_t data[6] = {
      CMD_WR_RAM,
      address,
      static_cast<uint8_t>((value >> 24) & 0xFF),
      static_cast<uint8_t>((value >> 16) & 0xFF),
      static_cast<uint8_t>((value >> 8) & 0xFF),
      static_cast<uint8_t>(value & 0xFF),
  };

  this->enable();
  this->write_array(data, sizeof(data));
  this->disable();

  // ESPHome's SPI write API does not report a device acknowledgement.
  return true;
}

void UFM02Component::update() {
  if (this->is_failed()) {
    return;
  }

  uint32_t volume_int_raw = 0;
  uint32_t volume_frac_raw = 0;
  uint32_t flow_raw = 0;
  uint32_t temperature_raw = 0;
  uint32_t error_flags_raw = 0;

  // Read the documented output registers.
  if (!this->read_register_(REG_VOLUME_INT, &volume_int_raw) ||
      !this->read_register_(REG_VOLUME_FRAC, &volume_frac_raw) ||
      !this->read_register_(REG_FLOW_LPH, &flow_raw) ||
      !this->read_register_(REG_TEMPERATURE, &temperature_raw) ||
      !this->read_register_(REG_ERROR_FLAGS, &error_flags_raw)) {
    ESP_LOGW(TAG, "Unable to read UFM-02 output registers");
    return;
  }

  // FLOW_LPH and TEMPERATURE use signed Q16.16 values.
  const float flow_lph =
      static_cast<float>(static_cast<int32_t>(flow_raw)) / Q16_SCALE;

  const float temperature_c =
      static_cast<float>(static_cast<int32_t>(temperature_raw)) / Q16_SCALE;

  // Total volume is an integer m³ part plus a fractional m³ part.
  const double volume_m3 =
      static_cast<double>(static_cast<int32_t>(volume_int_raw)) +
      static_cast<double>(volume_frac_raw) / Q32_SCALE;

  const float volume_litres =
      static_cast<float>(volume_m3 * 1000.0);

  // Error flags are published as the raw bit mask.
  ESP_LOGD(TAG,
           "Flow: %.3f L/h; volume: %.6f L; temperature: %.2f °C; "
           "error flags: 0x%08X",
           flow_lph,
           volume_litres,
           temperature_c,
           static_cast<unsigned int>(error_flags_raw));

  // ESPHome flow entities are configured in L/min.
  if (flow_sensor_ != nullptr) {
    flow_sensor_->publish_state(flow_lph / 60.0f);
  }

  // No separate filtered-flow register is documented in the register
  // map used here. Publish the documented flow value as an alias rather
  // than reading an undocumented register and reporting arbitrary data.
  if (filtered_flow_sensor_ != nullptr) {
    filtered_flow_sensor_->publish_state(flow_lph / 60.0f);
  }

  if (total_volume_sensor_ != nullptr) {
    total_volume_sensor_->publish_state(volume_litres);
  }

  if (temperature_sensor_ != nullptr) {
    temperature_sensor_->publish_state(temperature_c);
  }

  if (error_flags_sensor_ != nullptr) {
    error_flags_sensor_->publish_state(
        static_cast<float>(error_flags_raw));
  }

  if (!this->write_register_(REG_INTERRUPT_CLEAR, INTERRUPT_CLEAR_ALL)) {
    ESP_LOGW(TAG, "Failed to send interrupt-clear command");
  }
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
