#pragma once
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>

class HalGpio {
private:
    GPIO_TypeDef* port;
    uint16_t pin;

public:
    HalGpio(GPIO_TypeDef* port, uint16_t pin);
    ~HalGpio();
    void toggle(void);
    void write(bool value);
    bool read(void);
};

extern HalGpio gpio_led;