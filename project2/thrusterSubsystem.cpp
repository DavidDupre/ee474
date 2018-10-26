#include "thrusterSubsystem.h"
#include "schedule.h"
#include "predefinedMacros.h"
#include <stdint.h>
#include <Arduino.h>

/*
 * The number of milliseconds it would take a single thruster operating at
 * minimum throttle (a magnitude of 1) to consume one unit of fuel.
 *
 * (6 months / (5% throttle * 100 fuel units)) * (2.628e6 seconds / month)
 *      * (100% throttle / (2^4 - 1) LSB) = 210240 s/fuel unit/LSB
 */
#define MSEC_PER_FUEL_UNIT_PER_MAG_LSB 210240000


/*
 * Set the PWM output based on a command.
 */
void setThrusters(uint16_t command);


/*
 * Holds the max value of a `partialFuel`. Should be changed only for testing.
 */
uint32_t thrusterSubsystemFullFuel = MSEC_PER_FUEL_UNIT_PER_MAG_LSB;

void thrusterSubsystemInit() {
    pinMode(THRUSTER_PIN_LEFT, OUTPUT);
    pinMode(THRUSTER_PIN_RIGHT, OUTPUT);
    pinMode(THRUSTER_PIN_UP, OUTPUT);
    pinMode(THRUSTER_PIN_DOWN, OUTPUT);
}

void thrusterSubsystem(void *thrusterSubsystemData) {
    /*
     * partialFuel represents the number of milliseconds it would take one
     * thruster at minimum throttle to reduce the fuelLevel by 1.
     */
    static uint32_t partialFuel = thrusterSubsystemFullFuel;

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
        partialFuel = thrusterSubsystemFullFuel;
    }
    fuelExpended += partialFuelExpended / thrusterSubsystemFullFuel;
    partialFuel -= partialFuelExpended % thrusterSubsystemFullFuel;

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

    // update the thrusters to use the new current command
    setThrusters(command);

    return;
}

void setThrusters(uint16_t command) {
    // PWM ranges from 0 to 255, magnitude ranges from 0 to 15.
    // Convert magnitude to a PWM signal by multiplying by 16.
    uint8_t pwm = THRUSTER_CMD_MAG(command) << 4;

    uint8_t left = THRUSTER_CMD_LEFT(command) ? pwm : 0;
    uint8_t right = THRUSTER_CMD_RIGHT(command) ? pwm : 0;
    uint8_t up = THRUSTER_CMD_UP(command) ? pwm : 0;
    uint8_t down = THRUSTER_CMD_DOWN(command) ? pwm : 0;

    analogWrite(THRUSTER_PIN_LEFT, left);
    analogWrite(THRUSTER_PIN_RIGHT, right);
    analogWrite(THRUSTER_PIN_UP, up);
    analogWrite(THRUSTER_PIN_DOWN, down);
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

void setMaxPartialFuel(uint32_t maxPartialFuel) {
    #ifdef RUN_TASKS
        thrusterSubsystemFullFuel = maxPartialFuel;
    #endif
    return;
}
