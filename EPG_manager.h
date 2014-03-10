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
    
    uint64_t start_time;
    uint32_t duration;

    char *event_name, *event_description;
    struct list_node *content_description_list;

    uint16_t start_year, start_month, start_day;
    uint16_t start_hour, start_minute, start_second;
    uint16_t duration_hour, duration_minute, duration_second;
    uint16_t end_year, end_month, end_day;
    uint16_t end_hour, end_minute, end_second;

    struct list_node node;
};

struct SDTVersionNumber {
    struct Identifier id;

    uint32_t version_number;

    struct list_node node;
};

struct EITVersionNumber {
    struct Identifier id;

    uint32_t version_number;

    struct list_node node;
};

void em_init();
void em_finish();

uint32_t em_get_SDT_version_number(struct Identifier id, uint32_t table_id);
uint32_t em_get_EIT_version_number(struct Identifier id, uint32_t table_id);
void em_set_SDT_version_number(struct Identifier id, uint32_t table_id, uint32_t version_number);
void em_set_EIT_version_number(struct Identifier id, uint32_t table_id, uint32_t version_number);

void em_store_service(struct Identifier id, char *service_name);
void em_store_event(struct Identifier id, uint64_t start_time, uint32_t duration);
void em_store_content_description(struct Identifier id, struct list_node *content_description_list);
void em_store_event_description(struct Identifier id, char *event_name, char *event_description);

void em_show_whole_EPG();
void em_show_service_EPG(char *service_name);
void em_show_now_EPG(char *now_time);

void em_show_service(struct Service *service);
void em_show_event(struct Event *event);

struct list_node *SDT_version_number_list;
struct list_node *EIT_version_number_list;
struct list_node *service_list;

#endif
