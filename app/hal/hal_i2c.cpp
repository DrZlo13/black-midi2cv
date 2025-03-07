#include "hal_i2c.h"
#include "hal_cortex.h"
#include <stm32f4xx_ll_i2c.h>
#include "hl_hal/debug.h"

namespace HalI2C {
using namespace HalCortex;

static bool master_start(HalCortex::Timer& t) {
    LL_I2C_DisableBitPOS(I2C1);
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    return true;
}

static bool master_send_address(uint8_t address, bool read, HalCortex::Timer& t) {
    LL_I2C_TransmitData8(I2C1, (address << 1) | (read ? 0x01 : 0x00));

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

static void master_stop(void) {
    LL_I2C_GenerateStopCondition(I2C1);
    LL_I2C_ClearFlag_STOP(I2C1);
}

bool is_device_ready(uint8_t addr, uint32_t timeout_us) {
    Timer t = timer_get(timeout_us);
    if(!master_start(t)) {
        return false;
    }

    if(!master_send_address(addr, false, t)) {
        master_stop();
        return false;
    }

    master_stop();
    return true;
}

bool write_mem(uint8_t addr, uint8_t reg, std::span<const uint8_t> data, uint32_t timeout_us) {
    Timer t = timer_get(timeout_us);

    while(LL_I2C_IsActiveFlag_BUSY(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    LL_I2C_DisableBitPOS(I2C1);

    //
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    // Send write address
    LL_I2C_TransmitData8(I2C1, (addr << 1) | 0x00);

    while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    LL_I2C_ClearFlag_ADDR(I2C1);

    while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
        if(t.is_expired()) {
            LL_I2C_GenerateStopCondition(I2C1);
            return false;
        }
    }

    // Send register
    LL_I2C_TransmitData8(I2C1, reg);

    // Write data
    const uint8_t* ptr = data.data();
    size_t size = data.size();

    while(size > 0) {
        while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
            if(t.is_expired()) {
                LL_I2C_GenerateStopCondition(I2C1);
                return false;
            }
        }

        LL_I2C_TransmitData8(I2C1, *ptr);
        ptr++;
        size--;

        if(size != 0 && LL_I2C_IsActiveFlag_BTF(I2C1)) {
            LL_I2C_TransmitData8(I2C1, *ptr);
            ptr++;
            size--;
        }
    }

    while(!LL_I2C_IsActiveFlag_BTF(I2C1)) {
        if(t.is_expired()) {
            LL_I2C_GenerateStopCondition(I2C1);
            return false;
        }
    }

    LL_I2C_GenerateStopCondition(I2C1);

    return true;
}

bool read_mem(uint8_t addr, uint8_t reg, std::span<uint8_t> data, uint32_t timeout_us) {
    Timer t = timer_get(timeout_us);

    while(LL_I2C_IsActiveFlag_BUSY(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    LL_I2C_DisableBitPOS(I2C1);

    // Start and ack next data

    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    // Send write address

    LL_I2C_TransmitData8(I2C1, (addr << 1) | 0x00);

    while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    LL_I2C_ClearFlag_ADDR(I2C1);

    while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
        if(t.is_expired()) {
            LL_I2C_GenerateStopCondition(I2C1);
            return false;
        }
    }

    // Send register

    LL_I2C_TransmitData8(I2C1, reg);

    while(!LL_I2C_IsActiveFlag_TXE(I2C1)) {
        if(t.is_expired()) {
            LL_I2C_GenerateStopCondition(I2C1);
            return false;
        }
    }

    // Restart

    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    // Send read address

    LL_I2C_TransmitData8(I2C1, (addr << 1) | 0x01);

    while(!LL_I2C_IsActiveFlag_ADDR(I2C1)) {
        if(t.is_expired()) {
            return false;
        }
    }

    //
    if(data.size() == 0) {
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
    } else if(data.size() == 1) {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
    } else if(data.size() == 2) {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        LL_I2C_EnableBitPOS(I2C1);
        LL_I2C_ClearFlag_ADDR(I2C1);
    } else {
        LL_I2C_ClearFlag_ADDR(I2C1);
    }

    // Read data
    uint8_t* ptr = data.data();
    size_t size = data.size();

    while(size > 0) {
        if(size <= 3) {
            if(size == 1) {
                while(!LL_I2C_IsActiveFlag_RXNE(I2C1)) {
                    if(t.is_expired()) {
                        return false;
                    }
                }

                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;
            } else if(size == 2) {
                while(!LL_I2C_IsActiveFlag_BTF(I2C1)) {
                    if(t.is_expired()) {
                        return false;
                    }
                }

                LL_I2C_GenerateStopCondition(I2C1);

                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;

                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;
            } else {
                while(!LL_I2C_IsActiveFlag_BTF(I2C1)) {
                    if(t.is_expired()) {
                        return false;
                    }
                }

                LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);

                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;

                while(!LL_I2C_IsActiveFlag_BTF(I2C1)) {
                    if(t.is_expired()) {
                        return false;
                    }
                }

                LL_I2C_GenerateStopCondition(I2C1);

                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;

                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;
            }
        } else {
            while(!LL_I2C_IsActiveFlag_RXNE(I2C1)) {
                if(t.is_expired()) {
                    return false;
                }
            }

            *ptr = LL_I2C_ReceiveData8(I2C1);
            ptr++;
            size--;

            if(LL_I2C_IsActiveFlag_BTF(I2C1)) {
                *ptr = LL_I2C_ReceiveData8(I2C1);
                ptr++;
                size--;
            }
        }
    }

    return true;
}

bool write_mem(uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t timeout_us) {
    return write_mem(addr, reg, std::span<const uint8_t>(data, 1), timeout_us);
}

bool read_mem(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t timeout_us) {
    return read_mem(addr, reg, std::span<uint8_t>(data, 1), timeout_us);
}

} // namespace HalI2C