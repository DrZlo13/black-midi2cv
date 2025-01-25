#pragma once
#include <stdint.h>

namespace HalI2C {

bool is_device_ready(uint8_t addr, uint32_t timeout_us);
}