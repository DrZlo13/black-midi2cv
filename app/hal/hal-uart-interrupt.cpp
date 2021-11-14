#include "hal-uart.h"
#include <stm32f4xx_ll_usart.h>

typedef enum {
    UART_1 = 0,
    UART_2 = 1,
    UART_MAX
} UartList;

typedef struct {
    HalUart::EventCallback callback;
    void* context;
} UartInterrupt;

static volatile UartInterrupt uart_unterrupt[UART_MAX] = {0};

void HalUart::set_interrupt_callback(EventCallback callback, void* context) {
    if(this->uart == USART1) {
        uart_unterrupt[UART_1].callback = callback;
        uart_unterrupt[UART_1].context = context;

        if(callback == NULL) {
            NVIC_DisableIRQ(USART1_IRQn);
        } else {
            NVIC_EnableIRQ(USART1_IRQn);
        }
    } else if(this->uart == USART2) {
        uart_unterrupt[UART_2].callback = callback;
        uart_unterrupt[UART_2].context = context;

        if(callback == NULL) {
            NVIC_DisableIRQ(USART2_IRQn);
        } else {
            NVIC_EnableIRQ(USART2_IRQn);
        }
    }
}

extern "C" void USART1_IRQHandler(void) {
    if(LL_USART_IsActiveFlag_RXNE(USART1)) {
        uint8_t data = LL_USART_ReceiveData8(USART1);
        if(uart_unterrupt[UART_1].callback != NULL) {
            uart_unterrupt[UART_1].callback(
                HalUart::Event::RXNotEmpty, data, uart_unterrupt[UART_1].context);
        }
    } else if(LL_USART_IsActiveFlag_IDLE(USART1)) {
        if(uart_unterrupt[UART_1].callback != NULL) {
            uart_unterrupt[UART_1].callback(
                HalUart::Event::Idle, 0, uart_unterrupt[UART_1].context);
        }
        LL_USART_ClearFlag_IDLE(USART1);
    } else if(LL_USART_IsActiveFlag_ORE(USART1)) {
        LL_USART_ClearFlag_ORE(USART1);
    } else if(LL_USART_IsActiveFlag_FE(USART1)) {
        LL_USART_ClearFlag_FE(USART1);
    } else if(LL_USART_IsActiveFlag_NE(USART1)) {
        LL_USART_ClearFlag_NE(USART1);
    }
}

extern "C" void USART2_IRQHandler(void) {
    if(LL_USART_IsActiveFlag_RXNE(USART2)) {
        uint8_t data = LL_USART_ReceiveData8(USART2);
        if(uart_unterrupt[UART_2].callback != NULL) {
            uart_unterrupt[UART_2].callback(
                HalUart::Event::RXNotEmpty, data, uart_unterrupt[UART_2].context);
        }
    } else if(LL_USART_IsActiveFlag_IDLE(USART2)) {
        if(uart_unterrupt[UART_2].callback != NULL) {
            uart_unterrupt[UART_2].callback(
                HalUart::Event::Idle, 0, uart_unterrupt[UART_2].context);
        }
        LL_USART_ClearFlag_IDLE(USART2);
    } else if(LL_USART_IsActiveFlag_ORE(USART1)) {
        LL_USART_ClearFlag_ORE(USART1);
    } else if(LL_USART_IsActiveFlag_FE(USART1)) {
        LL_USART_ClearFlag_FE(USART1);
    } else if(LL_USART_IsActiveFlag_NE(USART1)) {
        LL_USART_ClearFlag_NE(USART1);
    }
}