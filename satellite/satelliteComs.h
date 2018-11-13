#ifndef _SATELLITE_COMS_H_
#define _SATELLITE_COMS_H_

#include "solarPanel.h"
#include "schedule.h"


typedef struct {
    bool *fuelLow;
    bool *batteryLow;
    SolarPanelState *solarPanelState;
    volatile unsigned int *batteryLevelPtr;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    unsigned int *thrusterCommand;
    char *vehicleResponse;
} SatelliteComsData;


extern TCB satelliteComsTCB;

void satelliteComsInit();

void satelliteComs(void* satelliteComsData);

#endif  /* _SATELLITE_COMS_H_ */