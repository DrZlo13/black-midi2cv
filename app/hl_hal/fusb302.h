#pragma once
#include <stdint.h>
#include <hal/hal_gpio.h>

namespace FUSB302 {

enum EventType {
    None,
    ConnectedDevice,
    ConnectedHost,
    Disconnected,
};

bool init(void);

EventType poll(void);

}; // namespace FUSB302