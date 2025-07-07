based on galagaking/espaxa. Thanks for your work!

# ESP-AXA - For ESPHome and Home Assistant
![AXA2](https://github.com/Baanaaana/espaxa/blob/main/docs/IMG_0939.jpeg)
This module will connect the AXA Remote 2.0 with your Home Assistant Server using ESPHOME. The module will integrate in Home Assistant, showing the position of your Window (open or closed), and give you the possibility to open and close the window from there. The remote control will still work, the status of the window will synchronize with home assistant.
## Power
The module is using a 5V power supply, and I use a DC-DC step-up converter (MT3608) to create the 8V (7.5 with some cable loss) for the AXA Remote from the 5V power supply. So, NO batteries needed in your AXA Remote (Remove these before plugging in the connector)! The Power Supply should have enough capacity to handle the motors inside the AXA. Preferable is a Raspberry Pi supply (model 3, 2.5A or more). The power can be connected via the WEMOS USB plug, or directly to the 5V and GND pin of the WEMOS. The DC-DC Converter should be adjusted to 8V, using a multimeter. You can plug in the power adapter to this module as well for delivering power.
## PCB
https://oshwlab.com/rene4/axa-remote

![3D_PCB](https://github.com/Baanaaana/espaxa/blob/main/docs/3D_PCB.png)
## Reference
- [Data sheet MCP 2003](http://ww1.microchip.com/downloads/en/devicedoc/22230a.pdf)
- [AXA Documentation](http://files.domoticaforum.eu/uploads/Axa/AXA%20Remote%202%20domotica%20English%202012nov_V2.pdf)
## Programming
We use ESPHome to create the firmware. This is more like a configuration than a program. [esphome-axa-remote.yaml](https://github.com/baanaaana/espaxa/blob/main/esphome-axa-remote.yaml) has all the definitions and is also referring to some program code in AXA/espaxa.h. You must create this directory and put the [expaxa.h](https://github.com/baanaaana/espaxa/blob/main/AXA/espaxa.h) file in it. Also fill in the WiFi credentials (or better: use the secrets file). More info on ESPHome
[here](https://esphome.io/) or [here](https://www.galagaking.org/2019/11/05/esphome-workshop/). To start, connect the ESPAXA (without connecting the AXA Remote) directly to your USB port to upload the sketch the first time. Now OTA is installed, you can upload any new sketch 'Over the Air'. ESPHome will try to find the module itself. If not, you must manage your DHCP server and 'hardcode' an IP address. The 'Closing' sequence starts with a short 'Open' command. It seems the AXA Remote will 'block' at the end position. By sending an 'Open' command before closing, the unit will 'deblock'.
## Connecting
Use a RJ12 connecter to connect the module to the AXA remote. Use the connection in the schema. Remove the batteries in the AXA Remote!

![Connector](https://github.com/Baanaaana/espaxa/blob/main/docs/20200831_155821.jpg)
## Features

- Reads window status (Open/Unlocked, Strong Locked, Weak Locked)
- Provides sensor and text sensor outputs
- Device info and version information retrieval
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

Complete configuration example:

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
    espaxa_id: axa_component
    type: status
    
  - platform: espaxa
    name: "AXA Device Info"
    espaxa_id: axa_component
    type: device_info
    
  - platform: espaxa
    name: "AXA Version"
    espaxa_id: axa_component
    type: version
```

## Text Sensor Types

The component supports three types of text sensors:

- **status**: Shows the current lock state (Strong Locked, Weak Locked, Unlocked)
- **device_info**: Shows device type information (e.g., "AXA RV2900 2.0")
- **version**: Shows firmware version of the AXA Remote

## Status Codes

- 210: Unlocked/Open
- 211: Strong Locked
- 212: Weak Locked

## Available Commands

The component uses these AXA Remote commands:
- `DEVICE` - Returns device type information
- `VERSION` - Returns firmware version
- `STATUS` - Returns current lock state
- `OPEN` - Opens the window opener
- `CLOSE` - Closes the window opener
- `STOP` - Stops the window opener

## Home Assistant
The ESP module will show up as a new integration. After enabling this integration, you can add the entities given in your espaxa yaml file in your interface. The objects will already use their default MDI picture.
![Home Assitant](https://github.com/Baanaaana/espaxa/blob/main/docs/homeassistant_esphome.png)
## Thank you
For inspiration and examples:
- [SuperHouse Automation](https://github.com/SuperHouse/)
- [nldroid CustomP1UartComponent](https://github.com/nldroid/CustomP1UartComponent)
## disclaimer
Use this project as-is, I am not responsible for any unexpected open window at your home, or any damage to any hardware. This is a creative commons project, for non-commercial use only.
