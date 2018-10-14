#ifndef _SATELLITE_COMS_H_
#define _SATELLITE_COMS_H_

typedef struct {
    bool *fuelLow;
    bool *batteryLow;
    bool *solarPanelState;
    unsigned short *batteryLevel;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    unsigned int *thrusterCommand;
} SatelliteComsData;

void satelliteComms(void*);