#include "app.h"
#include <main.h>
#include <string>
#include <hal/hal.h>

void uart1_event_cb(HalUart::Event event, uint8_t data, void* context) {
    if(event == HalUart::Event::RXNotEmpty) {
        HalUart* uart = reinterpret_cast<HalUart*>(context);
        uart->transmit(&data, 1);
    }
}

void app_main(void) {
    hal_init();

    uart_midi.set_interrupt_callback(uart1_event_cb, &uart_debug);
    uart_debug.set_interrupt_callback(uart1_event_cb, &uart_midi);

    uart_midi.transmit("UART1/MIDI\r\n");
    uart_debug.transmit("UART2/DEBUG\r\n");

    while(true) {
        gpio_led.toggle();
        HAL_Delay(1000);
    }
}