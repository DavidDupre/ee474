#pragma once

#include <stdint.h>

#define TLM_PACKET typedef struct __attribute__((__packed__))


void comsTxSend(uint8_t tlmId, void *data, uint8_t size);
