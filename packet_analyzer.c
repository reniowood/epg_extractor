#include "packet_analyzer.h"

void pa_analyze_packet(uint8_t *TS_packet) {
    static uint32_t old_PID, PID;
    uint32_t adaptation_field_control, adaptation_field_length, payload_index, payload_unit_start_indicator, pointer_field;
    static uint32_t section_length = 0, section_filled_length = 0;
    static uint8_t *section = NULL;

    /* see PID of packet in the header and check whether the packet is for SDT/EIT or not. */
    PID = get_bits(11, 13, TS_packet);

    if ((PID == PID_SDT) || (PID == PID_EIT)) {
        adaptation_field_control = get_bits(26, 2, TS_packet);
        if (adaptation_field_control == 1) { /* no adaptation field */
            payload_index = 0;
        } else if (adaptation_field_control == 3) {
            adaptation_field_length = get_bits(32, 8, TS_packet);
            payload_index = 1 + adaptation_field_length;
        } else {
            return;
        }

        payload_unit_start_indicator = get_bits(9, 1, TS_packet);

        /* payload_unit_start_indicator == 1; at least one section begins in a given TS packet */
        if (payload_unit_start_indicator == 1) {
            pointer_field = get_bits(32 + payload_index * 8, 8, TS_packet);

            memcpy(section + section_filled_length, TS_packet + payload_index, pointer_field);
            payload_index += pointer_field;
            payload_index++;

            if (section != NULL) {
                if (old_PID == PID_SDT) {
                    sa_analyze_SDT_section(section, section_length);
                } else { /* PID_EIT */
                    sa_analyze_EIT_section(section, section_length);
                }

                free(section);
                section = NULL;
            }

            old_PID = PID;

            if (pointer_field == 0) {
                section_length = get_bits(32 + payload_index * 8 + 12, 12, TS_packet);
                section_length += 3; /* add length until section_length field */

                section = (uint8_t *)malloc(section_length);
                if (section_length >= TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index)
                    memcpy(section, TS_packet + TS_PACKET_HEADER_SIZE + payload_index, TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index);
                else
                    memcpy(section, TS_packet + TS_PACKET_HEADER_SIZE + payload_index, section_length);

                section_filled_length = TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index;
            } else {
                section_length = get_bits(32 + payload_index * 8 + 12, 12, TS_packet);
                section_length += 3; /* add length until section_length field */

                section = (uint8_t *)malloc(section_length);
                memcpy(section, TS_packet + TS_PACKET_HEADER_SIZE + payload_index, TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index);

                section_filled_length = TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index;
            }
        } else { /* payload_unit_start_indicator == 0; there is no section begins in a given TS packet */
            if (section_length - section_filled_length >= TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index) {
                memcpy(section + section_filled_length, TS_packet + TS_PACKET_HEADER_SIZE + payload_index, TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index);

                section_filled_length += TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index;
            } else {
                memcpy(section + section_filled_length, TS_packet + TS_PACKET_HEADER_SIZE + payload_index, section_length - section_filled_length);

                section_filled_length = section_length;
            }
        }
    }
}
