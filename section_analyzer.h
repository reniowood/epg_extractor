#ifndef SECTION_ANALYZER_H
#define SECTION_ANALYZER_H

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "linked_list.h"

#define ACTUAL_SDT_ID 0x42
#define MIN_ACTUAL_EVENT_SCHEDULE_TABLE_ID  0x50
#define MAX_ACTUAL_EVENT_SCHEDULE_TABLE_ID  0x5f

#define SERVICE_DESCRIPTOR_TAG              0x48
#define CONTENT_DESCRIPTOR_TAG              0x54
#define SHORT_EVENT_DESCRIPTOR_TAG          0x4d
#define PARENTAL_RATING_DESCRIPTOR_TAG      0x55

void sa_analyze_SDT_section(char *section, int section_length);
void sa_analyze_EIT_section(char *section, int section_length);

struct ContentDescription {
    char *content_description_level_1, *content_description_level_2;
};

struct ParentalRating {
    int country_code;
    char rating;
};

char content_description_table[0xf][0xf+1][] = 
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
    {"user defined", "user defined", },
};

#endif
