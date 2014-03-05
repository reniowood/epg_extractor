#ifndef COMMON_H
#define COMMON_H

#define TS_PACKET_SIZE          188
#define TS_PACKET_HEADER_SIZE   4

#include <stdio.h>
#include <stdint.h>

uint64_t get_bits(uint8_t start, uint8_t size, uint8_t *data);

#endif
