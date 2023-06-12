#pragma once
#include <stdint.h>
#include "helper.h"

namespace HalSysTick {
typedef void (*Callback)(void* context);

void init(uint32_t ticks_per_s);

void set_callback(Callback callback, void* context);

}