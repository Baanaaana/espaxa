#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace espaxa {

#define DELAY_MS 5000
#define AXA_OPENED 210
#define AXA_STRONG_LOCKED 211
#define AXA_WEAK_LOCKED 212

class EspAxaComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_status_sensor(sensor::Sensor *sensor) { this->status_sensor_ = sensor; }
  void set_status_text_sensor(text_sensor::TextSensor *sensor) { this->status_text_sensor_ = sensor; }

 protected:
  void flush_uart_buffer();
  void request_status();
  void read_response();
  void update_sensor_state();

  unsigned long last_read_{0};
  char buffer_[30];
  int axa_status_{-1};
  
  sensor::Sensor *status_sensor_{nullptr};
  text_sensor::TextSensor *status_text_sensor_{nullptr};
};

}  // namespace espaxa
}  // namespace esphome