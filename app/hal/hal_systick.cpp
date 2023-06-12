#include "hal_systick.h"
#include <stm32f4xx_ll_utils.h>
#include <stm32f4xx_ll_cortex.h>

extern uint32_t SystemCoreClock;

namespace HalSysTick {
Callback callback = nullptr;
void* context = nullptr;

void init(uint32_t ticks_per_s) {
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_InitTick(SystemCoreClock, ticks_per_s);
    LL_SYSTICK_EnableIT();
}

void set_callback(Callback callback, void* context) {
    HalSysTick::callback = callback;
    HalSysTick::context = context;
}

}

extern "C" void SysTick_Handler(void) {
    if(HalSysTick::callback != nullptr) {
        HalSysTick::callback(HalSysTick::context);
    }

    HAL_IncTick();
}