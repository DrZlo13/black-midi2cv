#pragma once
#include <stdint.h>
#include "helper.h"
#include <stm32f4xx_ll_usart.h>
#include <string>

class HalUart {
private:
    USART_TypeDef* uart;
    DISALLOW_COPY_AND_ASSIGN(HalUart);

public:
    enum class Event {
        RXNotEmpty,
        Idle,
    };

    typedef void (*EventCallback)(Event event, uint8_t data, void* context);

    HalUart(USART_TypeDef* uart, uint32_t baudrate);
    ~HalUart();
    void transmit(const std::string& string);
    void transmit(const uint8_t* buffer, size_t buffer_size);

    void set_interrupt_callback(EventCallback, void* context);
};
