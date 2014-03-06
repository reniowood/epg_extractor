#ifndef EPG_MANAGER_H
#define EPG_MANAGER_H

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "linked_list.h"

void em_init();
void em_finish();

uint32_t em_get_SDT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t table_id);
uint32_t em_get_EIT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t table_id);
void em_set_SDT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t table_id, uint32_t version_number);
void em_set_EIT_version_number(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t table_id, uint32_t version_number);

void em_store_service(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, char *service_name);
void em_store_event(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, uint64_t start_time, uint32_t duration);
void em_store_content_description(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, struct list_node *content_description_list);
void em_store_event_description(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, char *event_name, char *event_description);
void em_store_parental_rating(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, struct list_node *parental_rating_list);

void em_show_whole_EPG();
void em_show_service_EPG(char *service_name);
void em_show_now_EPG(char *now_time);

void em_show_date_time(uint64_t start_time, uint32_t duration);

struct Service {
    uint32_t original_network_id;
    uint32_t transport_stream_id;
    uint32_t service_id;
    char *service_name;
    struct list_node *event_list;

    struct list_node node;
};

struct Event {
    uint32_t original_network_id;
    uint32_t transport_stream_id;
    uint32_t service_id;
    uint32_t event_id;
    uint64_t start_time;
    uint32_t duration;
    char *event_name, *event_description;
    struct list_node *content_description_list, *parental_rating_list;

    struct list_node node;
};

struct SDTVersionNumber {
    uint32_t original_network_id, transport_stream_id, table_id, version_number;

    struct list_node node;
};

struct EITVersionNumber {
    uint32_t original_network_id, transport_stream_id, service_id, table_id, version_number;

    struct list_node node;
};

struct list_node *SDT_version_number_list;
struct list_node *EIT_version_number_list;
struct list_node *service_list;

#endif
