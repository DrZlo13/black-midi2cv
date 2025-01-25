#include "hal_i2c.h"
#include "hal_cortex.h"
#include <stm32f4xx_ll_i2c.h>

namespace HalI2C {
using namespace HalCortex;

static bool master_start(uint32_t timeout_us) {
    volatile uint16_t reg;
    Timer t = timer_get(timeout_us);

    LL_I2C_DisableBitPOS(I2C1);
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    reg = I2C1->SR1; // TODO: why?

    return true;
}

static bool master_send_address(uint8_t address, uint32_t timeout_us) {
    LL_I2C_TransmitData8(I2C1, (address << 1) | 0x00);
    Timer t = timer_get(timeout_us);

    while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    LL_I2C_ClearFlag_ADDR(I2C1);

    while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    return true;
}

static bool master_send_data(uint8_t data, uint32_t timeout_us) {
    LL_I2C_TransmitData8(I2C1, data);
    Timer t = timer_get(timeout_us);

    while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }
    while(!LL_I2C_IsActiveFlag_BTF(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }
}

static void master_stop(void) {
    LL_I2C_GenerateStopCondition(I2C1);
    LL_I2C_ClearFlag_STOP(I2C1);
}

bool is_device_ready(uint8_t addr, uint32_t timeout_us) {
    if(!master_start(timeout_us)) {
        return false;
    }

    if(!master_send_address(addr, timeout_us)) {
        master_stop();
        return false;
    }

    master_stop();
    return true;
}

} // namespace HalI2C