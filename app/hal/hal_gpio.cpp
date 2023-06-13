#include "hal_gpio.h"
#include "helper.h"
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_exti.h>

#define GET_SYSCFG_EXTI_PORT(gpio)                \
    (((gpio) == (GPIOA)) ? LL_SYSCFG_EXTI_PORTA : \
     ((gpio) == (GPIOB)) ? LL_SYSCFG_EXTI_PORTB : \
     ((gpio) == (GPIOC)) ? LL_SYSCFG_EXTI_PORTC : \
     ((gpio) == (GPIOD)) ? LL_SYSCFG_EXTI_PORTD : \
     ((gpio) == (GPIOE)) ? LL_SYSCFG_EXTI_PORTE : \
                           LL_SYSCFG_EXTI_PORTH)

#define GPIO_PIN_MAP(pin, prefix)               \
    (((pin) == (LL_GPIO_PIN_0))  ? prefix##0 :  \
     ((pin) == (LL_GPIO_PIN_1))  ? prefix##1 :  \
     ((pin) == (LL_GPIO_PIN_2))  ? prefix##2 :  \
     ((pin) == (LL_GPIO_PIN_3))  ? prefix##3 :  \
     ((pin) == (LL_GPIO_PIN_4))  ? prefix##4 :  \
     ((pin) == (LL_GPIO_PIN_5))  ? prefix##5 :  \
     ((pin) == (LL_GPIO_PIN_6))  ? prefix##6 :  \
     ((pin) == (LL_GPIO_PIN_7))  ? prefix##7 :  \
     ((pin) == (LL_GPIO_PIN_8))  ? prefix##8 :  \
     ((pin) == (LL_GPIO_PIN_9))  ? prefix##9 :  \
     ((pin) == (LL_GPIO_PIN_10)) ? prefix##10 : \
     ((pin) == (LL_GPIO_PIN_11)) ? prefix##11 : \
     ((pin) == (LL_GPIO_PIN_12)) ? prefix##12 : \
     ((pin) == (LL_GPIO_PIN_13)) ? prefix##13 : \
     ((pin) == (LL_GPIO_PIN_14)) ? prefix##14 : \
                                   prefix##15)

#define GET_SYSCFG_EXTI_LINE(pin) GPIO_PIN_MAP(pin, LL_SYSCFG_EXTI_LINE)
#define GET_EXTI_LINE(pin) GPIO_PIN_MAP(pin, LL_EXTI_LINE_)

HalGpio::HalGpio(GPIO_TypeDef* port_, uint16_t pin_)
    : port{port_}
    , pin{pin_} {
}

HalGpio::~HalGpio() {
}

void HalGpio::config(
    const HalGpio::Mode mode,
    const HalGpio::Pull pull,
    const HalGpio::Speed speed,
    const HalGpio::AltFn alt_fn) {
    uint32_t sys_exti_port = GET_SYSCFG_EXTI_PORT(this->port);
    uint32_t sys_exti_line = GET_SYSCFG_EXTI_LINE(this->pin);
    uint32_t exti_line = GET_EXTI_LINE(this->pin);

    __disable_irq();
    switch(speed) {
    case Speed::Low:
        LL_GPIO_SetPinSpeed(this->port, this->pin, LL_GPIO_SPEED_FREQ_LOW);
        break;
    case Speed::Medium:
        LL_GPIO_SetPinSpeed(this->port, this->pin, LL_GPIO_SPEED_FREQ_MEDIUM);
        break;
    case Speed::High:
        LL_GPIO_SetPinSpeed(this->port, this->pin, LL_GPIO_SPEED_FREQ_HIGH);
        break;
    case Speed::VeryHigh:
        LL_GPIO_SetPinSpeed(this->port, this->pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
        break;
    }

    switch(pull) {
    case Pull::No:
        LL_GPIO_SetPinPull(this->port, this->pin, LL_GPIO_PULL_NO);
        break;
    case Pull::Up:
        LL_GPIO_SetPinPull(this->port, this->pin, LL_GPIO_PULL_UP);
        break;
    case Pull::Down:
        LL_GPIO_SetPinPull(this->port, this->pin, LL_GPIO_PULL_DOWN);
        break;
    }

    if(mode >= Mode::InterruptRise) {
        // Enable EXTI
        LL_GPIO_SetPinMode(this->port, this->pin, LL_GPIO_MODE_INPUT);
        LL_SYSCFG_SetEXTISource(sys_exti_port, sys_exti_line);

        switch(mode) {
        case Mode::InterruptRise:
            LL_EXTI_EnableIT_0_31(exti_line);
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
            break;
        case Mode::InterruptFall:
            LL_EXTI_EnableIT_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
            break;
        case Mode::InterruptRiseFall:
            LL_EXTI_EnableIT_0_31(exti_line);
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
            break;
        case Mode::EventRise:
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
            break;
        case Mode::EventFall:
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
            break;
        case Mode::EventRiseFall:
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
            break;
        default:
            // TODO error
            break;
        }
    } else {
        // Disable EXTI
        if(LL_SYSCFG_GetEXTISource(sys_exti_line) == sys_exti_port &&
           LL_EXTI_IsEnabledIT_0_31(exti_line)) {
            LL_EXTI_DisableIT_0_31(exti_line);
            LL_EXTI_DisableRisingTrig_0_31(exti_line);
            LL_EXTI_DisableFallingTrig_0_31(exti_line);
        }

        switch(mode) {
        case Mode::Input:
            LL_GPIO_SetPinMode(this->port, this->pin, LL_GPIO_MODE_INPUT);
            break;
        case Mode::OutputPushPull:
        case Mode::AltFunctionPushPull:
            LL_GPIO_SetPinMode(this->port, this->pin, LL_GPIO_MODE_OUTPUT);
            LL_GPIO_SetPinOutputType(this->port, this->pin, LL_GPIO_OUTPUT_PUSHPULL);
            break;
        case Mode::OutputOpenDrain:
        case Mode::AltFunctionOpenDrain:
            LL_GPIO_SetPinMode(this->port, this->pin, LL_GPIO_MODE_OUTPUT);
            LL_GPIO_SetPinOutputType(this->port, this->pin, LL_GPIO_OUTPUT_OPENDRAIN);
            break;
        case Mode::Analog:
            LL_GPIO_SetPinMode(this->port, this->pin, LL_GPIO_MODE_ANALOG);
            break;
        default:
            // TODO error
            break;
        }
    }

    if(mode == Mode::AltFunctionPushPull || mode == Mode::AltFunctionOpenDrain) {
        // enable alternate mode
        LL_GPIO_SetPinMode(this->port, this->pin, LL_GPIO_MODE_ALTERNATE);

        // set alternate function
        if(get_first_index_of_one(this->pin) < 8) {
            LL_GPIO_SetAFPin_0_7(this->port, this->pin, (uint32_t)alt_fn);
        } else {
            LL_GPIO_SetAFPin_8_15(this->port, this->pin, (uint32_t)alt_fn);
        }
    }

    __enable_irq();
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
