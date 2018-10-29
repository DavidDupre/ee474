#ifndef _VEHICLE_COMMS_H_
#define _VEHICLE_COMMS_H_

#define VCOMMS_TIMEOUT 500

typedef struct {
    char *vehicleCommand;
    char *vehicleResponse;
} VehicleCommsData;


void vehicleComms(void *vehicleCommsData);

#endif  /* _VEHICLE_COMMS_H_ */