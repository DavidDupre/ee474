#pragma once

#include <stdint.h>
#include <Arduino.h>

#define TLM_PACKET typedef struct __attribute__((__packed__))

#define BUS_SATELLITE  (&Serial)


typedef HardwareSerial serial_bus;


void sendTelemetryPacket(serial_bus *bus, uint8_t tlmId, uint8_t *data, uint8_t size);
