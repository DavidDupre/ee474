#ifndef _TRANSPORT_DISTANCE_H_
#define _TRANSPORT_DISTANCE_H_

#include "schedule.h"

typedef struct {
    volatile float *distanceBufferPtr;
} TransportDistanceData;

extern TCB transportDistanceTCB;

void transportDistanceInit();

void transportDistance(void *transportDistanceData);

#endif  /* _TRANSPORT_DISTANCE_H_ */