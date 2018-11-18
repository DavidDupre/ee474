#ifndef _VEHICLE_COMMS_H_
#define _VEHICLE_COMMS_H_

#include "schedule.h"


typedef struct {
    char *vehicleCommand;
    char *vehicleResponse;
} VehicleCommsData;

void vehicleCommsInit();

extern TCB vehicleCommsTCB;

void vehicleCommsInit();

void vehicleComms(void *vehicleCommsData);

#endif  /* _VEHICLE_COMMS_H_ */