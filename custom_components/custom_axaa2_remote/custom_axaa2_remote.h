#ifndef CUSTOM_AXAA2_REMOTE_H
#define CUSTOM_AXAA2_REMOTE_H

#include "esphome.h"

class CustomAXA2RemoteUART : public Component, public UARTDevice {
public:
    CustomAXA2RemoteUART(UARTComponent *parent);

    Sensor *axa_window;
    TextSensor *axa_status_text;

    void setup() override;
    void loop() override;

    Sensor* get_axa_window();
    TextSensor* get_axa_status_text();

protected:
    unsigned long lastread;
    char buff[30];
    int axa_status;

    void flush_uart_buffer();
    void request_status();
    void read_response();
    void update_sensor_state();
};

#endif // CUSTOM_AXAA2_REMOTE_H 