#ifndef _VEHICLE_COMMS_H_
#define _VEHICLE_COMMS_H_

#define VCOMMS_TIMEOUT 500

typedef struct {
    char *command;
    char *response;
} VehicleCommsData;


void vehicleComms(void *vehicleCommsData);

#endif  /* _VEHICLE_COMMS_H_ */