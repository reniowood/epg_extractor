#include "EPG_manager.h"

void em_init() {
    version_list = (struct list_node *)malloc(sizeof(struct list_node));
    service_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(version_list);
    init_list(service_list);
}

void em_finish() {
    struct list_node *entry, *old_entry, *event_entry, *old_event_entry, *content_description_entry, *old_content_description_entry;
    struct Version *version;
    struct Service *service;
    struct Event *event;
    char *service_name, *event_name, *event_description;
    struct ContentDescription *content_description;

    for (entry=version_list->next; entry!=version_list; ) {
        version = get_data(entry, struct Version);

        old_entry = entry->next;
        free(version);
        entry = old_entry;
    }
    free(version_list);

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

struct Version *em_get_version(struct Identifier id) {
    struct list_node *version_number_entry = NULL;
    struct Version *version = NULL;

    list_for_each(version_number_entry, version_list) {
        version = get_data(version_number_entry, struct Version);

        if (compare_id(&version->id, &id))
            return version;
    }

    version = (struct Version *)malloc(sizeof(struct Version));

    version->id = id;
    version->version_number = (uint32_t)-1;
    version->completed = 0;

    add_data_tail(version, version_list);

    return version;
}

void em_set_version_number(struct Identifier id, uint32_t version_number) {
    struct list_node *version_number_entry = NULL;
    struct Version *version;

    list_for_each(version_number_entry, version_list) {
        version = get_data(version_number_entry, struct Version);

        if (compare_id(&version->id, &id))
            version->version_number = version_number;
    }
}

void em_set_version_completed(struct Identifier id, int completed) {
    struct list_node *version_number_entry = NULL;
    struct Version *version;

    list_for_each(version_number_entry, version_list) {
        version = get_data(version_number_entry, struct Version);

        if (compare_id(&version->id, &id))
            version->completed = completed;
    }
}

void em_store_service(struct Identifier id, char *service_name) {
    struct Service *service = NULL;
    struct list_node *service_entry, *event_list;

    /* find existing service data first */
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        if (compare_id(&service->id, &id)) {
            service->service_name = service_name;

            return;
        }
    }

    service = (struct Service *)malloc(sizeof(struct Service));
    service->id = id;
    service->service_name = service_name;
    service->event_list = (struct list_node *)malloc(sizeof(struct list_node));

    init_list(service->event_list);

    add_data_tail(service, service_list);
}

void em_store_event(struct Identifier id, uint64_t start_time, uint32_t duration) {
    struct Event *new_event = NULL, *event;
    struct Service *service;
    struct list_node *service_entry, *event_entry, *event_list;

    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        if (compare_id(&service->id, &id)) {
            event_list = service->event_list;

            list_for_each(event_entry, event_list) {
                event = get_data(event_entry, struct Event);

                if (compare_id(&event->id, &id))
                    return;
            }
        }
    }

    new_event = (struct Event *)malloc(sizeof(struct Event));
    new_event->id = id;

    new_event->event_name = NULL;
    new_event->event_description = NULL;

    new_event->content_description_list = (struct list_node *)malloc(sizeof(struct list_node));

    new_event->start = init_date();
    new_event->end = init_date();
    new_event->duration = init_date();

    get_ymd(&new_event->start, start_time);
    get_hms(&new_event->start, start_time);
    get_hms(&new_event->duration, duration);
    add_date(&new_event->end, &new_event->start, &new_event->duration);

    init_list(new_event->content_description_list);

    event_list = NULL;
    id.event_id = -1;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);
            
        if (compare_id(&service->id, &id)) {
            event_list = service->event_list;

            break;
        }
    }

    if (event_list == NULL) { /* make dummy service data */
        service = (struct Service *)malloc(sizeof(struct Service));
        service->id = id;
        service->service_name = NULL;
        service->event_list = (struct list_node *)malloc(sizeof(struct list_node));

        init_list(service->event_list);

        event_list = service->event_list;

        add_data_tail(service, service_list);
    }

    list_for_each(event_entry, event_list) {
        event = get_data(event_entry, struct Event);

        if (compare_date(&new_event->start, &event->start)) {
            add_data_before(new_event, event_entry, event_list);
            new_event = NULL;

            break;
        }
    }

    if (new_event)
        add_data_tail(new_event, event_list);
}

void em_store_content_description(struct Identifier id, struct list_node *content_description_list) {
    struct list_node *service_entry, *event_entry, *old_content_description_entry, *content_description_entry, *event_list;
    struct Service *service;
    struct Event *event;
    struct ContentDescription *content_description;

    event = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        if (compare_id(&service->id, &id)) {
            event_list = service->event_list;

            list_for_each(event_entry, event_list) {
                event = get_data(event_entry, struct Event);

                if (compare_id(&event->id, &id)) {
                    if (event->content_description_list != NULL) {
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
                    event->content_description_list = content_description_list;

                    return;
                }
            }
        }
    }
}

void em_store_event_description(struct Identifier id, char *event_name, char *event_description) {
    struct list_node *service_entry, *event_entry, *event_list;
    struct Service *service;
    struct Event *event;

    event = NULL;
    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);
        if (compare_id(&service->id, &id)) {
            event_list = service->event_list;

            list_for_each(event_entry, event_list) {
                event = get_data(event_entry, struct Event);

                if (compare_id(&event->id, &id)) {
                    if (event->event_name != NULL)
                        free(event->event_name);
                    event->event_name = event_name;

                    if (event->event_description != NULL)
                        free(event->event_description);
                    event->event_description = event_description;

                    return;
                }
            }
        }
    }
}

void em_show_whole_EPG() {
    struct list_node *service_entry, *event_entry;
    struct Service *service;
    struct Event *event;

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
    struct list_node *service_entry, *event_entry;
    struct Service *service;
    struct Event *event;

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
    struct list_node *service_entry, *event_entry;
    struct Service *service;
    struct Event *event;

    struct Date now;

    char *token;

    token = strtok(now_time, "-");
    now.year = atoi(token);
    token = strtok(NULL, "-");
    now.month = atoi(token);
    token = strtok(NULL, "-");
    now.day = atoi(token);
    token = strtok(NULL, "-");
    now.hour = atoi(token);
    token = strtok(NULL, "-");
    now.minute = atoi(token);
    token = strtok(NULL, "-");
    now.second = atoi(token);

    list_for_each(service_entry, service_list) {
        service = get_data(service_entry, struct Service);

        em_show_service(service);

        list_for_each(event_entry, service->event_list) {
            event = get_data(event_entry, struct Event);

            if (event->event_name && compare_date(&event->start, &now) && compare_date(&now, &event->end))
                em_show_event(event);
        }
    }
}

void em_show_service(struct Service *service) {
    printf("SERVICE_IDENTIFIER: (%d, %d, %d)\n", service->id.original_network_id, service->id.transport_stream_id, service->id.service_id);
    printf("SERVICE_NAME: %s\n", service->service_name);
}

void em_show_event(struct Event *event) {
    struct list_node *content_description_entry;
    struct ContentDescription *content_description;

    printf("EVENT_IDENTIFIER: (%d, %d, %d, %d)\n", event->id.original_network_id, event->id.transport_stream_id, event->id.service_id, event->id.event_id);
    printf("EVENT_NAME: %s\n", event->event_name);

    printf("CONTENT_DESCRIPTION: ");
    list_for_each(content_description_entry, event->content_description_list) {
        content_description = get_data(content_description_entry, struct ContentDescription);
        printf("(%s / %s) ", content_description->content_description_level_1, content_description->content_description_level_2);
    }
    putchar('\n');

    printf("TIME: ");
    printf("%d-%d-%d ", event->start.year, event->start.month, event->start.day);
    printf("%02d:%02d:%02d - %02d:%02d:%02d (%02d:%02d:%02d)", event->start.hour, event->start.minute, event->start.second, event->end.hour, event->end.minute, event->end.second, event->duration.hour, event->duration.minute, event->duration.second);
    putchar('\n');

    printf("EVENT_DESCRIPTION: %s\n", event->event_description);
}
