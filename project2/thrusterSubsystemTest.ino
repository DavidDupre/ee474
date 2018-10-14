#include "thrusterSubsystem.h"
#include <stdint.h>

/*
 * Test the macros that parse a thruster command
 */
test(thrustersParse) {
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
