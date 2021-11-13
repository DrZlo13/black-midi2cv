#include "hal-uart.h"
#include <stm32f4xx_ll_usart.h>

HalUart::HalUart(USART_TypeDef* uart) {
    this->uart = uart;
}

HalUart::~HalUart() {
}

void HalUart::config(uint32_t baudrate) {
    if(LL_USART_IsEnabled(this->uart)) {
        while(!LL_USART_IsActiveFlag_TC(this->uart))
            ;
        LL_USART_Disable(this->uart);
    }

    LL_USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.BaudRate = baudrate;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Init(this->uart, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(this->uart);
    LL_USART_Enable(this->uart);

    LL_USART_EnableIT_RXNE(this->uart);
    LL_USART_EnableIT_IDLE(this->uart);

    if(this->uart == USART1) {
        NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    } else if(this->uart == USART2) {
        NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    }
}

void HalUart::transmit(const std::string& string) {
    transmit(reinterpret_cast<const uint8_t*>(string.data()), string.size());
}

void HalUart::transmit(const uint8_t* buffer, size_t buffer_size) {
    while(buffer_size > 0) {
        while(!LL_USART_IsActiveFlag_TXE(this->uart))
            ;

        LL_USART_TransmitData8(this->uart, *buffer);
        buffer++;
        buffer_size--;
    }
}