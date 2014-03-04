#ifndef EPG_MANAGER_H
#define EPG_MANAGER_H

#include <stdio.h>

#include "common.h"
#include "linked_list.h"

void em_init();

unsigned int em_get_SDT_version_number(unsigned int table_id);
unsigned int em_get_EIT_version_number(unsigned int table_id);
void em_set_SDT_version_number(unsigned int table_id, unsigned int version_number);
void em_set_EIT_version_number(unsigned int table_id, unsigned int version_number);

void em_store_service(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, char *service_name);
void em_store_event(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, unsigned long start_time, unsigned int duration);
void em_store_content_description(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, struct list_node *content_description_list);
void em_store_event_description(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, char *event_name, char *event_description);
void em_store_parental_rating(unsigned int original_network_id, unsigned int transport_stream_id, unsigned int service_id, unsigned int event_id, struct list_node *parental_rating_list);

void em_show_whole_EPG();
void em_show_service_EPG(char *service_name);
void em_show_now_EPG(unsigned long now_time);

struct Service {
    unsigned int original_network_id;
    unsigned int transport_stream_id;
    unsigned int service_id;
    char *service_name;
    struct list_node *event_list;
};

struct Event {
    unsigned int event_id;
    int start_time, duration;
    char *event_name, *event_description;
    struct list_node *content_description_list, *parental_ratings_list;
};

struct VersionNumber {
    unsigned int table_id, version_number;
};

struct list_node *SDT_version_number_list;
struct list_node *EIT_version_number_list;
struct list_node *service_list;

#endif
