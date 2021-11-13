#include "hal-gpio.h"
#define GPIO_NUMBER 16U

HalGpio gpio_led(GPIOC, GPIO_PIN_13);

HalGpio::HalGpio(GPIO_TypeDef* port, uint16_t pin) {
    this->port = port;
    this->pin = pin;
}

HalGpio::~HalGpio() {
}

void HalGpio::toggle(void) {
    LL_GPIO_TogglePin(this->port, this->pin);
}

void HalGpio::write(bool value) {
    if(value) {
        LL_GPIO_SetOutputPin(this->port, this->pin);
    } else {
        LL_GPIO_ResetOutputPin(this->port, this->pin);
    }
}

bool HalGpio::read(void) {
    if((LL_GPIO_ReadInputPort(this->port) & this->pin) != 0x00U) {
        return true;
    } else {
        return false;
    }
}
