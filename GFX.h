#ifndef _ADAFRUIT_COMPOSITEVIDEO_H_
#define _ADAFRUIT_COMPOSITEVIDEO_H_

#include <Adafruit_GFX.h>

class GFX : public Adafruit_GFX {
public:
  GFX(int16_t width, int16_t height);
  void setup();
  void begin(int clear);
  void end();

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  char*** getFrame();

protected:
  char** _frame;
  char** _backbuffer;
};


#endif // _ADAFRUIT_COMPOSITEVIDEO_H_