#include <stdio.h>

#include "stream_converter.h"
#include "EPG_manager.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Enter the name of the .ts file.\n");

        return -1;
    }

    em_init();

    if (sc_convert_stream_to_packet(argv[1]) == -1) {
        printf("Extracting is failed.\n");

        em_finish();

        return -1;
    }

    em_show_whole_EPG();

    em_finish();

    return 0;
}
