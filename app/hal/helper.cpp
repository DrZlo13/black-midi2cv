#include "helper.h"

uint8_t get_first_index_of_one(uint16_t value) {
    uint8_t index = 0;
    for(index = 0; index < 16; index++) {
        if(value & (1 << index)) break;
    }
    return index;
}