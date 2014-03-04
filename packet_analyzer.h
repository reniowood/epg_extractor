#ifndef PACKET_ANALYZER_H
#define PACKET_ANALYZER_H

#include <stdlib.h>

#include "common.h"

#define PID_SDT 0x11
#define PID_EIT 0x12

void pa_analyze_packet(char *TS_packet);

#endif
