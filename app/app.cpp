#include "app.h"
#include <main.h>
#include <string>
#include <hal/hal.h>

void app_main(void) {
    hal_init();

    uart_debug.transmit("UART2/DEBUG\r\n");
    uart_midi.transmit("UART1/MIDI\r\n");

    while(true) {
        gpio_led.toggle();
        HAL_Delay(1000);
    }
}