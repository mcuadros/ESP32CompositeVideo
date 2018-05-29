#include <GFXCompositeVideo.h>

GFXCompositeVideo::GFXCompositeVideo(int16_t width, int16_t height):
    Adafruit_GFX(width, height) { }

void GFXCompositeVideo::setup()
{
  _frame = (char**)malloc(_height * sizeof(char*));
  _backbuffer = (char**)malloc(_height * sizeof(char*));

  for(int y = 0; y < _height; y++) {
    _frame[y] = (char*)malloc(_width);
    _backbuffer[y] = (char*)malloc(_width);
  }
}

void GFXCompositeVideo::begin(int clear = -1)
{
  if(clear == -1) return;

  for(int y = 0; y < _height; y++) {
    for(int x = 0; x < _width; x++) {
      _backbuffer[y][x] = clear;
    }
  }
}

void GFXCompositeVideo::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  int16_t t;
  switch(rotation) {
    case 1:
      t = x;
      x = WIDTH  - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH  - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
  }

  _backbuffer[y][x] = color;
}

char*** GFXCompositeVideo::getFrame()
{
  return &_frame;
}

void GFXCompositeVideo::end()
{
  char **b = _backbuffer;
  _backbuffer = _frame;
  _frame = b;
}