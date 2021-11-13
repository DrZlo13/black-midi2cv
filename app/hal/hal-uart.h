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
    HalUart(USART_TypeDef* uart, uint32_t baudrate);
    ~HalUart();
    void transmit(const std::string& string);
    void transmit(const uint8_t* buffer, size_t buffer_size);
};
