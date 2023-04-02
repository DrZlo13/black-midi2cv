#pragma once
#include <stdint.h>
#include "helper.h"
#include <stm32f4xx_ll_tim.h>
#include <string>

class HalTimer {
private:
    TIM_TypeDef* timer;
    DISALLOW_COPY_AND_ASSIGN(HalTimer);

public:
    HalTimer(TIM_TypeDef* timer);
    ~HalTimer();

    enum class Channel : uint32_t {
        CH1 = LL_TIM_CHANNEL_CH1,
        CH2 = LL_TIM_CHANNEL_CH2,
        CH3 = LL_TIM_CHANNEL_CH3,
        CH4 = LL_TIM_CHANNEL_CH4,
    };

    void config(uint32_t prescaler, uint32_t period);
    void config_channel(Channel channel, uint32_t pulse);
    void set_channel_value(Channel channel, uint32_t pulse);
    void enable_channel(Channel channel);
    void disable_channel(Channel channel);

    void start(void);
    void stop(void);
};