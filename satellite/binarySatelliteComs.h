#pragma once

#include <stdint.h>
#include "schedule.h"

#define TLM_PACKET typedef struct __attribute__((__packed__))


typedef struct { 
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

