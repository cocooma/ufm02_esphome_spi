
# UFM-02 ESPHome component

External ESPHome component for the ScioSense UFM-02 ultrasonic
flow sensor.

## Wiring

| UFM-02 | ESP32 example |
|---|---|
| VCC | 5 V |
| GND | GND |
| CS | GPIO5 |
| MO / MOSI | GPIO23 |
| SCK | GPIO18 |
| MI / MISO | GPIO19 |
| INT | GPIO2 |

The sensor supply is 5 V. Its I/O operates at 3.3 V.

## Installation

Place the `components/ufm02` directory beside the ESPHome
YAML file and configure it as a local external component.

## Configuration

Configure `cs_pin`, `clk_pin`, `mosi_pin`, `miso_pin`, and
`int_pin` separately. Set `register_address` to a verified
address from the ScioSense driver or datasheet.

## Current limitations

This implementation is a register-read scaffold. It does not
implement the full UFM-02 initialization, measurement decoding,
flow-unit conversion, or error recovery.

Before using it for actual measurements, port and verify the
initialization and decoding logic from:
https://github.com/sciosense/ufm02-arduino

Compile and test on the target ESP32 before relying on readings.
