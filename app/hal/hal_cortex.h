#pragma once
#include <stdint.h>

namespace HalCortex {

class Timer {
private:
    uint32_t start;
    uint32_t value;

public:
    Timer(uint32_t start, uint32_t value);

    bool is_expired();

    void wait();
};

void init(void);

void delay_us(uint32_t us);

Timer timer_get(uint32_t timeout_us);

}