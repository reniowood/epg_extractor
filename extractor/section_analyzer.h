#ifndef SECTION_ANALYZER_H
#define SECTION_ANALYZER_H

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "linked_list.h"

#include "EPG_manager.h"

#define ACTUAL_SDT_ID                       0x42
#define MIN_ACTUAL_EVENT_SCHEDULE_TABLE_ID  0x50
#define MAX_ACTUAL_EVENT_SCHEDULE_TABLE_ID  0x5f

#define SERVICE_DESCRIPTOR_TAG              0x48
#define CONTENT_DESCRIPTOR_TAG              0x54
#define SHORT_EVENT_DESCRIPTOR_TAG          0x4d

#define MAX_CONTENT_DESCRIPTION_LENGTH      64

void sa_analyze_SDT_section(uint8_t *section, uint32_t section_length);
void sa_analyze_EIT_section(uint8_t *section, uint32_t section_length);

struct ContentDescription {
    char *content_description_level_1, *content_description_level_2;

    struct list_node node;
};

#endif
