#ifndef _TRANSPORT_DISTANCE_H_
#define _TRANSPORT_DISTANCE_H_

#define LONGEST_PERIOD 250000
#define SENSOR_PIN 45

#define MIN_FREQ 0
#define MAX_FREQ 10000
#define MIN_DISTANCE 100
#define MAX_DISTANCE 2000

#include "schedule.h"

typedef struct {
    volatile float *distanceBufferPtr;
} TransportDistanceData;

extern TCB transportDistanceTCB;

void transportDistanceInit();

void transportDistance(void *transportDistanceData);

#endif  /* _TRANSPORT_DISTANCE_H_ */