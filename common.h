#ifndef COMMON_H
#define COMMON_H

#define TS_PACKET_SIZE          188
#define TS_PACKET_HEADER_SIZE   4

#include <stdio.h>
#include <stdint.h>

struct Identifier {
    uint32_t original_network_id;
    uint32_t transport_stream_id;
    uint32_t service_id;
    uint32_t event_id;
    uint32_t table_id;
};

uint64_t get_bits(uint8_t start, uint8_t size, uint8_t *data);
struct Identifier init_id();
int compare_id(struct Identifier *id_1, struct Identifier *id_2);

#endif
