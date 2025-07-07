# ESPAXA - ESPHome Component for AXA Remote 2.0

This is an ESPHome external component for controlling AXA Remote 2.0 window openers via UART.

## Features

- Reads window status (Open/Unlocked, Strong Locked, Weak Locked)
- Provides sensor and text sensor outputs
- Compatible with ESPHome 2025.6.3+

## Installation

Add this to your ESPHome configuration:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/Baanaaana/espaxa
      ref: main
    components: [ espaxa ]
```

## Configuration

```yaml
# UART configuration
uart:
  id: uart_bus
  tx_pin: GPIO4
  rx_pin: GPIO5
  baud_rate: 19200
  stop_bits: 2

# ESPAXA component
espaxa:
  id: axa_component
  uart_id: uart_bus

# Sensors
sensor:
  - platform: espaxa
    name: "Window Status"
    id: axa_window_status
    espaxa_id: axa_component

text_sensor:
  - platform: espaxa
    name: "AXA Status"
    id: axa_status_text
    espaxa_id: axa_component
```

## Status Codes

- 210: Unlocked/Open
- 211: Strong Locked
- 212: Weak Locked

## Example Usage

See the included `esphome-axa-remote-2.yaml` for a complete example configuration.