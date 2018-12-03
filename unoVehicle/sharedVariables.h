#ifndef _SHARED_VARIABLES_H_
#define _SHARED_VARIABLES_H_

#include <stdint.h>
#include "imageCapture.h"

/*
 * These variables must have global scope.
 *
 * They should normally be accessed through data in Task Control Blocks.
 * However, some must also be accessed in Interrupt Service Routines which
 * are not given TCBs.
 */

extern uint8_t numCmdErrors;

extern unsigned short imageDataRaw[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
extern unsigned int imageData[IMAGE_CAPTURE_FREQ_BUFFER_LENGTH];

#endif /* _SHARED_VARIABLES_H_ */