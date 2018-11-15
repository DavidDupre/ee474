#pragma once

#include <stdint.h>
#include "schedule.h"

// telemetry processing
#define TLMID_META        0
#define TLMID_POWER       1
#define TLMID_SOLAR_PANEL 2
#define TLMID_THRUSTER    4

// command dispatch
#define ENTITYID_SOLAR_PANEL 2
#define ENTITYID_THRUSTER    4

#define TLM_PACKET typedef struct __attribute__((__packed__))


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
void bcRegisterTlmSender(uint8_t tlmId, uint8_t length, void *data);

/**
 * Indicate that a registered telemetry packet is ready to send.
 *
 * inputs:
 *  tlmId: The unique telemetry ID
 */
void bcSend(uint8_t tlmId);

void bcRegisterCmdHandler(uint8_t entityId, cmd_handler_fn handler);
