#ifndef EPG_MANAGER_H
#define EPG_MANAGER_H

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "linked_list.h"

#include "section_analyzer.h"

struct Service {
    struct Identifier id;

    char *service_name;
    struct list_node *event_list;

    struct list_node node;
};

struct Event {
    struct Identifier id;

    char *event_name, *event_description;
    struct list_node *content_description_list;
    struct Date start, duration, end;

    struct list_node node;
};

struct Version {
    struct Identifier id;

    uint32_t version_number;
    int completed;

    struct list_node node;
};

void em_init();
void em_finish();

struct Version *em_get_version(struct Identifier id);
void em_set_version_number(struct Identifier id, uint32_t version_number);
void em_set_version_completed(struct Identifier id, int completed);

void em_store_service(struct Identifier id, char *service_name);
void em_store_event(struct Identifier id, uint64_t start_time, uint32_t duration);
void em_store_content_description(struct Identifier id, struct list_node *content_description_list);
void em_store_event_description(struct Identifier id, char *event_name, char *event_description);

void em_show_whole_EPG();
void em_show_service_EPG(char *service_name);
void em_show_now_EPG(char *now_time);

void em_show_service(struct Service *service);
void em_show_event(struct Event *event);

struct Date translate_start_time(uint64_t start_time);

struct list_node *version_list;
struct list_node *service_list;

#endif
