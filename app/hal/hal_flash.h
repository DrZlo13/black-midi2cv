#pragma once
#include <stdint.h>
#include <stm32f4xx.h>
#include "helper.h"

class HalFlash {
private:
    DISALLOW_COPY_AND_ASSIGN(HalFlash);
    void unlock(void);
    void lock(void);

    bool flags_is_error(void);
    void clear_flags(void);

public:
    enum Sector {
        S0_16k = 0b000,
        S1_16k = 0b001,
        S2_16k = 0b010,
        S3_16k = 0b011,
        S4_64k = 0b100,
        S5_128k = 0b101,
        S6_128k = 0b110,
        S7_128k = 0b111,
    };

    HalFlash();
    ~HalFlash();

    bool erase(Sector sector);
    bool write(Sector sector, size_t offset, uint32_t data);
    uint32_t read(Sector sector, size_t offset);
};