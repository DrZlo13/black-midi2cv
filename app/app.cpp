#include "app.h"
#include <main.h>
#include <string>
#include <hal/hal.h>

void uart1_event_cb(HalUart::Event event, uint8_t data, void* context) {
    gpio_led.write(0);
}

void app_main(void) {
    hal_init();
    gpio_led.write(1);
    uart_midi.set_interrupt_callback(uart1_event_cb, NULL);
    uart_debug.set_interrupt_callback(uart1_event_cb, NULL);

    uart_midi.transmit("UART1/MIDI\r\n");
    uart_debug.transmit("UART2/DEBUG\r\n");

    while(true) {
        HAL_Delay(1000);
    }
}