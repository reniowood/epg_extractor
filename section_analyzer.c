#include "section_analyzer.h"

char content_description_table[0xf][0xf+1][MAX_CONTENT_DESCRIPTION_LENGTH] = 
{
    {"undefined", "undefined", },
    {"Movie/Drama", "general", "detective/thriller", "advanture/western/war", "science fiction/fantasy/horror", "comedy", "soap/melodrama/folkloric", "romance", "serious/classical/religious/historical movie/drama", "adult movie/drama", },
    {"News/Current affairs", "general", "news/weather report", "news magazine", "documentary", "discussion/interview/debate", },
    {"show/Game show", "general", "game show/quiz/content", "variety show", "talk show", },
    {"Sports", "general", "special events", "sports magazines", "football/soccer", "tennis/squash", "team sports", "athletics", "motor sport", "water sport", "winter sports", "equestrian", "martial sports", },
    {"Children's/Youth programmes", "general", "pre-school children's programmes", "entertainment for 6 to 14", "entertainment for 10 to 16", "informational/educational/school", "cartoons/puppets", },
    {"Music/Ballet/Dance", "general", "rock/pop", "serious music/classical music", "folk/traditional music", "jazz", "musical/opera", "ballet", },
    {"Arts/Culture", "general", "performing arts", "fine arts", "religion", "popular culture/traditional arts", "literature", "film/cinema", "experimental film/video", "broadcasting/press", "new media", "arts/culture magazines", "fashion", },
    {"Social/Political issues/Economics", "general", "magazines/reports/documentary", "economics/social advisory", "remarkable people", },
    {"Education/Science/Factual topics", "general", "nature/animals/environment", "technology/natural sciences", "medicine/physiology/psychology", "foreign countries/expeditions", "social/spiritual sciences", "furthre education", },
    {"Leisure hobbies", "general", "tourism/travel", "handcraft", "motoring", "fitness and health", "cooking", "advertisement/shopping", "gradening", },
    {"Special characteristics", "original language", "black and white", "unpublished", "live broadcast", },
    {"reserved", "reserved", },
    {"reserved", "reserved", },
    {"reserved", "reserved", },
    {"user defined", "user defined", }
};

void sa_analyze_SDT_section(uint8_t *section, uint32_t section_length) {
    struct Identifier id;
    uint32_t table_id;
    uint32_t table_version_number, section_version_number;
    uint32_t descriptor_tag, descriptor_length, descriptors_loop_length;
    uint32_t service_provider_name_length, service_name_length;
    char *service_name = NULL;
    int section_bytes_scaned, descriptors_loop_bytes_scaned;

    table_id = get_bits(0, 8, section);
    if (table_id != ACTUAL_SDT_ID)
        return;

    id = init_id();
    id.transport_stream_id = get_bits(24, 16, section);
    id.original_network_id = get_bits(64, 16, section);

    /*
    section_number = get_bits(48, 8, section);
    last_section_number = get_bits(56, 8, section);
    section_version_number = get_bits(42, 5, section);

    table_version_number = em_get_SDT_version_number(original_network_id, transport_stream_id, service_id, table_id);

    if (table_version_number != (uint32_t)-1 && section_version_number != (table_version_number + 1) % 32)
        return;
    else
        em_set_SDT_version_number(original_network_id, transport_stream_id, service_id, table_id, section_version_number);
    */

    section += 11;
    section_bytes_scaned = 11;
    while (section_bytes_scaned < section_length - 4) {
        id.service_id = get_bits(0, 16, section);
        descriptors_loop_length = get_bits(28, 12, section);

        section += 5;
        descriptors_loop_bytes_scaned = 0;
        while (descriptors_loop_bytes_scaned < descriptors_loop_length) {
            descriptor_tag = get_bits(0, 8, section);
            descriptor_length = get_bits(8, 8, section);

            if (descriptor_tag == SERVICE_DESCRIPTOR_TAG) {
                service_provider_name_length = get_bits(24, 8, section);

                section += 4 + service_provider_name_length;

                service_name_length = get_bits(0, 8, section);
                section++;
                service_name = (char *)malloc(service_name_length + 1);
                memcpy(service_name, section, service_name_length);
                service_name[service_name_length] = '\0';

                section += service_name_length;
            } else {
                section += 2 + descriptor_length;
            }

            descriptors_loop_bytes_scaned += 2 + descriptor_length;
        }

        section_bytes_scaned += 5 + descriptors_loop_length;

        em_store_service(id, service_name);
    }
}

void sa_analyze_EIT_section(uint8_t *section, uint32_t section_length) {
    struct Identifier id;
    uint32_t table_id;
    uint32_t section_version_number, table_version_number;
    uint8_t section_number, last_section_number;

    uint8_t descriptor_tag, descriptor_length;
    uint32_t descriptors_loop_length;
    int section_bytes_scaned, descriptors_loop_bytes_scaned, descriptor_data_bytes_scaned;

    uint32_t event_name_length, text_length;
    uint8_t *event_name, *event_description;
    uint64_t start_time;
    uint32_t duration;

    struct list_node *content_description_list;
    struct ContentDescription *content_description;
    uint8_t content_nibble_level_1, content_nibble_level_2;

    int country_code;
    uint8_t rating;

    table_id = get_bits(0, 8, section);
    if (table_id < MIN_ACTUAL_EVENT_SCHEDULE_TABLE_ID || table_id > MAX_ACTUAL_EVENT_SCHEDULE_TABLE_ID)
        return;

    id = init_id();

    id.service_id = get_bits(24, 16, section);
    id.transport_stream_id = get_bits(64, 16, section);
    id.original_network_id = get_bits(80, 16, section);

    /*
    section_number = get_bits(48, 8, section);
    last_section_number = get_bits(56, 8, section);
    section_version_number = get_bits(42, 5, section);
    table_version_number = em_get_EIT_version_number(original_network_id, transport_stream_id, service_id, table_id);

    if (table_version_number != (uint32_t)-1 && section_version_number != (table_version_number + 1) % 32)
        return;
    else
        em_set_EIT_version_number(original_network_id, transport_stream_id, service_id, table_id, section_version_number);
    */

    section += 14;
    section_bytes_scaned = 14;
    while (section_bytes_scaned < section_length - 4) {
        id.event_id = get_bits(0, 16, section);
        start_time = get_bits(16, 40, section);
        duration = get_bits(56, 24, section);
        event_name = NULL;
        event_description = NULL;
        content_description_list = NULL;

        descriptors_loop_length = get_bits(84, 12, section);
        section += 12;
        if (descriptors_loop_length == 0) {
            section_bytes_scaned += 12;

            continue;
        }

        descriptors_loop_bytes_scaned = 0;
        while (descriptors_loop_bytes_scaned < descriptors_loop_length) {
            descriptor_tag = get_bits(0, 8, section);
            descriptor_length = get_bits(8, 8, section);

            if (descriptor_tag == CONTENT_DESCRIPTOR_TAG) {
                content_description_list = (struct list_node *)malloc(sizeof(struct list_node));
                init_list(content_description_list);

                descriptor_data_bytes_scaned = 0;
                while (descriptor_data_bytes_scaned < descriptor_length) {
                    content_description = (struct ContentDescription *)malloc(sizeof(struct ContentDescription));
                    
                    content_nibble_level_1 = get_bits(16 + descriptor_data_bytes_scaned * 8, 4, section);
                    content_nibble_level_2 = get_bits(16 + descriptor_data_bytes_scaned * 8 + 4, 4, section);

                    content_description->content_description_level_1 = (char *)malloc(strlen(content_description_table[content_nibble_level_1][0]) + 1);
                    strcpy(content_description->content_description_level_1, content_description_table[content_nibble_level_1][0]);

                    if (content_description_table[content_nibble_level_1][content_nibble_level_2 + 1] == 0)
                        content_description->content_description_level_2 = NULL;
                    else {
                        content_description->content_description_level_2 = (char *)malloc(strlen(content_description_table[content_nibble_level_1][content_nibble_level_2 + 1]) + 1);
                        strcpy(content_description->content_description_level_2, content_description_table[content_nibble_level_1][content_nibble_level_2 + 1]);
                    }

                    add_data_tail(content_description, content_description_list);

                    descriptor_data_bytes_scaned += 2;
                }

                em_store_content_description(id, content_description_list);

                section += 2 + descriptor_length;
            } else if (descriptor_tag == SHORT_EVENT_DESCRIPTOR_TAG) {
                event_name_length = get_bits(40, 8, section);
                section += 6;
                event_name = (uint8_t *)malloc(event_name_length + 1);
                memcpy(event_name, section, event_name_length);
                event_name[event_name_length] = '\0';
                section += event_name_length;

                text_length = get_bits(0, 8, section);
                section++;

                event_description = (uint8_t *)malloc(text_length + 1);
                memcpy(event_description, section, text_length);
                event_description[text_length] = '\0';

                em_store_event_description(id, event_name, event_description);

                section += text_length;
            } else {
                section += 2 + descriptor_length;
            }

            descriptors_loop_bytes_scaned += 2 + descriptor_length;
        }

        section_bytes_scaned += 12 + descriptors_loop_length;

        em_store_event(id, start_time, duration);
    }
}
