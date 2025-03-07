#pragma once
#include <stdint.h>
#include <hal/hal_gpio.h>

namespace FUSB302 {

enum EventType {
    ConnectedDevice,
    ConnectedHost,
    Disconnected,
};

typedef void (*EventCallback)(EventType event, void* context);

bool init(HalGpio& int_gpio, EventCallback callback, void* context);

}; // namespace FUSB302