#include "app.h"
#include <main.h>
#include <string>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

UART_HandleTypeDef* uart_midi = &huart1;
UART_HandleTypeDef* uart_debug = &huart2;

const std::string uart_midi_text = "UART1/MIDI\r\n";
const std::string uart_debug_text = "UART2/DEBUG\r\n";

void app_main(void) {
    while(true) {
        HAL_UART_Transmit(
            uart_midi, (uint8_t*)(char*)uart_midi_text.data(), uart_midi_text.size(), 1000000);
        HAL_UART_Transmit(
            uart_debug, (uint8_t*)(char*)uart_debug_text.data(), uart_debug_text.size(), 1000000);
        HAL_GPIO_TogglePin(LED_PIN_GPIO_Port, LED_PIN_Pin);
        HAL_Delay(1000);
    }
}