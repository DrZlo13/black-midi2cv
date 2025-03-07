#pragma once
#include <span>
#include <stdint.h>

namespace HalI2C {

bool is_device_ready(uint8_t addr, uint32_t timeout_us);

bool write_mem(uint8_t addr, uint8_t reg, std::span<const uint8_t> data, uint32_t timeout_us);

bool write_mem(uint8_t addr, uint8_t reg, const uint8_t* data, uint32_t timeout_us);

bool read_mem(uint8_t addr, uint8_t reg, std::span<uint8_t> data, uint32_t timeout_us);

bool read_mem(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t timeout_us);
}