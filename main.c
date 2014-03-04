#include <stdio.h>

#include "stream_converter.h"
#include "EPG_manager.h"

#define MENU_READ_TS_FILE       1
#define MENU_SHOW_WHOLE_EPG     2
#define MENU_SHOW_SERVICE_EPG   3
#define MENU_SHOW_NOW_EPG       4
#define MENU_EXIT               5

#define MAX_FILENAME_LENGTH     256
#define MAX_SERVICE_NAME_LENGTH 1024
#define MAX_TIME_LENGTH         1024

int main() {
    int select_menu;
    int EPG_loaded;

    char TS_filename[MAX_FILENAME_LENGTH], service_name[MAX_SERVICE_NAME_LENGTH], now_time[MAX_TIME_LENGTH];

    select_menu = 0;
    EPG_loaded = 0;

    em_init();

    while (1) {
        printf("1. Read TS file\n");
        printf("2. Show whole EPG data\n");
        printf("3. Show EPG data for a service\n");
        printf("4. Show all events now broadcasting\n");
        printf("5. Exit\n");

        printf("> ");
        scanf("%d", &select_menu);

        switch (select_menu) {
        case MENU_READ_TS_FILE:
            printf("Enter the name of TS file: ");
            scanf("%s", TS_filename);

            if (!sc_convert_stream_to_packet(TS_filename))
                printf("EPG data is extracted.\n");
            else
                printf("Extracting EPG data is failed.\n");

            break;
        case MENU_SHOW_WHOLE_EPG:
            em_show_whole_EPG();

            break;
        case MENU_SHOW_SERVICE_EPG:
            printf("Enter the name of the service: ");
            scanf("%s", service_name);

            em_show_service_EPG(service_name);

            break;
        case MENU_SHOW_NOW_EPG:
            printf("What time is it now (YYYY-MM-DD-HH-MM): ");
            scanf("%s", now_time);

            /* em_show_now_EPG(now_time); */

            break;
        case MENU_EXIT:
            printf("Bye.\n");

            return 0;
        default:
            printf("Enter a number between 0 and 5.\n\n");
        }
    }

    return -1;
}
