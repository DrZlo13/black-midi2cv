#include "hal_timer.h"

HalTimer::HalTimer(TIM_TypeDef* timer) {
    this->timer = timer;
}

HalTimer::~HalTimer() {
}

void HalTimer::config(uint32_t prescaler, uint32_t period) {
    LL_TIM_InitTypeDef tim_init;
    LL_TIM_StructInit(&tim_init);
    tim_init.Prescaler = prescaler;
    tim_init.Autoreload = period;
    LL_TIM_Init(this->timer, &tim_init);
    LL_TIM_EnableARRPreload(TIM1);
    LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
}

void HalTimer::config_channel(Channel channel, uint32_t pulse) {
    LL_TIM_OC_InitTypeDef tim_oc_init;
    LL_TIM_OC_StructInit(&tim_oc_init);
    tim_oc_init.OCMode = LL_TIM_OCMODE_PWM1;
    tim_oc_init.OCState = LL_TIM_OCSTATE_ENABLE;
    tim_oc_init.CompareValue = pulse;
    LL_TIM_OC_Init(this->timer, (uint32_t)channel, &tim_oc_init);
    LL_TIM_OC_EnablePreload(TIM1, (uint32_t)channel);
}

void HalTimer::set_channel_value(Channel channel, uint32_t pulse) {
    switch(channel) {
    case Channel::CH1:
        LL_TIM_OC_SetCompareCH1(this->timer, pulse);
        break;
    case Channel::CH2:
        LL_TIM_OC_SetCompareCH2(this->timer, pulse);
        break;
    case Channel::CH3:
        LL_TIM_OC_SetCompareCH3(this->timer, pulse);
        break;
    case Channel::CH4:
        LL_TIM_OC_SetCompareCH4(this->timer, pulse);
        break;
    }
}

void HalTimer::enable_channel(Channel channel) {
    LL_TIM_CC_EnableChannel(this->timer, (uint32_t)channel);
}

void HalTimer::disable_channel(Channel channel) {
    LL_TIM_CC_DisableChannel(this->timer, (uint32_t)channel);
}

void HalTimer::start(void) {
    LL_TIM_EnableAllOutputs(this->timer);
    LL_TIM_EnableCounter(this->timer);
}

void HalTimer::stop(void) {
    LL_TIM_DisableAllOutputs(this->timer);
    LL_TIM_DisableCounter(this->timer);
}
