#pragma once

#include <stdint.h>
#include "schedule.h"
#include <Arduino.h>

#define TLM_PACKET typedef struct __attribute__((__packed__))

#define BUS_SATELLITE  (&Serial)


typedef HardwareSerial serial_bus;

typedef struct {
} ComsTxData;


extern TCB comsTxTCB;


void comsTxInit();

void comsTx(void *comsTxData);

/**
 * inputs:
 *  bus:    The serial bus to send the packet on
 *  tlmId:  The unique telemetry ID
 *  length: The length of the telemetry, not counting the header
 *  data:   The telemtry to send. Must be a static location
 */
void comsTxRegisterSender(serial_bus *bus, uint8_t tlmId, uint8_t length,
        void *data);

/**
 * Indicate that a registered telemetry packet is ready to send.
 *
 * inputs:
 *  tlmId: The unique telemetry ID
 */
void comsTxSend(uint8_t tlmId);