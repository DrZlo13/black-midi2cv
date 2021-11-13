#pragma once
#include <stdint.h>
#include <stm32f4xx_ll_usart.h>

class HalUart {
private:
    USART_TypeDef* uart;

public:
    HalUart();
    ~HalUart();
};
