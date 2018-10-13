#ifndef _THRUSTER_SUBSYSTEM_H_
#define _THRUSTER_SUBSYSTEM_H_

#include <stdint.h>

/*
 * This is a special value of thrusterCommand that represents the absence
 * of a new command.
 *
 * This value was chosen because it would be unreasonable to use all thrusters
 * at the same time.
 */
#define THRUSTER_CMD_NONE 0xFFFF


typedef struct {
    unsigned int *thrusterCommand;
    unsigned short *fuelLevel;
} ThrusterSubsystemData;


void thrusterSubsystem(void *thrusterSubsystemData);

/*
 * Create a thruster command.
 *
 * inputs:
 *  useLeft: true iff the left thruster should be used
 *  useRight: true iff the right thruster should be used
 *  useUp: true iff the up thruster should be used
 *  useDown: true iff the up thruster should be used
 *  magnitude: the magnitude of the thrust from 0 to 15
 *  duration: the duration of the command in seconds
 *
 * outputs: the formatted thrust command (as in thrusterSubsystem.ino)
 */
uint16_t createThrusterCommand(bool useLeft, bool useRight, bool useUp,
                               bool useDown, uint8_t magnitude,
                               uint8_t duration);

#endif  /* _THRUSTER_SUBSYSTEM_H_ */