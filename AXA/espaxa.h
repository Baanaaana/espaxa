#include "esphome.h"

using namespace esphome;

#define DELAY_MS 5000 // Delay in milliseconds between window checks

// Define AXA Remote 2.0 return codes
#define AXA_OPENED 210 // 210 Unlocked
#define AXA_STRONG_LOCKED 211 // 211 Strong Locked
#define AXA_WEAK_LOCKED 212 // 212 Weak Locked

class CustomAXA2RemoteUART : public Component, public UARTDevice {
public:
    CustomAXA2RemoteUART(UARTComponent *parent) : UARTDevice(parent) {}

    Sensor *axa_window = new Sensor();  // Define Sensor for open/close status
    TextSensor *axa_status_text = new TextSensor();  // Define Text Sensor for status display

    void setup() override {
        lastread = 0;
        ESP_LOGCONFIG("espaxa", "Setting up AXA UART...");
    }

    void loop() override {
        unsigned long now = millis();

        if (now - lastread > DELAY_MS || lastread == 0) {
            lastread = now;
            flush_uart_buffer(); // Clear buffer
            request_status();
            delay(100);

            if (available()) {
                read_response();
                update_sensor_state();
            }
        }
    }

protected:
    unsigned long lastread;
    char buff[30];
    int axa_status;

    void flush_uart_buffer() {
        while (available()) {
            read();
        }
    }

    void request_status() {
        write_str("\r\n");
        delay(100);
        write_str("STATUS\r\n");
    }

    void read_response() {
        int i = 0;
        while (available()) {
            buff[i++] = read();
            if (i >= 29) break;
        }
        buff[i] = '\0';

        // Manually parse lines from the buffer
        char *line = strtok(buff, "\n");
        bool parsed = false;

        while (line != NULL) {
            // Remove any carriage return characters
            for (int j = 0; line[j] != '\0'; j++) {
                if (line[j] == '\r') {
                    line[j] = '\0';
                    break;
                }
            }

            // Check if the line starts with a 3-digit status code
            if (strlen(line) >= 3 && isdigit(line[0]) && isdigit(line[1]) && isdigit(line[2])) {
                axa_status = atoi(line);  // Convert first 3 characters to integer
                ESP_LOGD("espaxa", "Parsed AXA Status Code: %d, Line: %s", axa_status, line);
                parsed = true;
                break;
            }

            // Move to the next line
            line = strtok(NULL, "\n");
        }

        if (!parsed) {
            axa_status = -1;  // Set to an invalid status if parsing fails
            ESP_LOGW("espaxa", "Failed to parse AXA Status Code. Full Response: %s", buff);
        }
    }

    void update_sensor_state() {
        if (axa_status == AXA_STRONG_LOCKED) {
            ESP_LOGD("espaxa", "Setting window state to STRONG LOCKED (CLOSED)");
            axa_window->publish_state(0);  // Publish 0 for strong locked (closed)
            axa_status_text->publish_state("Strong Locked");
        } else if (axa_status == AXA_OPENED) {
            ESP_LOGD("espaxa", "Setting window state to OPEN (UNLOCKED)");
            axa_window->publish_state(1);  // Publish 1 for open
            axa_status_text->publish_state("Unlocked");
        } else if (axa_status == AXA_WEAK_LOCKED) {
            ESP_LOGD("espaxa", "Setting window state to WEAK LOCKED (CLOSED)");
            axa_window->publish_state(0);  // Publish 0 for weak locked (closed)
            axa_status_text->publish_state("Weak Locked");
        } else {
            ESP_LOGD("espaxa", "Unknown AXA status, state not updated.");
            axa_status_text->publish_state("Unknown");
        }
    }
};
