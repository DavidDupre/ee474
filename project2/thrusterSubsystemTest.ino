#include "thrusterSubsystem.h"
#include <stdint.h>

/*
 * Test the macros that parse a thruster command
 */
test(thrusterParse) {
    // duration 2 s, magnitude 4, left and up on
    uint16_t cmd = 0b0000001001000101;
    uint8_t duration = THRUSTER_CMD_DUR(cmd);
    uint8_t magnitude = THRUSTER_CMD_MAG(cmd);
    bool left = THRUSTER_CMD_LEFT(cmd);
    bool right = THRUSTER_CMD_RIGHT(cmd);
    bool up = THRUSTER_CMD_UP(cmd);
    bool down = THRUSTER_CMD_DOWN(cmd);
    assertEqual(duration, 2);
    assertEqual(magnitude, 4);
    assertTrue(left);
    assertFalse(right);
    assertTrue(up);
    assertFalse(down);
}

/*
 * Test createThrusterCommand
 */
test(thrusterCreateCommand) {
    uint16_t cmd = createThrusterCommand(true, false, true, false, 4, 2);
    assertEqual(THRUSTER_CMD_DUR(cmd), 2);
    assertEqual(THRUSTER_CMD_MAG(cmd), 4);
    assertTrue(THRUSTER_CMD_LEFT(cmd));
    assertFalse(THRUSTER_CMD_RIGHT(cmd));
    assertTrue(THRUSTER_CMD_UP(cmd));
    assertFalse(THRUSTER_CMD_DOWN(cmd));
}

/*
 * Test thrusterSubsystem
 */
test(thrusterSubsystem) {
    // build the input data struct
    uint8_t magnitude = 10;
    uint8_t duration = 2;
    unsigned int thrusterCommand
        = createThrusterCommand(true, false, true, false, magnitude, duration);
    unsigned short fuelLevel = 100;
    ThrusterSubsystemData data = {
        &thrusterCommand,
        &fuelLevel
    };

    // lower the full fuel so it drains much faster (1 fuel/second)
    thrusterSubsystemFullFuel = 1000;

    // send the new command
    thrusterSubsystem((void *) &data);

    // wait for the command to finish
    delay(2000);

    // call the function again to update the fuel level
    thrusterSubsystem((void *) &data);

    // check that the fuel level was set to the correct amount
    // 10 * (2 seconds) * (1 fuel/second) = 10 fuel expended
    int expectedFuelLevel = 90;
    assertEqual(expectedFuelLevel, (int) fuelLevel);

    // make sure that the fuel doesn't decrease after the command ends
    delay(1000);
    thrusterSubsystem((void *) &data);
    assertEqual(expectedFuelLevel, (int) fuelLevel);
}
