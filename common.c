#include "common.h"

uint32_t get_bits(uint8_t start, uint8_t size, uint8_t *data) {
    uint8_t start_byte = start / 8;
    uint8_t end_byte = (start + size - 1) / 8;

    uint32_t bits;
    uint8_t temp;

    bits = *(data + start_byte);
    bits &= ((1 << (8 - (start % 8))) - 1);
    bits <<= 8 - (start % 8);

    for (start_byte = start_byte+1; start_byte<end_byte; ++start_byte) {
        temp = *(data + start_byte);
        bits |= temp;
        bits <<= 8;
    }

    temp = *(data + end_byte);
    temp &= ((1 << (8 - (start + size) % 8)) - 1);
    bits |= temp;

    return bits;
}
