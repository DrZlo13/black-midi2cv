#include "hal-resources.h"

HalGpio gpio_led(GPIOC, LL_GPIO_PIN_13);
HalGpio gpio_button(GPIOA, LL_GPIO_PIN_0);
HalUart uart_midi(USART1);
HalUart uart_debug(USART2);