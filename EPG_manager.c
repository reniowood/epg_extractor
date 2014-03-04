#include "EPG_manager.h"

void em_init() {
    init_list(SDT_version_number_list);
    init_list(EIT_version_number_list);
    init_list(service_list);
}

unsigned int em_get_SDT_version_number(unsigned int table_id) {
    struct list_node *version_number_entry;
    struct VersionNumber *version_number = NULL;

    list_for_each(version_number_entry, SDT_version_number_list) {
        version_number = version_number_entry->data;

        if (version_number->table_id == table_id)
            return version_number->version_number;
    }

    return -1;
}

unsigned int em_get_EIT_version_number(unsigned int table_id) {
    struct list_node *version_number_entry;
    struct VersionNumber *version_number = NULL;

    list_for_each(version_number_entry, EIT_version_number_list) {
        version_number = version_number_entry->data;

        if (version_number->table_id == table_id)
            return version_number->version_number;
    }

    return -1;
}

void em_set_SDT_version_number(unsigned int table_id, unsigned int version_number) {
    struct VersionNumber *version_number = NULL;

    version_number = (struct VersionNumber *)malloc(sizeof(struct VersionNumber));

    version_number->table_id = table_id;
    version_number->version_number = version_number;

    list_add_tail(version_number, SDT_version_number_list);
}

void em_set_EIT_version_number(unsigned int table_id, unsigned int version_number) {
    struct VersionNumber *version_number;

    version_number = (struct VersionNumber *)malloc(sizeof(struct VersionNumber));

    version_number->table_id = table_id;
    version_number->version_number = version_number;

    list_add_tail(version_number, EIT_version_number_list);
}

void em_store_service(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, char *service_name) {
    struct Service *new_service = NULL;

    new_service = (struct Service *)malloc(sizeof(struct Service));
    new_service->original_network_id = original_network_id;
    new_service->transport_stream_id = transport_stream_id;
    new_service->service_id = service_id;
    new_service->service_name = service_name;
    new_service->event_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(new_service->event_list);

    list_add_tail(new_service, service_list);
}

void em_store_event(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, unsigned long start_time, unsigned int duration) {
    struct Event *new_event = NULL;
    struct Service *service;
    struct list_node *service_list_entry, *event_list;

    new_event = (struct Service *)malloc(sizeof(struct Event));
    new_event->original_network_id = original_network_id;
    new_event->transport_stream_id = transport_stream_id;
    new_event->service_id = service_id;
    new_event->event_id = event_id;
    new_event->start_time = start_time;
    new_event->duration = duration;

    event_list = NULL;
    list_for_each(service_list_entry, service_list) {
        service = service_list_entry->data;

        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            break;
        }
    }

    if (event_list == NULL) {
        printf("EVENT_STORE_ERROR\n");

        return;
    }

    list_add_tail(new_event, event_list);
}

void em_store_content_description(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, struct list_node *content_description_list) {
    struct list_node *service_list_entry, *event_list_entry, *event_list;
    struct Service *service;
    struct Event *event;

    event = NULL;
    list_for_each(service_list_entry, service_list) {
        service = service_list_entry->data;

        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            list_for_each(event_list_entry, event_list) {
                event = event_list_entry->data;

                if (event->event_id == event_id) {
                    event->content_description_list = content_description_list;

                    return;
                }
            }
        }
    }
}

void em_store_event_description(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, char *event_name, char *event_description) {
    struct list_node *service_list_entry, *event_list_entry, *event_list;
    struct Service *service;
    struct Event *event;

    event = NULL;
    list_for_each(service_list_entry, service_list) {
        service = service_list_entry->data;

        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            list_for_each(event_list_entry, event_list) {
                event = event_list_entry->data;

                if (event->event_id == event_id) {
                    event->event_name = event_name;
                    event->event_description = event_description;

                    return;
                }
            }
        }
    }
}

void em_store_parental_rating(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, struct list_node *parental_rating_list) {
    struct list_node *service_list_entry, *event_list_entry, *event_list;
    struct Service *service;
    struct Event *event;

    service = NULL;
    event = NULL;
    list_for_each(service_list_entry, service_list) {
        service = service_list_entry->data;

        if (service->original_network_id == original_network_id &&
            service->transport_stream_id == transport_stream_id &&
            service->service_id == service_id) {
            event_list = service->event_list;

            list_for_each(event_list_entry, event_list) {
                event = event_list_entry->data;

                if (event->event_id == event_id) {
                    event->parental_ratings_list = parental_ratings_list;

                    return;
                }
            }
        }
    }
}

void em_show_whole_EPG() {
    struct list_node *service_list_entry, *event_list_entry, *event_list;
    struct Service *service;
    struct Event *event;

    service = NULL;
    event = NULL;
    list_for_each(service_list_entry, service_list) {
        service = service_list_entry->data;

        printf("(%d, %d, %d): %s\n", service->original_network_id, service->transport_stream_id, service->service_id, service->service_name);

        event_list = service->event_list;

        list_for_each(event_list_entry, event_list) {
            event = event_list_entry->data;

            printf("%d: %d(%d) %s - %s\n", event->event_id, event->start_time, event->duration, event->event_name, event->event_description);
        }

        putchar('\n');
    }
}

void em_show_service_EPG(char *service_name) {
    return;
}

void em_show_now_EPG(unsigned long now_time) {
    return;
}
