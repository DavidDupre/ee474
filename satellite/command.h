#pragma once

#include <stdint.h>
#include "schedule.h"


typedef enum {
    CMDID_INC_PANEL_SPEED = 0,
    CMDID_DEC_PANEL_SPEED = 1,
    CMDID_THRUSTER =        2,
    CMDID_VEHICLE =         3,
    CMDID_ACK_TEMP =        4
} CmdId;


// returns true if the command was handled
typedef bool (*cmd_callback_fn)(uint8_t*);

typedef struct {
    uint8_t *numErrors;
} CmdData;


extern TCB cmdTCB;


void cmdInit();

void cmdUpdate(void *cmdData);

void cmdRegisterCallback(CmdId cmdId, cmd_callback_fn callback);
