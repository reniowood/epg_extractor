#include "common.h"

uint64_t get_bits(uint8_t start, uint8_t size, uint8_t *data) {
    uint8_t end = start + size - 1;
    uint8_t start_byte = start / 8, end_byte = end / 8;
    uint8_t bit_index, byte_index;
    uint8_t temp;
    uint64_t bits;

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

struct Identifier init_id() {
    struct Identifier id;

    id.original_network_id = -1;
    id.transport_stream_id = -1;
    id.service_id = -1;
    id.event_id = -1;
    id.table_id = -1;

    return id;
}

int compare_id(struct Identifier *id_1, struct Identifier *id_2) {
    return (id_1->original_network_id == id_2->original_network_id &&
            id_1->transport_stream_id == id_2->transport_stream_id &&
            id_1->service_id == id_2->service_id &&
            ((id_1->event_id == -1 || id_2->event_id == -1) || id_1->event_id == id_2->event_id) &&
            ((id_1->table_id == -1 || id_2->table_id == -1) || id_1->table_id == id_2->table_id));
}

struct Date init_date() {
    struct Date date;

    date.year = 0;
    date.month = 0;
    date.day = 0;
    date.hour = 0;
    date.minute = 0;
    date.second = 0;
}

int compare_date(struct Date *date_1, struct Date *date_2) {
    if (date_1->year < date_2->year ||
       ((date_1->year == date_2->year) && date_1->month < date_2->month) ||
       ((date_1->year == date_2->year && date_1->month == date_2->month) && date_1->day < date_2->day) ||
       ((date_1->year == date_2->year && date_1->month == date_2->month && date_1->day == date_2->day) && date_1->hour < date_2->hour) ||
       ((date_1->year == date_2->year && date_1->month == date_2->month && date_1->day == date_2->day && date_1->hour == date_2->hour) && date_1->minute < date_2->minute) ||
       ((date_1->year == date_2->year && date_1->month == date_2->month && date_1->day == date_2->day && date_1->hour == date_2->hour && date_1->minute == date_2->minute) && date_1->second <= date_2->second))
        return 1; // date_1 <= date_2;
    else
        return 0; // date_1 > date_2;
}

void get_ymd(struct Date *date, uint64_t time) {
    uint16_t MJD, k;

    MJD = (time >> 24) & 0xffff;
    date->year = (MJD - 15078.2) / 365.25;
    date->month = (MJD - 14956.1 - (int)(date->year * 365.25)) / 30.6001;
    date->day = MJD - 14956 - (int)(date->year * 365.25) - (int)(date->month * 30.6001);
    if (date->month == 14 || date->month == 15)
        k = 1;
    else
        k = 0;
    date->year += k;
    date->year += 1900;
    date->month -= 1 + 12 * k;
}

void get_hms(struct Date *date, uint64_t time) {
    time &= 0xffffff;

    date->hour = ((time >> 16) & 0xff) / 16 * 10+ ((time >> 16) & 0xff) % 16;
    date->minute = ((time >> 8) & 0xff) / 16 * 10 + ((time >> 8) & 0xff) % 16;
    date->second = (time & 0xff) / 16 + (time & 0xff) % 16;
}

void add_date(struct Date *result, struct Date *date_1, struct Date *date_2) {
    int new_day;

    result->second = date_1->second + date_2->second;
    result->minute = date_1->minute + date_2->minute + result->second / 60;
    result->second %= 60;
    result->hour = date_1->hour + date_2->hour + result->minute / 60;
    result->minute %= 60;
    result->day = date_1->day + result->hour / 24;
    result->hour %= 24;
    result->month = date_1->month;
    result->year = date_1->year;

    while (new_day = is_illegal_date(result->year, result->month, result->day)) {
        result->day = new_day;
        result->month++;
        if (result->month == 13) {
            result->month = 1;
            result->year++;
        }
    }
}

int is_illegal_date(int year, int month, int day) {
    if ((day > 31) &&
        ((month == 1) ||
         (month == 3) ||
         (month == 5) ||
         (month == 7) ||
         (month == 8) ||
         (month == 10) ||
         (month == 12))) {
        return day - 31;
    } else if ((day > 30) &&
               ((month == 2) ||
                (month == 4) ||
                (month == 6) ||
                (month == 9) ||
                (month == 11))) {
        return day - 30;
    } else if (month == 2) {
        if ((day > 29) &&
            (year % 4 == 0) &&
            (year % 100 != 0) ||
            (year % 400 == 0)) {
            return day - 29;
        } else if ((day > 28) &&
                   (year % 4 != 0)) {
            return day - 28;
        }
    }

    return 0;
}
