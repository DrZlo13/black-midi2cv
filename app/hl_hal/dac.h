#pragma once
#include <hal/hal.h>

#define DAC_VOLTAGE_MAX 8.0f
#define DAC_VALUE_MAX UINT16_MAX

class DAC {
private:
    HalTimer timer;
    HalTimer::Channel ch_msb;
    HalTimer::Channel ch_lsb;
    float max_voltage;

public:
    DAC(TIM_TypeDef* timer, HalTimer::Channel msb, HalTimer::Channel lsb)
        : timer(timer) {
        ch_msb = msb;
        ch_lsb = lsb;
        max_voltage = DAC_VOLTAGE_MAX;
    }

    void start() {
        this->timer.config(0, 255);
        this->timer.config_channel(this->ch_msb, 0);
        this->timer.config_channel(this->ch_lsb, 0);
        this->timer.enable_channel(this->ch_msb);
        this->timer.enable_channel(this->ch_lsb);
        this->timer.start();
    }

    void set_voltage(float voltage) {
        // Clamp voltage to 0V and max DAC voltage
        if(voltage < 0) {
            voltage = 0;
        } else if(voltage > max_voltage) {
            voltage = max_voltage;
        }

        uint32_t value = (uint32_t)((voltage / max_voltage) * DAC_VALUE_MAX);
        if(value > DAC_VALUE_MAX) {
            value = DAC_VALUE_MAX;
        }
        this->timer.set_channel_value(this->ch_msb, (value >> 8) & 0xFF);
        this->timer.set_channel_value(this->ch_lsb, value & 0xFF);
    }

    void set_max_voltage(float max_voltage) {
        this->max_voltage = max_voltage;
    }
};