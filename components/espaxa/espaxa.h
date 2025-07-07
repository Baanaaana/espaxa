#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace espaxa {

#define DELAY_MS 5000
#define DEVICE_INFO_DELAY_MS 10000
#define VERSION_DELAY_MS 15000
#define INFO_RETRY_INTERVAL_MS 60000
#define MAX_RETRY_COUNT 3
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
  void set_device_info_text_sensor(text_sensor::TextSensor *sensor) { this->device_info_text_sensor_ = sensor; }
  void set_version_text_sensor(text_sensor::TextSensor *sensor) { this->version_text_sensor_ = sensor; }

 protected:
  void flush_uart_buffer();
  void request_status();
  void request_device_info();
  void request_version();
  void read_response();
  void update_sensor_state();

  unsigned long last_read_{0};
  unsigned long last_device_info_request_{0};
  unsigned long last_version_request_{0};
  char buffer_[30];
  int axa_status_{-1};
  bool device_info_requested_{false};
  bool version_requested_{false};
  bool expecting_device_info_{false};
  bool expecting_version_{false};
  bool device_info_success_{false};
  bool version_success_{false};
  uint8_t device_info_retry_count_{0};
  uint8_t version_retry_count_{0};
  
  sensor::Sensor *status_sensor_{nullptr};
  text_sensor::TextSensor *status_text_sensor_{nullptr};
  text_sensor::TextSensor *device_info_text_sensor_{nullptr};
  text_sensor::TextSensor *version_text_sensor_{nullptr};
};

}  // namespace espaxa
}  // namespace esphome