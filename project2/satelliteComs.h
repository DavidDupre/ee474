#ifndef _SATELLITE_COMS_H_
#define _SATELLITE_COMS_H_

typedef struct {
    bool *fuelLow;
    bool *batteryLow;
    bool *solarPanelState;
    volatile unsigned int *batteryLevelPtr;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    unsigned int *thrusterCommand;
    char *vehicleResponse;
} SatelliteComsData;

void satelliteComs(void* satelliteComsData);

#endif  /* _SATELLITE_COMS_H_ */