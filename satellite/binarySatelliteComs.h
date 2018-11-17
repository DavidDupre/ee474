#pragma once

#include <stdint.h>
#include "schedule.h"

#define TLM_PACKET typedef struct __attribute__((__packed__))


/*
 * Telemetry IDs used to identify telemetry packets
 *
 * These must be unique across the entire satellite.
 * Range 0 through 255.
 */
typedef enum {
    TLMID_META =        0,
    TLMID_POWER =       1,
    TLMID_SOLAR_PANEL = 2,
    TLMID_THRUSTER =    3,
    TLMID_TIMES =       5,
} TlmId;

// returns true if the command was handled
typedef bool (*cmd_handler_fn)(uint8_t, uint8_t*);

typedef struct {
    uint8_t *numErrors;
} BCData;


extern TCB bcTCB;


void bcInit();

void binarySatelliteComs(void *bcData);

/**
 * inputs:
 *  tlmId:  The unique telemetry ID
 *  length: The length of the telemetry, not counting the header
 *  data:   The telemtry to send. Must be a static location
 */
void bcRegisterTlmSender(TlmId tlmId, uint8_t length, void *data);

/**
 * Indicate that a registered telemetry packet is ready to send.
 *
 * inputs:
 *  tlmId: The unique telemetry ID
 */
void bcSend(TlmId tlmId);

void bcRegisterCmdHandler(TaskId taskId, cmd_handler_fn handler);
