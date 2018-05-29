#ifndef Output_h
#define Output_h
#include <FreeRTOS.h>
#include "driver/i2s.h"
#include "Properties.h"

const i2s_port_t I2S_PORT = (i2s_port_t)I2S_NUM_0;

class Output
{
  public:
  Output(Mode mode, int xres, int yres, double Vcc);
  void setup();
  void sendFrameHalfResolution(char ***frame);

  private:
  int _samplesLine;
  int _samplesSync;
  int _samplesBlank;
  int _samplesBack;
  int _samplesActive;
  int _samplesBlackLeft;
  int _samplesBlackRight;

  int _samplesVSyncShort;
  int _samplesVSyncLong;

  char _levelSync;
  char _levelBlank;
  char _levelBlack;
  char _levelWhite;
  char _grayValues;

  int _targetXres;
  int _targetYres;
  int _targetYresEven;
  int _targetYresOdd;

  int _linesEven;
  int _linesOdd;
  int _linesEvenActive;
  int _linesOddActive;
  int _linesEvenVisible;
  int _linesOddVisible;
  int _linesEvenBlankTop;
  int _linesEvenBlankBottom;
  int _linesOddBlankTop;
  int _linesOddBlankBottom;

  float _pixelAspect;
  unsigned short *_line;

  void _configure(const TechProperties props, int xres, int yres, double Vcc);
  void _sendLine();
  void _fillValues(int &i, unsigned char value, int count);
  void _fillLine(char *pixels);
  void _fillLong(int &i);
  void _fillShort(int &i);
  void _fillBlank();
  void _fillHalfBlank(int &i);
};

#endif