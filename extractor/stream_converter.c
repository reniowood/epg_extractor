#include "stream_converter.h"

int sc_convert_stream_to_packet(char filename[]) {
    FILE *TS_file;

    uint8_t *TS_packet = NULL;
    uint8_t TS_packet_sync_byte;

    TS_file = fopen(filename, "r");

    if (TS_file == NULL) {
        printf("Opening TS file is failed.\n");

        return -1;
    }

    while (!feof(TS_file)) {
        fread(&TS_packet_sync_byte, 1, 1, TS_file);
        if (TS_packet_sync_byte == 0x47) {
            TS_packet = (uint8_t *)malloc(TS_PACKET_SIZE);
            TS_packet[0] = TS_packet_sync_byte;

            if (fread(TS_packet + 1, TS_PACKET_SIZE - 1, 1, TS_file) == 0) {
                free(TS_packet);

                break;
            } else {
                pa_analyze_packet(TS_packet);

                free(TS_packet);
            }
        }
    }

    fclose(TS_file);

    return 0;
}
