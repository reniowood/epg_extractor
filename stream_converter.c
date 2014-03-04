#include "stream_converter.h"
#include "packet_analyzer.h"

int sc_convert_stream_to_packet(char filename[]) {
    FILE *TS_file;

    char *TS_packet = NULL;
    char TS_packet_sync_byte;

    TS_file = fopen(filename, 'r');

    if (TS_file == NULL) {
        printf("Opening TS file is failed.\n");

        return -1;
    }

    while (!feof(TS_file)) {
        if (fread(TS_packet_sync_byte, 1, 1, TS_file) == 0x47) {
            TS_packet = (char *)malloc(TS_PACKET_SIZE);
            TS_packet[0] = TS_packet_sync_byte;

            if (fread(TS_packet + 1, TS_PACKET_SIZE - 1, 1, TS_file) == 0)
                break;
            else
                pa_analyze_packet(TS_packet);
        }
    }

    free(TS_packet);

    fclose(TS_file);

    return 0;
}
