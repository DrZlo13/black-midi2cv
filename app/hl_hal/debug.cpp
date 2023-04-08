#include <hal/hal.h>
#include "debug.h"
#include <cstdarg>

static HalUart uart_debug(USART2);
static size_t tick = 0;

#define COLOR(clr) "\033[0;" clr "m"
#define COLOR_RESET "\033[0m"

#define COLOR_RED COLOR("31")
#define COLOR_GREEN COLOR("32")
#define COLOR_BROWN COLOR("33")

namespace Debug {
enum Level {
    Info,
    Warning,
    Error,
};

void print(Level level, const char* tag, const char* message, va_list args) {
    uart_debug.transmit(std::to_string(tick));

    switch(level) {
    case Info:
        uart_debug.transmit(COLOR_GREEN);
        uart_debug.transmit(" [I][");
        break;
    case Warning:
        uart_debug.transmit(COLOR_BROWN);
        uart_debug.transmit(" [W][");
        break;
    case Error:
        uart_debug.transmit(COLOR_RED);
        uart_debug.transmit(" [E][");
        break;
    }
    uart_debug.transmit(tag);
    uart_debug.transmit("] ");
    uart_debug.transmit(COLOR_RESET);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), message, args);

    uart_debug.transmit(buffer);
    uart_debug.transmit("\r\n");

    tick++;
}

void init(void) {
    uart_debug.config(230400, false, true);
    info("Debug", "Init done");
}

void info(const char* tag, const char* message, ...) {
    va_list args;
    va_start(args, message);
    print(Level::Info, tag, message, args);
    va_end(args);
}

void warning(const char* tag, const char* message, ...) {
    va_list args;
    va_start(args, message);
    print(Level::Warning, tag, message, args);
    va_end(args);
}

void error(const char* tag, const char* message, ...) {
    va_list args;
    va_start(args, message);
    print(Level::Error, tag, message, args);
    va_end(args);
}

}
