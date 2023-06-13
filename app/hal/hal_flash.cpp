#include "hal_flash.h"

static const size_t sector_address[8] = {
    0x08000000,
    0x08004000,
    0x08008000,
    0x0800C000,
    0x08010000,
    0x08020000,
    0x08040000,
    0x08060000,
};

HalFlash::HalFlash() {
}

HalFlash::~HalFlash() {
}

void HalFlash::unlock(void) {
    // wait for flash to be ready
    while(FLASH->SR & FLASH_SR_BSY) {
    }

    // unlock flash
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    // enable interrupts
    FLASH->CR |= FLASH_CR_EOPIE | FLASH_CR_ERRIE;
}

void HalFlash::lock(void) {
    // disable interrupts
    FLASH->CR &= ~(FLASH_CR_EOPIE | FLASH_CR_ERRIE);

    // lock flash
    FLASH->CR |= FLASH_CR_LOCK;
}

void HalFlash::clear_flags(void) {
    // clear errors
    FLASH->SR |= FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR |
                 FLASH_SR_RDERR | FLASH_SR_EOP;
}

bool HalFlash::flags_is_error(void) {
    uint32_t sr = FLASH->SR;
    if(sr & FLASH_SR_WRPERR || sr & FLASH_SR_PGAERR || sr & FLASH_SR_PGPERR ||
       sr & FLASH_SR_PGSERR) {
        return false;
    }

    return true;
}

bool HalFlash::erase(Sector sector) {
    unlock();
    clear_flags();

    // check sector number
    if(sector > 0b111) {
        return false;
    }

    // wait for flash to be ready
    while(FLASH->SR & FLASH_SR_BSY) {
    }

    // set parallelism to 32-bit
    FLASH->CR |= FLASH_CR_PSIZE_1;

    // set sector erase bit and sector number
    FLASH->CR |= FLASH_CR_SER | ((sector << 3) & FLASH_CR_SNB_Msk);

    // start erase
    FLASH->CR |= FLASH_CR_STRT;

    // wait for erase to finish
    while(FLASH->SR & FLASH_SR_BSY) {
    }

    // clear sector erase bit
    FLASH->CR &= ~FLASH_CR_SER;

    // clear programming bits
    FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_SER | FLASH_CR_SNB | FLASH_MER_BIT);

    bool ret = flags_is_error();

    lock();

    return ret;
}

bool HalFlash::write(Sector sector, size_t offset, uint32_t data) {
    unlock();
    clear_flags();

    // wait for flash to be ready
    while(FLASH->SR & FLASH_SR_BSY) {
    }

    // set parallelism to 32-bit
    FLASH->CR |= FLASH_CR_PSIZE_1;

    // set programming bit
    FLASH->CR |= FLASH_CR_PG;

    // calculate address
    uint32_t address = sector_address[sector] + offset;

    // write data
    *reinterpret_cast<volatile uint32_t*>(address) = data;

    // wait for write to finish
    while(FLASH->SR & FLASH_SR_BSY) {
    }

    // clear programming bits
    FLASH->CR &= ~(FLASH_CR_PG | FLASH_CR_SER | FLASH_CR_SNB | FLASH_MER_BIT);

    bool ret = flags_is_error();

    lock();

    return ret;
}

uint32_t HalFlash::read(Sector sector, size_t offset) {
    // calculate address
    uint32_t address = sector_address[sector] + offset;

    return *reinterpret_cast<volatile uint32_t*>(address);
}

size_t HalFlash::get_sector_size(Sector sector) {
    switch(sector) {
    case S0_16k:
    case S1_16k:
    case S2_16k:
    case S3_16k:
        return 16 * 1024;
    case S4_64k:
        return 64 * 1024;
    case S5_128k:
    case S6_128k:
    case S7_128k:
        return 128 * 1024;
    default:
        return 0;
    }
}
