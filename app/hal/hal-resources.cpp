#include "hal-resources.h"

HalGpio gpio_led(GPIOC, LL_GPIO_PIN_13, HalGpio::Mode::OutputPushPull);
HalGpio gpio_button(GPIOA, LL_GPIO_PIN_0, HalGpio::Mode::Input, HalGpio::Pull::Up);

HalUart uart_midi(USART1, 31250);
HalUart uart_debug(USART2, 115200);