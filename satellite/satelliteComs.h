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
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    unsigned int *thrusterCommand;
    char *vehicleResponse;
} SatelliteComsData;

typedef void (*cmd_handler_fn)(uint8_t, uint8_t*, SatelliteComsData*);


extern TCB satelliteComsTCB;

void satelliteComsInit();

void satelliteComs(void* satelliteComsData);

void satelliteComsRegisterCmdHandler(uint8_t entityId, cmd_handler_fn handler);

#endif  /* _SATELLITE_COMS_H_ */