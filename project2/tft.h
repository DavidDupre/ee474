#ifndef _TFT_H_
#define _TFT_H_

#include <TouchScreen.h>
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFFA5

// pins for the shield
#define YP A3
#define XM A2
#define YM 9
#define XP 8


extern TouchScreen touchScreen;
extern Elegoo_TFTLCD tft;


#endif  /* _TFT_H_ */
