#ifndef _TRANSPORT_DISTANCE_H_
#define _TRANSPORT_DISTANCE_H_

#define LONGEST_PERIOD 550000
#define SENSOR_PIN 45

#define MIN_FREQ 0
#define MAX_FREQ 10000
#define MIN_DISTANCE 100
#define MAX_DISTANCE 2000

typedef struct {
    volatile unsigned int *distanceBufferPtr;
} TransportDistanceData;


void transportDistance(void *transportDistanceData);

#endif  /* _TRANSPORT_DISTANCE_H_ */