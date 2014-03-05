#include "EPG_manager.h"

void em_init() {
    SDT_version_number_list = (struct list_node *)malloc(sizeof(struct list_node));
    EIT_version_number_list = (struct list_node *)malloc(sizeof(struct list_node));
    service_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(SDT_version_number_list);
    init_list(EIT_version_number_list);
    init_list(service_list);
}

uint32_t em_get_SDT_version_number(uint32_t table_id) {
    struct list_node *version_number_entry = NULL;
    struct VersionNumber *version_number = NULL;

    list_for_each(version_number_entry, SDT_version_number_list) {
        version_number = get_data(version_number_entry, struct VersionNumber);

        if (version_number->table_id == table_id)
            return version_number->version_number;
    }

    return -1;
}

uint32_t em_get_EIT_version_number(uint32_t table_id) {
    struct list_node *version_number_entry = NULL;
    struct VersionNumber *version_number = NULL;

    list_for_each(version_number_entry, EIT_version_number_list) {
        version_number = get_data(version_number_entry, struct VersionNumber);

        if (version_number->table_id == table_id)
            return version_number->version_number;
    }

    return -1;
}

void em_set_SDT_version_number(uint32_t table_id, uint32_t version_number) {
    struct VersionNumber *new_version_number = NULL;

    new_version_number = (struct VersionNumber *)malloc(sizeof(struct VersionNumber));

    new_version_number->table_id = table_id;
    new_version_number->version_number = version_number;

    add_data_tail(new_version_number, SDT_version_number_list);
}

void em_set_EIT_version_number(uint32_t table_id, uint32_t version_number) {
    struct VersionNumber *new_version_number;

    new_version_number = (struct VersionNumber *)malloc(sizeof(struct VersionNumber));

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

    /* (printf("(%d, %d, %d): %s\n", original_network_id, transport_stream_id, service_id, service_name); */
}

void em_store_event(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, uint64_t start_time, uint32_t duration) {
    struct Event *new_event = NULL;
    struct Service *service;
    struct list_node *service_entry, *event_list;

    new_event = (struct Event *)malloc(sizeof(struct Event));
    new_event->original_network_id = original_network_id;
    new_event->transport_stream_id = transport_stream_id;
    new_event->service_id = service_id;
    new_event->event_id = event_id;
    new_event->start_time = start_time;
    new_event->duration = duration;

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

    add_data_tail(new_event, event_list);

    /* printf("(%d, %d, %d, %d): %ld %d\n", original_network_id, transport_stream_id, service_id, event_id, start_time, duration); */
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

    /* printf("(%d, %d, %d, %d)\n", original_network_id, transport_stream_id, service_id, event_id); */
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

void em_store_parental_rating(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, struct list_node *parental_ratings_list) {
    struct list_node *service_entry, *event_entry, *event_list;
    struct Service *service;
    struct Event *event;

    service = NULL;
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
                    event->parental_ratings_list = parental_ratings_list;

                    return;
                }
            }
        }
    }
}

void em_show_whole_EPG() {
    struct list_node *service_entry, *event_entry, *event_list;
    struct Service *service;
    struct Event *event;

    service = NULL;
    event = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        printf("(%d, %d, %d): %s\n", service->original_network_id, service->transport_stream_id, service->service_id, service->service_name);

        event_list = service->event_list;
        list_for_each(event_entry, event_list) {
            event = get_data(event_entry, struct Event);

            printf("%d: %ld(%d) %s - %s\n", event->event_id, event->start_time, event->duration, event->event_name, event->event_description);
        }

        putchar('\n');
    }
}

void em_show_service_EPG(char *service_name) {
    return;
}

void em_show_now_EPG(uint64_t now_time) {
    return;
}
