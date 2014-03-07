#include "EPG_manager.h"
#include "section_analyzer.h"

void em_init() {
    SDT_version_number_list = (struct list_node *)malloc(sizeof(struct list_node));
    EIT_version_number_list = (struct list_node *)malloc(sizeof(struct list_node));
    service_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(SDT_version_number_list);
    init_list(EIT_version_number_list);
    init_list(service_list);
}

uint32_t em_get_SDT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t table_id) {
    struct list_node *version_number_entry = NULL;
    struct SDTVersionNumber *version_number = NULL;

    list_for_each(version_number_entry, SDT_version_number_list) {
        version_number = get_data(version_number_entry, struct SDTVersionNumber);

        if (version_number->original_network_id == original_network_id &&
            version_number->transport_stream_id == transport_stream_id &&
            version_number->table_id == table_id)
                return version_number->version_number;
    }

    return -1;
}

void em_finish() {
    struct list_node *entry, *old_entry, *event_entry, *old_event_entry, *content_description_entry, *old_content_description_entry;
    struct SDTVersionNumber *SDT_version_number;
    struct EITVersionNumber *EIT_version_number;
    struct Service *service;
    struct Event *event;
    char *service_name, *event_name, *event_description;
    struct ContentDescription *content_description;

    for (entry=SDT_version_number_list->next; entry!=SDT_version_number_list; ) {
        SDT_version_number = get_data(entry, struct SDTVersionNumber);

        old_entry = entry->next;
        free(SDT_version_number);
        entry = old_entry;
    }
    free(SDT_version_number_list);

    for (entry=EIT_version_number_list->next; entry!=EIT_version_number_list; ) {
        EIT_version_number = get_data(entry, struct EITVersionNumber);

        old_entry = entry->next;
        free(EIT_version_number);
        entry = old_entry;
    }
    free(EIT_version_number_list);

    for (entry=service_list->next; entry!=service_list; ) {
        service = get_data(entry, struct Service);

        for (event_entry=service->event_list->next; event_entry!=service->event_list; ) {
            event = get_data(event_entry, struct Event);

            free(event->event_name);
            free(event->event_description);

            if (event->content_description_list) {
                for (content_description_entry=event->content_description_list->next; content_description_entry!=event->content_description_list; ) {
                    content_description = get_data(content_description_entry, struct ContentDescription);

                    old_content_description_entry = content_description_entry->next;
                    free(content_description->content_description_level_1);
                    free(content_description->content_description_level_2);
                    free(content_description);
                    content_description_entry = old_content_description_entry;
                }
                free(event->content_description_list);
            }

            old_event_entry = event_entry->next;
            free(event);
            event_entry = old_event_entry;
        }

        free(service->event_list);
        free(service->service_name);

        old_entry = entry->next;
        free(service);
        entry = old_entry;
    }
    free(service_list);
}

uint32_t em_get_EIT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t table_id) {
    struct list_node *version_number_entry = NULL;
    struct EITVersionNumber *version_number = NULL;

    list_for_each(version_number_entry, EIT_version_number_list) {
        version_number = get_data(version_number_entry, struct EITVersionNumber);

        if (version_number->original_network_id == original_network_id &&
            version_number->transport_stream_id == transport_stream_id &&
            version_number->service_id == service_id &&
            version_number->table_id == table_id)
                return version_number->version_number;
    }

    return -1;
}

void em_set_SDT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t table_id, uint32_t version_number) {
    struct SDTVersionNumber *new_version_number = NULL;

    new_version_number = (struct SDTVersionNumber *)malloc(sizeof(struct SDTVersionNumber));

    new_version_number->original_network_id = original_network_id;
    new_version_number->transport_stream_id = transport_stream_id;
    new_version_number->table_id = table_id;
    new_version_number->version_number = version_number;

    add_data_tail(new_version_number, SDT_version_number_list);
}

void em_set_EIT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t table_id, uint32_t version_number) {
    struct EITVersionNumber *new_version_number;

    new_version_number = (struct EITVersionNumber *)malloc(sizeof(struct EITVersionNumber));

    new_version_number->original_network_id = original_network_id;
    new_version_number->transport_stream_id = transport_stream_id;
    new_version_number->service_id = service_id;
    new_version_number->table_id = table_id;
    new_version_number->version_number = version_number;

    add_data_tail(new_version_number, EIT_version_number_list);
}

void em_store_service(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, char *service_name) {
    struct Service *service = NULL;
    struct list_node *service_entry, *event_list;

    /* find existing service data first */
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            service->service_name = service_name;

            return;
        }
    }

    service = (struct Service *)malloc(sizeof(struct Service));
    service->original_network_id = original_network_id;
    service->transport_stream_id = transport_stream_id;
    service->service_id = service_id;
    service->service_name = service_name;
    service->event_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(service->event_list);

    add_data_tail(service, service_list);
}

void em_store_event(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, uint64_t start_time, uint32_t duration) {
    struct Event *new_event = NULL, *event;
    struct Service *service;
    struct list_node *service_entry, *event_entry, *event_list;

    new_event = (struct Event *)malloc(sizeof(struct Event));
    new_event->original_network_id = original_network_id;
    new_event->transport_stream_id = transport_stream_id;
    new_event->service_id = service_id;
    new_event->event_id = event_id;
    new_event->start_time = start_time;
    new_event->duration = duration;

    new_event->event_name = NULL;
    new_event->event_description = NULL;

    new_event->content_description_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(new_event->content_description_list);

    event_list = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);
        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            break;
        }
    }

    if (event_list == NULL) { /* make dummy service data */
        service = (struct Service *)malloc(sizeof(struct Service));
        service->original_network_id = original_network_id;
        service->transport_stream_id = transport_stream_id;
        service->service_id = service_id;
        service->service_name = NULL;
        service->event_list = (struct list_node *)malloc(sizeof(struct list_node));

        init_list(service->event_list);

        event_list = service->event_list;

        add_data_tail(service, service_list);
    }

    list_for_each(event_entry, event_list) {
        event = get_data(event_entry, struct Event);

        if (new_event->start_time < event->start_time) {
            add_data_before(new_event, event_entry, event_list);
            new_event = NULL;

            break;
        }
    }

    if (new_event)
        add_data_tail(new_event, event_list);
}

void em_store_content_description(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, struct list_node *content_description_list) {
    struct list_node *service_entry, *event_entry, *event_list;
    struct Service *service;
    struct Event *event;

    event = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);
        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            list_for_each(event_entry, event_list) {
                event = get_data(event_entry, struct Event);
                if (event->event_id == event_id) {
                    event->content_description_list = content_description_list;

                    return;
                }
            }
        }
    }
}

void em_store_event_description(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, char *event_name, char *event_description) {
    struct list_node *service_entry, *event_entry, *event_list;
    struct Service *service;
    struct Event *event;

    event = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);
        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            list_for_each(event_entry, event_list) {
                event = get_data(event_entry, struct Event);

                if (event->event_id == event_id) {
                    event->event_name = event_name;
                    event->event_description = event_description;

                    return;
                }
            }
        }
    }
}

void em_show_whole_EPG() {
    struct list_node *service_entry, *event_entry, *content_description_entry;
    struct Service *service;
    struct Event *event;
    struct ContentDescription *content_description;

    int i;

    service = NULL;
    event = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);
        em_show_service(service);

        list_for_each(event_entry, service->event_list) {
            event = get_data(event_entry, struct Event);

            if (event->event_name) {
                em_show_event(event);
            }
        }

        putchar('\n');
    }
}

void em_show_service_EPG(char *service_name) {
    struct list_node *service_entry, *event_entry, *content_description_entry;
    struct Service *service;
    struct Event *event;
    struct ContentDescription *content_description;

    uint16_t MJD, year, month, date, k;

    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        if (!strcmp(service_name, service->service_name)) {
            em_show_service(service);

            list_for_each(event_entry, service->event_list) {
                event = get_data(event_entry, struct Event);

                if (event->event_name) {
                    em_show_event(event);
                }
            }
        }
    }
}

void em_show_now_EPG(char *now_time) {
    return;
}

void em_show_date_time(uint64_t start_time, uint32_t duration) {
    uint16_t MJD, year, month, date, k;
    uint16_t start_hour, start_minute, start_second;
    uint16_t duration_hour, duration_minute, duration_second;
    uint16_t end_hour, end_minute, end_second;

    MJD = (start_time >> 24) & 0xffff;
    year = (MJD - 15078.2) / 365.25;
    month = (MJD - 14956.1 - (int)(year * 365.25)) / 30.6001;
    date = MJD - 14956 - (int)(year * 365.25) - (int)(month * 30.6001);
    if (month == 14 || month == 15)
        k = 1;
    else
        k = 0;
    year += k;
    year += 1900;
    month -= 1 + 12 * k;

    start_time &= 0xffffff;
    duration &= 0xffffff;

    start_hour = ((start_time >> 16) & 0xff) / 16 * 10+ ((start_time >> 16) & 0xff) % 16;
    start_minute = ((start_time >> 8) & 0xff) / 16 * 10 + ((start_time >> 8) & 0xff) % 16;
    start_second = (start_time & 0xff) / 16 + (start_time & 0xff) % 16;

    duration_hour = ((duration >> 16) & 0xff) / 16 * 10 + ((duration >> 16) & 0xff) % 16;
    duration_minute = ((duration >> 8) & 0xff) / 16 * 10 + ((duration >> 8) & 0xff) % 16;
    duration_second = (duration & 0xff) / 16 * 10 + (duration & 0xff) % 16;

    end_second = start_second + duration_second;
    end_minute = start_minute + duration_minute + end_second / 60;
    end_hour = start_hour + duration_hour + end_minute / 60;

    end_hour %= 24;
    end_minute %= 60;
    end_second %= 60;

    printf("%d-%d-%d ", year, month, date);
    printf("%02d:%02d:%02d - %02d:%02d:%02d (%02d:%02d:%02d)", start_hour, start_minute, start_second, end_hour, end_minute, end_second, duration_hour, duration_minute, duration_second);
}

void em_show_service(struct Service *service) {
    printf("SERVICE_IDENTIFIER: (%d, %d, %d)\n", service->original_network_id, service->transport_stream_id, service->service_id);
    printf("SERVICE_NAME: %s\n", service->service_name);
}

void em_show_event(struct Event *event) {
    struct list_node *content_description_entry;
    struct ContentDescription *content_description;

    printf("EVENT_IDENTIFIER: (%d, %d, %d, %d)\n", event->original_network_id, event->transport_stream_id, event->service_id, event->event_id);
    printf("EVENT_NAME: %s\n", event->event_name);

    printf("CONTENT_DESCRIPTION: ");
    list_for_each(content_description_entry, event->content_description_list) {
        content_description = get_data(content_description_entry, struct ContentDescription);
        printf("(%s / %s) ", content_description->content_description_level_1, content_description->content_description_level_2);
    }
    putchar('\n');

    printf("TIME: ");
    em_show_date_time(event->start_time, event->duration);
    putchar('\n');

    printf("EVENT_DESCRIPTION: %s\n", event->event_description);
}
