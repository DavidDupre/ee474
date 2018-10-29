#ifndef _TFT_H_
#define _TFT_H_

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFFA5

#define TFT_IDENTIFIER 0x9341


extern Elegoo_TFTLCD tft;
extern TouchScreen ts;


#endif  /* _TFT_H_ */
