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

struct Identifier init_id();
int compare_id(struct Identifier *id_1, struct Identifier *id_2);

struct Date {
    uint16_t year, month, day;
    uint16_t hour, minute, second;
};

struct Date init_date();
int compare_date(struct Date *date_1, struct Date *date_2);
void get_ymd(struct Date *date, uint64_t time);
void get_hms(struct Date *date, uint64_t time);
void add_date(struct Date *result, struct Date *date_1, struct Date *date_2);
int is_illegal_date(int year, int month, int day);

uint64_t get_bits(uint8_t start, uint8_t size, uint8_t *data);

#endif
