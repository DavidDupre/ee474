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

// The GPIO pins that the thruster subsystem will output a PWM signal to
#define THRUSTER_PIN_LEFT  41
#define THRUSTER_PIN_RIGHT 42
#define THRUSTER_PIN_UP    43
#define THRUSTER_PIN_DOWN  44

typedef struct {
    unsigned int *thrusterCommand;
    unsigned short *fuelLevel;
} ThrusterSubsystemData;


/******************************************************************************
 * name: thrusterSubsystemInit
 *
 * inputs: void
 *
 * outputs: void
 *
 * description:
 *  Initialize the thruster subsystem. This sets the pin mode for the pins used
 *  by this subsystem
 *
 * author: David Dupre
 *****************************************************************************/
void thrusterSubsystemInit();

/******************************************************************************
 * name: thrusterSubsystem
 *
 * inputs:
 *  thrusterSubsystemData (void*): must be of type ThrusterSubsystemData*. If
 *      thrusterCommand is not THRUSTER_CMD_NONE, this function will update the
 *      fuelLevel using that command. Otherwise, the last command will be used
 *      (or 0 if no command has been input yet).
 *
 * outputs: void
 *
 * description:
 *  thrusterSubsystem manages the satellite's fuel level based on the
 *  current thruster command. The current thrust command is always the last
 *  non-THRUSTER_CMD_NONE command entered to this function, or 0 if no command
 *  has been entered yet.
 *
 *  thrusterCommand is interpreted as follows:
 *
 *  +--------------------+--------------+--------+---------+
 *  |        Data        | Bit(s) first | Length |  Units  |
 *  |                    |   to last    | (bits) |         |
 *  +--------------------+--------------+--------+---------+
 *  | Left thruster on?  | 0            | 1      | -       |
 *  | Right thruster on? | 1            | 1      | -       |
 *  | Up thruster on?    | 2            | 1      | -       |
 *  | Down thruster on?  | 3            | 1      | -       |
 *  | Magnitude          | 4-7          | 4      | -       |
 *  | Duration           | 8-15         | 8      | seconds |
 *  +--------------------+--------------+--------+---------+
 *
 *  If there is not enough fuel left to execute the last command, the fuel will
 *  drop to zero. This function must be called at least once per second to
 *  to guarantee a 1-second duration resolution. Assuming an initial fuel level
 *  of 100, one thruster running at 5% magnitude will consume all fuel after 6
 *  months. In other words, fuel is expended at
 *  4.756e-6 LSB * fuel units / second, where LSB is the least significant bit
 *  of the magnitude (i.e. a magnitude of 1).
 *
 * pseudocode:
 *  magnitude = getMagnitude(currentCommand)
 *  numThrustersInUse = getNumThrusters(currentCommand)
 *  timePassed = globalTimeBase - lastGlobalTimeBase
 *  lastGlobalTimeBase = globalTimeBase
 *
 *  fuelExpended = numThrustersInUse * magnitude * timePassed
 *  if fuelExpended >= fuelLevel then
 *      fuelLevel = 0
 *  else
 *      fuelLevel = fuelLevel - fuelExpended
 *
 *  if inputCommand != THRUSTER_CMD_NONE then
 *      currentCommand = inputCommand
 *  else if timePassed >= getDuration(currentCommand) then
 *      currentCommand = 0
 *
 * author: David Dupre
 *****************************************************************************/
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