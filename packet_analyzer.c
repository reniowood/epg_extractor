#include "packet_analyzer.h"
#include "section_analyzer.h"

void pa_analyze_packet(char *TS_packet) {
    static unsigned int PID;
    unsigned int adaptation_field_control, adaptation_field_length, payload_index, payload_unit_start_indicator, pointer_field;
    static unsigned int section_length = 0, section_filled_length = 0;
    static char *section = NULL;

    /* see PID of packet in the header and check whether the packet is for SDT/EIT or not. */
    PID = *(unsigned int *)TS_packet;
    PID = (PID >> 8) & 0x1fff;

    if ((PID == PID_SDT) || (PID == PID_EIT)) {
        adaptation_field_control = *(unsigned int *)TS_packet;
        adaptation_field_control = (adaptation_field_control >> 4) & 0x02;
        if (adaptation_field_control == 1) /* no adaptation field */
            payload_index = 0;
        else if (adaptation_field_control == 3) {
            adaptation_field_length = *((unsigned int *)TS_packet + 1);
            adaptation_field_length = (adaptation_field_length >> 24) & 0xff;
            payload_index = 1 + adaptation_field_length;
        }

        payload_unit_start_indicator = *(unsigned int *)TS_packet;
        payload_unit_start_indicator = (payload_unit_start_indicator >> 22) & 0x1;

        /* payload_unit_start_indicator == 1; at least one section begins in a given TS packet */
        if (payload_unit_start_indicator == 1) {
            pointer_field = *((unsigned int *)TS_packet + 1 + payload_index);
            pointer_field = (pointer_field >> 24) & 0xff;
            payload_index++;

            if (pointer_field == 0) {
                section_length = *((unsigned int *)TS_packet + 1 + payload_index);
                section_length = (section_length >> 8) & 0x0fff;
                section_length += 3; /* add length until section_length field */

                section = (char *)malloc(section_length);
                memcpy(section, TS_packet + payload_index, TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index);

                section_filled_length = TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index;
            } else {
                memcpy(section + section_filled_length, TS_packet + payload_index, pointer_field);
                payload_index += pointer_field;

                if (PID == PID_SDT) {
                    sa_analyze_SDT_section(section, section_length);
                } else { /* PID_EIT */
                    sa_analyze_EIT_section(section, section_length);
                }

                section_length = *((unsigned int *)TS_packet + 1 + payload_index);
                section_length = (section_length >> 8) & 0x0fff;
                section_length += 3; /* add length until section_length field */

                section = (char *)malloc(section_length);
                memcpy(section, TS_packet + payload_index, TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index);

                section_filled_length = TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index;
            }
        } else { /* payload_unit_start_indicator == 0; there is no section begins in a given TS packet */
            memcpy(section + section_filled_length, TS_packet + payload_index, TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index);
            section_filled_length += TS_PACKET_SIZE - TS_PACKET_HEADER_SIZE - payload_index;
        }
    }
}
