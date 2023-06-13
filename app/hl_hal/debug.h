#pragma once
#include <string>

namespace Debug {
void init(void);
void info(const char* tag, const char* message, ...);
void warning(const char* tag, const char* message, ...);
void error(const char* tag, const char* message, ...);
}