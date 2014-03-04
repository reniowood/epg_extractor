#ifndef EPG_MANAGER_H
#define EPG_MANAGER_H

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "linked_list.h"

void em_init();

uint32_t em_get_SDT_version_number(uint32_t table_id);
uint32_t em_get_EIT_version_number(uint32_t table_id);
void em_set_SDT_version_number(uint32_t table_id, uint32_t version_number);
void em_set_EIT_version_number(uint32_t table_id, uint32_t version_number);

void em_store_service(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint8_t *service_name);
void em_store_event(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, unsigned long start_time, uint32_t duration);
void em_store_content_description(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, struct list_node *content_description_list);
void em_store_event_description(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, uint8_t *event_name, uint8_t *event_description);
void em_store_parental_rating(uint32_t original_network_id, uint32_t transport_stream_id, uint32_t service_id, uint32_t event_id, struct list_node *parental_ratings_list);

void em_show_whole_EPG();
void em_show_service_EPG(uint8_t *service_name);
void em_show_now_EPG(unsigned long now_time);

struct Service {
    uint32_t original_network_id;
    uint32_t transport_stream_id;
    uint32_t service_id;
    uint8_t *service_name;
    struct list_node *event_list;

    struct list_node node;
};

struct Event {
    uint32_t original_network_id;
    uint32_t transport_stream_id;
    uint32_t service_id;
    uint32_t event_id;
    int start_time, duration;
    uint8_t *event_name, *event_description;
    struct list_node *content_description_list, *parental_ratings_list;

    struct list_node node;
};

struct VersionNumber {
    uint32_t table_id, version_number;

    struct list_node node;
};

struct list_node *SDT_version_number_list;
struct list_node *EIT_version_number_list;
struct list_node *service_list;

#endif
