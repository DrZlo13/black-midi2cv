#include "hal_gpio.h"
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_exti.h>

typedef struct {
    HalGpio::Callback callback;
    void* context;
} GpioInterrupt;

static volatile GpioInterrupt gpio_interrupt[GPIO_COUNT] = {0};

static void gpio_interrupt_call(uint16_t pin_num) {
    if(gpio_interrupt[pin_num].callback != NULL) {
        gpio_interrupt[pin_num].callback(gpio_interrupt[pin_num].context);
    }
}

void HalGpio::set_interrupt_callback(Callback callback, void* context) {
    __disable_irq();
    uint8_t pin_num = get_first_index_of_one(this->pin);
    gpio_interrupt[pin_num].callback = callback;
    gpio_interrupt[pin_num].context = context;
    __enable_irq();
}

/* Interrupt handlers */
extern "C" void EXTI0_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
        gpio_interrupt_call(0);
    }
}

extern "C" void EXTI1_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
        gpio_interrupt_call(1);
    }
}

extern "C" void EXTI2_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
        gpio_interrupt_call(2);
    }
}

extern "C" void EXTI3_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
        gpio_interrupt_call(3);
    }
}

extern "C" void EXTI4_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
        gpio_interrupt_call(4);
    }
}

extern "C" void EXTI9_5_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
        gpio_interrupt_call(5);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
        gpio_interrupt_call(6);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
        gpio_interrupt_call(7);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
        gpio_interrupt_call(8);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
        gpio_interrupt_call(9);
    }
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
        gpio_interrupt_call(10);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_11)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
        gpio_interrupt_call(11);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_12)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_12);
        gpio_interrupt_call(12);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
        gpio_interrupt_call(13);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_14);
        gpio_interrupt_call(14);
    }
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_15)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
        gpio_interrupt_call(15);
    }
}