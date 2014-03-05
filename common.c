#include "common.h"

uint32_t get_bits(uint8_t start, uint8_t size, uint8_t *data) {
    uint8_t end = start + size - 1;
    uint8_t start_byte = start / 8, end_byte = end / 8;
    uint8_t bit_index, byte_index;
    uint32_t bits, temp;

    bits = 0;
    bit_index = start;
    while (bit_index <= end) {
        byte_index = bit_index / 8;

        if (end < (byte_index + 1) * 8) {
            temp = *(data + byte_index);
            temp >>= (byte_index + 1) * 8 - end - 1;
            temp &= ((1 << (end - bit_index + 1)) - 1);

            bits <<= end - bit_index + 1;
            bits |= temp;

            bit_index = end + 1;
        } else {
            temp = *(data + byte_index);
            temp &= ((1 << ((byte_index + 1) * 8 - bit_index)) - 1);

            bits <<= (byte_index + 1) * 8 - bit_index;
            bits |= temp;

            bit_index += (byte_index + 1) * 8 - bit_index;
        }
    }

    return bits;
}
