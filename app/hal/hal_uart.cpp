#include "hal_uart.h"
#include <string.h>
#include <stm32f4xx_ll_usart.h>

HalUart::HalUart(USART_TypeDef* uart) {
    this->uart = uart;
}

HalUart::~HalUart() {
}

void HalUart::config(uint32_t baudrate, bool rx_enable, bool tx_enable) {
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

    if(rx_enable && tx_enable) {
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    } else if(rx_enable) {
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_RX;
    } else if(tx_enable) {
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
    } else {
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_NONE;
    }

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

void HalUart::transmit(const std::string_view string) {
    transmit(reinterpret_cast<const uint8_t*>(string.data()), string.size());
}

void HalUart::transmit(const char* cstring) {
    transmit(reinterpret_cast<const uint8_t*>(cstring), strlen(cstring));
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

void HalUart::transmit_as_hex(const uint8_t data) {
    char symbol;
    uint8_t nibble;

    nibble = (data >> 4) & 0xF;
    if(nibble > 9)
        symbol = 'A' + nibble - 10;
    else
        symbol = '0' + nibble;

    this->transmit((const uint8_t*)&symbol, 1);

    nibble = data & 0xF;
    if(nibble > 9)
        symbol = 'A' + nibble - 10;
    else
        symbol = '0' + nibble;

    this->transmit((const uint8_t*)&symbol, 1);
}
