#ifndef PACKET_ANALYZER_H
#define PACKET_ANALYZER_H

#include <stdlib.h>

#include "common.h"
#include "section_analyzer.h"

#define PID_SDT 0x11
#define PID_EIT 0x12

void pa_analyze_packet(uint8_t *TS_packet);

#endif
