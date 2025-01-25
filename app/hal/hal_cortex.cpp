#include "hal_cortex.h"
#include <stm32f4xx.h>

#define HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND (SystemCoreClock / 1000000)

namespace HalCortex {

void init(void) {
    CoreDebug->DEMCR |= (CoreDebug_DEMCR_TRCENA_Msk | CoreDebug_DEMCR_MON_EN_Msk);
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0U;

    /* Enable instruction prefetch */
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);
}

void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND * us;

    while((DWT->CYCCNT - start) < time_ticks) {
    };
}

Timer timer_get(uint32_t timeout_us) {
    Timer cortex_timer = {DWT->CYCCNT, HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND * timeout_us};
    return cortex_timer;
}

Timer::Timer(uint32_t start, uint32_t value)
    : start(start)
    , value(value) {
}

bool Timer::is_expired() {
    return !((DWT->CYCCNT - this->start) < this->value);
}

void Timer::wait() {
    while(!is_expired());
}

}