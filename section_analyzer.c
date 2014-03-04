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
    uint32_t table_id, version_number;
    uint32_t original_network_id, transport_stream_id, service_id;
    uint32_t descriptor_tag, descriptor_length, descriptors_loop_length;
    uint32_t service_provider_name_length, service_name_length;
    uint8_t *service_name;
    int section_bytes_scaned, descriptors_loop_bytes_scaned;

    table_id = *section;
    if (table_id != ACTUAL_SDT_ID)
        return;

    version_number = *(section + 5);
    version_number = (version_number >> 1) & 0x001f;

    if ((uint32_t)((version_number + 1) % 32) != em_get_SDT_version_number(table_id))
        return;
    else
        em_set_SDT_version_number(table_id, version_number);

    transport_stream_id = *(section + 1);
    transport_stream_id = (transport_stream_id >> 2) & 0xffff;
    original_network_id = *(section + 6);
    original_network_id &= 0xffff; 

    section += 11;
    section_bytes_scaned = 11;
    while (section_bytes_scaned < section_length - 4) {
        service_id = *(uint32_t *)section;
        service_id = (service_id >> 16) & 0xffff;

        descriptors_loop_length = *(uint32_t *)(section + 1);
        descriptors_loop_length &= 0x0fff;

        section += 3;
        descriptors_loop_bytes_scaned = 0;
        while (descriptors_loop_bytes_scaned < descriptors_loop_length) {
            descriptor_tag = *section;
            descriptor_length = *(section + 1);

            if (descriptor_tag == SERVICE_DESCRIPTOR_TAG) {
                service_provider_name_length = *((uint32_t *)section);
                service_provider_name_length &= 0xff;

                section += 4 + service_provider_name_length;

                service_name_length = *section++;
                service_name = (uint8_t *)malloc(service_name_length);
                memcpy(service_name, section, service_name_length);

                section += service_name_length;
            } else {
                section += 2 + descriptor_length;
            }

            descriptors_loop_bytes_scaned += 2 + descriptor_length;
        }

        section_bytes_scaned += 5 + descriptors_loop_length;

        printf("(%d, %d, %d): %s\n", original_network_id, transport_stream_id, service_id, service_name);
        em_store_service(original_network_id, transport_stream_id, service_id, service_name);
    }
}

void sa_analyze_EIT_section(uint8_t *section, uint32_t section_length) {
    uint32_t table_id, version_number;
    uint32_t original_network_id, transport_stream_id, service_id, event_id;
    uint32_t descriptor_tag, descriptor_length, descriptors_loop_length;
    int section_bytes_scaned, descriptors_loop_bytes_scaned, descriptor_data_bytes_scaned;

    uint32_t event_name_length, text_length;
    uint8_t *event_name, *event_description;
    unsigned long start_time;
    uint32_t duration;

    struct list_node *content_description_list;
    struct ContentDescription *content_description;
    uint8_t content_description_bits;

    struct list_node *parental_rating_list;
    struct ParentalRating *parental_rating;
    int country_code;
    uint8_t rating;

    table_id = *section;
    if (table_id < MIN_ACTUAL_EVENT_SCHEDULE_TABLE_ID || table_id > MAX_ACTUAL_EVENT_SCHEDULE_TABLE_ID)
        return;

    version_number = *(section + 5);
    version_number = (version_number >> 1) & 0x001f;

    if ((version_number + 1) % 32 != em_get_EIT_version_number(table_id))
        return;
    else
        em_set_EIT_version_number(table_id, version_number);

    transport_stream_id = *(section + 6);
    transport_stream_id &= 0xffff;
    original_network_id = *(section + 8);
    original_network_id &= 0xffff; 

    section += 14;
    section_bytes_scaned = 14;
    while (section_bytes_scaned < section_length - 4) {
        event_id = *(uint32_t *)section;
        event_id = (event_id >> 16) & 0xffff;

        start_time = *(unsigned long *)section;
        start_time = (start_time >> 8) & 0xffffffffff;

        duration = *(section + 6);
        duration &= 0xffffff;

        descriptors_loop_length = *(section + 8);
        descriptors_loop_length &= 0x0fff;

        em_store_event(original_network_id, transport_stream_id, service_id, event_id, start_time, duration);

        section += 3;
        descriptors_loop_bytes_scaned = 0;
        while (descriptors_loop_bytes_scaned < descriptors_loop_length) {
            descriptor_tag = *section;
            descriptor_length = *(section + 1);

            if (descriptor_tag == CONTENT_DESCRIPTOR_TAG) { // get only first description of content
                init_list(content_description_list);

                descriptor_data_bytes_scaned = 0;
                while (descriptor_data_bytes_scaned < descriptor_length) {
                    content_description = (struct ContentDescription *)malloc(sizeof(struct ContentDescription));
                    
                    content_description_bits = *(section + 2);
                    content_description->content_description_level_1 = (char *)malloc(strlen(content_description_table[(content_description_bits >> 4) & 0x0f][0]) + 1);
                    strcpy(content_description->content_description_level_1, content_description_table[(content_description_bits >> 4) & 0x0f][0]);

                    if (content_description_table[(content_description_bits >> 4) & 0x0f][content_description_bits & 0x0f + 1] == 0)
                        content_description->content_description_level_2 = NULL;
                    else {
                        content_description->content_description_level_2 = (char *)malloc(strlen(content_description_table[(content_description_bits >> 4) & 0x0f][content_description_bits & 0x0f + 1]) + 1);
                        strcpy(content_description->content_description_level_2, content_description_table[(content_description_bits >> 4) & 0x0f][content_description_bits & 0x0f + 1]);
                    }

                    add_data_tail(content_description, content_description_list);
                }

                em_store_content_description(original_network_id, transport_stream_id, service_id, event_id, content_description_list);

                section += 2 + descriptor_length;
            } else if (descriptor_tag == SHORT_EVENT_DESCRIPTOR_TAG) {
                event_name_length = *(section + 5);
                section += 5;
                event_name = (uint8_t *)malloc(event_name_length);
                memcpy(event_name, section, event_name_length);
                section += event_name_length;

                text_length = *section++;
                event_description = (uint8_t *)malloc(text_length);
                memcpy(event_description, section, text_length);

                em_store_event_description(original_network_id, transport_stream_id, service_id, event_id, event_name, event_description);

                section += text_length;
            } else if (descriptor_tag == PARENTAL_RATING_DESCRIPTOR_TAG) {
                init_list(parental_rating_list);

                descriptor_data_bytes_scaned = 0;
                while (descriptor_data_bytes_scaned < descriptor_length) {
                    parental_rating = (struct ParentalRating *)malloc(sizeof(struct ParentalRating));
                    parental_rating->country_code = *(section + 2 + descriptor_data_bytes_scaned);
                    parental_rating->country_code = (country_code >> 8) & 0xffffff;
                    parental_rating->rating = *(section + 2 + descriptor_data_bytes_scaned);
                    parental_rating->rating &= 0xff;

                    add_data_tail(parental_rating, parental_rating_list);

                    descriptor_data_bytes_scaned += 4;
                }

                em_store_parental_rating(original_network_id, transport_stream_id, service_id, event_id, parental_rating_list);

                section += 2 + descriptor_length;
            } else {
                section += 2 + descriptor_length;
            }

            descriptors_loop_bytes_scaned += 2 + descriptor_length;
        }

        section_bytes_scaned += 5 + descriptors_loop_length;
    }
}
