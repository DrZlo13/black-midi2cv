#include <hal/hal.h>

extern "C" void Error_Handler(void);

void core_crash(const char* message) {
    if(message != NULL) {
        uart_debug.transmit(message);
    }
    Error_Handler();
}