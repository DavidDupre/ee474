#ifndef _THRUSTER_SUBSYSTEM_H_
#define _THRUSTER_SUBSYSTEM_H_


typedef struct {
    unsigned int *thrusterCommand;
    unsigned short *fuelLevel;
} ThrusterSubsystemData;


void thrusterSubsystem(void *thrusterSubsystemData);

#endif  /* _THRUSTER_SUBSYSTEM_H_ */