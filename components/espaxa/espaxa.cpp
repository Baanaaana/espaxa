#include "espaxa.h"
#include "esphome/core/log.h"

namespace esphome {
namespace espaxa {

static const char *const TAG = "espaxa";

void EspAxaComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up AXA UART...");
  this->last_read_ = 0;
}

void EspAxaComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "ESPAXA:");
  ESP_LOGCONFIG(TAG, "  Update interval: %d ms", DELAY_MS);
}

void EspAxaComponent::loop() {
  unsigned long now = millis();

  if (now - this->last_read_ > DELAY_MS || this->last_read_ == 0) {
    this->last_read_ = now;
    this->flush_uart_buffer();
    this->request_status();
    delay(100);

    if (this->available()) {
      this->read_response();
      this->update_sensor_state();
    }
  }
}

void EspAxaComponent::flush_uart_buffer() {
  while (this->available()) {
    this->read();
  }
}

void EspAxaComponent::request_status() {
  this->write_str("\r\n");
  delay(100);
  this->write_str("STATUS\r\n");
}

void EspAxaComponent::read_response() {
  int i = 0;
  while (this->available() && i < 29) {
    this->buffer_[i++] = this->read();
  }
  this->buffer_[i] = '\0';

  char *line = strtok(this->buffer_, "\n");
  bool parsed = false;

  while (line != NULL) {
    for (int j = 0; line[j] != '\0'; j++) {
      if (line[j] == '\r') {
        line[j] = '\0';
        break;
      }
    }

    if (strlen(line) >= 3 && isdigit(line[0]) && isdigit(line[1]) && isdigit(line[2])) {
      this->axa_status_ = atoi(line);
      ESP_LOGD(TAG, "Parsed AXA Status Code: %d, Line: %s", this->axa_status_, line);
      parsed = true;
      break;
    }

    line = strtok(NULL, "\n");
  }

  if (!parsed) {
    this->axa_status_ = -1;
    ESP_LOGW(TAG, "Failed to parse AXA Status Code. Full Response: %s", this->buffer_);
  }
}

void EspAxaComponent::update_sensor_state() {
  if (this->axa_status_ == AXA_STRONG_LOCKED) {
    ESP_LOGD(TAG, "Setting window state to STRONG LOCKED (CLOSED)");
    if (this->status_sensor_ != nullptr) {
      this->status_sensor_->publish_state(0);
    }
    if (this->status_text_sensor_ != nullptr) {
      this->status_text_sensor_->publish_state("Strong Locked");
    }
  } else if (this->axa_status_ == AXA_OPENED) {
    ESP_LOGD(TAG, "Setting window state to OPEN (UNLOCKED)");
    if (this->status_sensor_ != nullptr) {
      this->status_sensor_->publish_state(1);
    }
    if (this->status_text_sensor_ != nullptr) {
      this->status_text_sensor_->publish_state("Unlocked");
    }
  } else if (this->axa_status_ == AXA_WEAK_LOCKED) {
    ESP_LOGD(TAG, "Setting window state to WEAK LOCKED (CLOSED)");
    if (this->status_sensor_ != nullptr) {
      this->status_sensor_->publish_state(0);
    }
    if (this->status_text_sensor_ != nullptr) {
      this->status_text_sensor_->publish_state("Weak Locked");
    }
  } else {
    ESP_LOGD(TAG, "Unknown AXA status, state not updated.");
    if (this->status_text_sensor_ != nullptr) {
      this->status_text_sensor_->publish_state("Unknown");
    }
  }
}

}  // namespace espaxa
}  // namespace esphome