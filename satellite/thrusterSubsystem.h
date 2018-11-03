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

// macros to extract data from the thruster command
#define THRUSTER_CMD_LEFT(cmd)  ((bool) (cmd & (1 << 0)))
#define THRUSTER_CMD_RIGHT(cmd) ((bool) ((cmd & (1 << 1)) >> 1))
#define THRUSTER_CMD_UP(cmd)    ((bool) ((cmd & (1 << 2)) >> 2))
#define THRUSTER_CMD_DOWN(cmd)  ((bool) ((cmd & (1 << 3)) >> 3))
#define THRUSTER_CMD_MAG(cmd)   ((uint8_t) ((cmd & 0xF0) >> 4))
#define THRUSTER_CMD_DUR(cmd)   ((uint8_t) ((cmd & 0xFF00) >> 8))


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

/*
 * Set the max value of partialFuel. Decreasing this will simulate a faster
 * fuel consumption. Do not use this outside of testing.
 */
void setMaxPartialFuel(uint32_t maxPartialFuel);

#endif  /* _THRUSTER_SUBSYSTEM_H_ */