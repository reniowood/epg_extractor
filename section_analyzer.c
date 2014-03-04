#include "section_analyzer.h"

void sa_analyze_SDT_section(char *section, unsigned int section_length) {
    unsigned int table_id, version_number;
    unsigned int original_network_id, transport_stream_id, service_id;
    unsigned int descriptor_tag, descriptor_length;
    unsigned int service_provider_name_length, service_name_length;
    char *service_name;
    int section_bytes_scaned, descriptors_loop_bytes_scaned;

    table_id = *section;
    if (table_id != ACTUAL_SDT_ID)
        return;

    version_number = *(section + 5);
    version_number = (version_number >> 1) & 0x001f;

    if ((version_number + 1) % 32 != em_get_SDT_version_number(table_id))
        return;
    else
        em_set_SDT_version_number(table_id, version_number);

    transport_stream_id = *(section + 1);
    transport_stream_id = (transport_stream_id >> 2) & 0xffff;
    original_stream_id = *(section + 6);
    original_stream_id &= 0xffff; 

    section += 11;
    section_bytes_scaned = 11;
    while (section_bytes_scaned < section_length - 4) {
        service_id = *(unsigned int *)section;
        service_id = (service_id >> 16) & 0xffff;

        descriptors_loop_length = *(unsigned int *)(section + 1);
        descriptors_loop_length &= 0x0fff;

        section += 3;
        descriptors_loop_bytes_scaned = 0;
        while (descriptors_loop_bytes_scaned < descriptors_loop_length) {
            descriptor_tag = *section;
            descriptor_length = *(section + 1);

            if (descriptor_tag == SERVICE_DESCRIPTOR_TAG) {
                service_provider_name_length = *((unsigned int *)section);
                service_provider_name_length &= 0xff;

                section += 4 + service_provider_name_length;

                service_name_length = *section++;
                service_name = (char *)malloc(service_name_length);
                memcpy(service_name, section, service_name_length);

                section += service_name_length;
            } else {
                section += 2 + descriptor_length;
            }

            descriptors_loop_bytes_scaned += 2 + descriptor_length;
        }

        section_bytes_scaned += 5 + descriptors_loop_length;

        em_store_service(original_network_id, transport_stream_id, service_id, service_name);
    }
}

void sa_analyze_EIT_section(char *section, unsigned int section_length) {
    unsigned int table_id, version_number;
    unsigned int original_network_id, transport_stream_id, service_id, event_id;
    unsigned int descriptor_tag, descriptor_length;
    int section_bytes_scaned, descriptors_loop_bytes_scaned, descriptor_data_bytes_scaned;

    unsigned int event_name_length, text_length;
    char *event_name, *event_description;
    unsigned long start_time;
    unsigned int duration;

    struct list_node *content_description_list;
    struct ContentDescription *content_description;
    char content_description_bits;

    struct list_node *parental_rating_list;
    struct ParentalRating *parental_rating;
    int country_code;
    char rating;

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
    original_stream_id = *(section + 8);
    original_stream_id &= 0xffff; 

    section += 14;
    section_bytes_scaned = 14;
    while (section_bytes_scaned < section_length - 4) {
        event_id = *(unsigned int *)section;
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
                    content_description = (struct ContentDescription *)malloc(sizeof(ContentDescription));
                    
                    content_description_bits = *(section + 2);
                    content_description->content_description_level_1 = (char *)malloc(strlen(content_description_table[(content_description_table >> 4) & 0x0f][0]) + 1);
                    strcpy(content_description->content_description_level_1, content_description_table[(content_description_table >> 4) & 0x0f][0]);

                    if (content_description_table[(content_description_table >> 4) & 0x0f][content_description_table & 0x0f + 1] == 0)
                        content_description->content_description_level_2 = NULL;
                    else {
                        content_description->content_description_level_2 = (char *)malloc(strlen(content_description_table[(content_description_table >> 4) & 0x0f][content_description_table & 0x0f + 1]) + 1);
                        strcpy(content_description->content_description_level_2, content_description_table[(content_description_table >> 4) & 0x0f][content_description_table & 0x0f + 1]);
                    }

                    add_data_tail(content_description, struct ContentDescription, content_description_list);
                }

                em_store_content_description(original_network_id, transport_stream_id, service_id, event_id, content_description_list);

                section += 2 + descriptor_length;
            } else if (descriptor_tag == SHORT_EVENT_DESCRIPTOR_TAG) {
                event_name_length = *(section + 5);
                section += 5;
                event_name = (char *)malloc(event_name_length);
                memcpy(event_name, section, event_name_length);
                section += event_name_length;

                text_length = *section++;
                event_description = (char *)malloc(text_length);
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

                    add_data_tail(parental_rating, struct ParentalRating, parental_rating_list);

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
