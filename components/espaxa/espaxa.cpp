#include "espaxa.h"
#include "esphome/core/log.h"

namespace esphome {
namespace espaxa {

static const char *const TAG = "espaxa";

void EspAxaComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up AXA UART...");
  this->last_read_ = 0;
  this->device_info_requested_ = false;
  this->version_requested_ = false;
  this->expecting_device_info_ = false;
  this->expecting_version_ = false;
  
  // Initialize sensors with empty states to prevent initial type name publishing
  if (this->device_info_text_sensor_ != nullptr) {
    this->device_info_text_sensor_->publish_state("");
  }
  if (this->version_text_sensor_ != nullptr) {
    this->version_text_sensor_->publish_state("");
  }
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
    
    if (!this->device_info_requested_ && this->device_info_text_sensor_ != nullptr) {
      this->request_device_info();
      this->device_info_requested_ = true;
      this->expecting_device_info_ = true;
    } else if (!this->version_requested_ && this->version_text_sensor_ != nullptr) {
      this->request_version();
      this->version_requested_ = true;
      this->expecting_version_ = true;
    } else {
      this->request_status();
    }
    
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

void EspAxaComponent::request_device_info() {
  this->write_str("\r\n");
  delay(100);
  this->write_str("DEVICE\r\n");
}

void EspAxaComponent::request_version() {
  ESP_LOGD(TAG, "Requesting VERSION command");
  this->write_str("\r\n");
  delay(100);
  this->write_str("VERSION\r\n");
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

    if (strlen(line) > 0) {
      if (this->expecting_device_info_ && this->device_info_text_sensor_ != nullptr) {
        // Skip command echo
        if (strcmp(line, "DEVICE") == 0) {
          ESP_LOGD(TAG, "Skipping DEVICE command echo");
        } else if (strstr(line, "502") != nullptr) {
          this->device_info_text_sensor_->publish_state("Not Supported");
          ESP_LOGD(TAG, "Device Info: Not Supported (502)");
          this->expecting_device_info_ = false;
          parsed = true;
        } else if (strstr(line, "260") != nullptr) {
          // Extract device info from "260 AXA RV2900 2.0"
          char* device_start = strchr(line, ' ');
          if (device_start != nullptr) {
            device_start++; // Skip the space
            this->device_info_text_sensor_->publish_state(device_start);
            ESP_LOGD(TAG, "Device Info: %s", device_start);
          } else {
            this->device_info_text_sensor_->publish_state(line);
            ESP_LOGD(TAG, "Device Info: %s", line);
          }
          this->expecting_device_info_ = false;
          parsed = true;
        } else {
          this->device_info_text_sensor_->publish_state(line);
          ESP_LOGD(TAG, "Device Info: %s", line);
          this->expecting_device_info_ = false;
          parsed = true;
        }
      } else if (this->expecting_version_ && this->version_text_sensor_ != nullptr) {
        // Skip command echo
        if (strcmp(line, "VERSION") == 0) {
          ESP_LOGD(TAG, "Skipping VERSION command echo");
        } else if (strstr(line, "261") != nullptr) {
          // Extract version from "261 Firmware V1.20"
          char* version_start = strchr(line, ' ');
          if (version_start != nullptr) {
            version_start++; // Skip the space
            this->version_text_sensor_->publish_state(version_start);
            ESP_LOGD(TAG, "Version: %s", version_start);
          } else {
            this->version_text_sensor_->publish_state(line);
            ESP_LOGD(TAG, "Version: %s", line);
          }
          this->expecting_version_ = false;
          parsed = true;
        } else {
          this->version_text_sensor_->publish_state(line);
          ESP_LOGD(TAG, "Version: %s", line);
          this->expecting_version_ = false;
          parsed = true;
        }
      } else if (strlen(line) >= 3 && isdigit(line[0]) && isdigit(line[1]) && isdigit(line[2])) {
        int status_code = atoi(line);
        // Only treat as status if it's a known status code
        if (status_code == 210 || status_code == 211 || status_code == 212) {
          this->axa_status_ = status_code;
          ESP_LOGD(TAG, "Parsed AXA Status Code: %d, Line: %s", this->axa_status_, line);
          parsed = true;
          break;
        } else {
          ESP_LOGD(TAG, "Parsed AXA Status Code: %d, Line: %s", status_code, line);
          parsed = true;
        }
      }
    }

    line = strtok(NULL, "\n");
  }

  if (!parsed) {
    this->axa_status_ = -1;
    ESP_LOGW(TAG, "Failed to parse response. Full Response: %s", this->buffer_);
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