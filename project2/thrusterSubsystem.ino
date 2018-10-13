#include "thrusterSubsystem.h"
#include <stdint.h>

/*
 * The number of milliseconds it would take a single thruster operating at
 * minimum throttle (a magnitude of 1) to consume one unit of fuel.
 *
 * (6 months / (5% throttle * 100 fuel units)) * (2.628e6 seconds / month)
 *      * (100% throttle / (2^4 - 1) LSB) = 210240 s/fuel unit/LSB
 */
#define MSEC_PER_FUEL_UNIT_PER_MAG_LSB 210240000

// macros to extract data from the thruster command
#define THRUSTER_CMD_LEFT(cmd)  ((bool) (cmd & (1 << 0)))
#define THRUSTER_CMD_RIGHT(cmd) ((bool) ((cmd & (1 << 1)) >> 1))
#define THRUSTER_CMD_UP(cmd)    ((bool) ((cmd & (1 << 2)) >> 2))
#define THRUSTER_CMD_DOWN(cmd)  ((bool) ((cmd & (1 << 3)) >> 3))
#define THRUSTER_CMD_MAG(cmd)   ((uint8_t) ((cmd & 0xF0) >> 4))
#define THRUSTER_CMD_DUR(cmd)   ((uint8_t) ((cmd & 0xFF00) >> 8))


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
void thrusterSubsystem(void *thrusterSubsystemData) {
    /*
     * partialFuel represents the number of milliseconds it would take one
     * thruster at minimum throttle to reduce the fuelLevel by 1.
     */
    static uint32_t partialFuel = MSEC_PER_FUEL_UNIT_PER_MAG_LSB;

    // command is the last command entered to this function
    static uint16_t command = 0;

    // lastUpdateEpochMs is the system time of the last thrusterSubsystem call
    static unsigned long lastUpdateEpochMs = globalTimeBase();

    // cast the input to the correct ThrusterSubsystemData type
    ThrusterSubsystemData *data
        = (ThrusterSubsystemData *) thrusterSubsystemData;

    // calculate how long the last command has been running
    unsigned long currentEpochMs = globalTimeBase();
    unsigned long timePassedMs = currentEpochMs - lastUpdateEpochMs;
    lastUpdateEpochMs = currentEpochMs;

    // calculate the fuel expended during the duration
    uint8_t numThrustersInUse = THRUSTER_CMD_LEFT(command)
        + THRUSTER_CMD_RIGHT(command)
        + THRUSTER_CMD_UP(command)
        + THRUSTER_CMD_DOWN(command);
    uint32_t partialFuelExpended = timePassedMs * numThrustersInUse
        * THRUSTER_CMD_MAG(command);

    // convert the partial units into full sized fuel units and update the
    // partial fuel level accordingly
    unsigned short fuelExpended = 0;
    if (partialFuelExpended >= partialFuel) {
        partialFuelExpended -= partialFuel;
        fuelExpended++;
        partialFuel = MSEC_PER_FUEL_UNIT_PER_MAG_LSB;
    }
    fuelExpended += partialFuelExpended / MSEC_PER_FUEL_UNIT_PER_MAG_LSB;
    partialFuel -= partialFuelExpended % MSEC_PER_FUEL_UNIT_PER_MAG_LSB;

    // update the output fuel level
    if (fuelExpended >= *data->fuelLevel) {
        *data->fuelLevel = 0;
    } else {
        *data->fuelLevel -= fuelExpended;
    }

    // update the local thruster command
    if (*data->thrusterCommand != THRUSTER_CMD_NONE) {
        // Use the incoming command if it's not none.
        // Set the command to NONE after accepting it, so that commands are not
        // double-accepted.
        command = *data->thrusterCommand;
        *data->thrusterCommand = THRUSTER_CMD_NONE;
    } else if (timePassedMs >= 1000 * THRUSTER_CMD_DUR(command)) {
        // if the current command is past its duration, clear the command
        command = 0;
    }

    return;
}

uint16_t createThrusterCommand(bool useLeft, bool useRight, bool useUp,
                               bool useDown, uint8_t magnitude,
                               uint8_t duration) {
    uint16_t cmd = 0;
    if (useLeft) {
        cmd |= 1;
    }
    if (useRight) {
        cmd |= 1 << 1;
    }
    if (useUp) {
        cmd |= 1 << 2;
    }
    if (useDown) {
        cmd |= 1 << 3;
    }
    cmd |= (magnitude << 4);
    cmd |= ((uint16_t) duration) << 8;
    return cmd;
}
