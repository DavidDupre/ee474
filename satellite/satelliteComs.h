#ifndef _SATELLITE_COMS_H_
#define _SATELLITE_COMS_H_

#include "solarPanel.h"
#include "schedule.h"
#include <stdint.h>


typedef struct {
    bool *fuelLow;
    bool *batteryLow;
    SolarPanelState *solarPanelState;
    volatile unsigned int *batteryLevelPtr;
    volatile unsigned int *batteryTempPtr;
    volatile float *distanceBufferPtr;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    uint16_t *thrusterCommand;
    char *vehicleCommand;
    char *vehicleResponse;
    unsigned int *imageData;
} SatelliteComsData;


extern TCB satelliteComsTCB;

void satelliteComsInit();

void satelliteComs(void* satelliteComsData);

#endif  /* _SATELLITE_COMS_H_ */