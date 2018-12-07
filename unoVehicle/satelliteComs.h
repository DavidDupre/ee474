// #ifndef _SATELLITE_COMS_H_
// #define _SATELLITE_COMS_H_

// #include "schedule.h"

// typedef struct {
//     char *satelliteCommand;
// } SatelliteComsData;

// extern TCB sateliteComsTCB;

// void satelliteComsInit();

// void satelliteComs(void *satelliteComsData);

// #endif  /* _SATELLITE_COMS_H_ */

#pragma once

#include <stdint.h>
#include "schedule.h"

void satelliteComsInit();