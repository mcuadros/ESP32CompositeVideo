#include "Output.h"

Output::Output(Mode mode, int xres, int yres, double Vcc)
{
  _configure((mode==NTSC) ? NTSCProperties: PALProperties, xres, yres, Vcc);
}

void Output::_configure(const TechProperties props, int xres, int yres, double Vcc)
{
  int linesSyncTop = 5;
  int linesSyncBottom = 3;

  _linesOdd = props.lines / 2;
  _linesEven = props.lines - _linesOdd;
  _linesEvenActive = _linesEven - props.linesFirstTop - linesSyncBottom;
  _linesOddActive = _linesOdd - props.linesFirstTop - linesSyncBottom;
  _linesEvenVisible = _linesEvenActive - props.linesOverscanTop - props.linesOverscanBottom; 
  _linesOddVisible = _linesOddActive - props.linesOverscanTop - props.linesOverscanBottom;

  _targetYresOdd = (yres / 2 < _linesOddVisible) ? yres / 2 : _linesOddVisible;
  _targetYresEven = (yres - _targetYresOdd < _linesEvenVisible) ? yres - _targetYresOdd : _linesEvenVisible;
  _targetYres = _targetYresEven + _targetYresOdd;

  _linesEvenBlankTop = props.linesFirstTop - linesSyncTop + props.linesOverscanTop + (_linesEvenVisible - _targetYresEven) / 2;
  _linesEvenBlankBottom = _linesEven - _linesEvenBlankTop - _targetYresEven - linesSyncBottom;
  _linesOddBlankTop = _linesEvenBlankTop;
  _linesOddBlankBottom = _linesOdd - _linesOddBlankTop - _targetYresOdd - linesSyncBottom;

  double _samplesPerSecond = 160000000.0 / 3.0 / 2.0 / 2.0;
  double _samplesPerMicro = _samplesPerSecond * 0.000001;
  _samplesLine = (int)(_samplesPerMicro * props.lineMicros + 1.5) & ~1;
  _samplesSync = _samplesPerMicro * props.syncMicros + 0.5;
  _samplesBlank = _samplesPerMicro * (props.blankEndMicros - props.syncMicros + props.overscanLeftMicros) + 0.5;
  _samplesBack = _samplesPerMicro * (props.backMicros + props.overscanRightMicros) + 0.5;
  _samplesActive = _samplesLine - _samplesSync - _samplesBlank - _samplesBack;

  _targetXres = xres < _samplesActive ? xres : _samplesActive;

  _samplesVSyncShort = _samplesPerMicro * props.shortVSyncMicros + 0.5;
  _samplesBlackLeft = (_samplesActive - _targetXres) / 2;
  _samplesBlackRight = _samplesActive - _targetXres - _samplesBlackLeft;
  double dacPerVolt = 255.0 / Vcc;
  _levelSync = 0;
  _levelBlank = (props.blankVolts - props.syncVolts) * dacPerVolt + 0.5;
  _levelBlack = (props.blackVolts - props.syncVolts) * dacPerVolt + 0.5;
  _levelWhite = (props.whiteVolts - props.syncVolts) * dacPerVolt + 0.5;
  _grayValues = _levelWhite - _levelBlack + 1;

  _pixelAspect = (float(_samplesActive) / (_linesEvenVisible + _linesOddVisible)) / props.imageAspect;
}

void Output::setup()
{
  _line = (unsigned short*)malloc(sizeof(unsigned short) * _samplesLine);
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 1000000,  //not really used
    .bits_per_sample = (i2s_bits_per_sample_t)I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = _samplesLine  //a buffer per line
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL); //start i2s driver
  i2s_set_pin(I2S_PORT, NULL); //use internal DAC
  i2s_set_sample_rates(I2S_PORT, 1000000); //dummy sample rate, since the function fails at high values

  //this is the hack that enables the highest sampling rate possible ~13MHz, have fun
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_A_V, 1, I2S_CLKM_DIV_A_S);
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_B_V, 1, I2S_CLKM_DIV_B_S);
  SET_PERI_REG_BITS(I2S_CLKM_CONF_REG(0), I2S_CLKM_DIV_NUM_V, 2, I2S_CLKM_DIV_NUM_S);
  SET_PERI_REG_BITS(I2S_SAMPLE_RATE_CONF_REG(0), I2S_TX_BCK_DIV_NUM_V, 2, I2S_TX_BCK_DIV_NUM_S);
}

void Output::_sendLine()
{
  i2s_write_bytes(I2S_PORT, (char*)_line, _samplesLine * sizeof(unsigned short), portMAX_DELAY);
}

void Output::_fillValues(int &i, unsigned char value, int count)
{
  for(int j = 0; j < count; j++) {
    _line[i++^1] = value << 8;
  }
}

void Output::_fillLine(char *pixels)
{
  int i = 0;
  _fillValues(i, _levelSync, _samplesSync);
  _fillValues(i, _levelBlank, _samplesBlank);
  _fillValues(i, _levelBlack, _samplesBlackLeft);

  for(int x = 0; x < _targetXres / 2; x++) {
    short pix = (_levelBlack + pixels[x]) << 8;
    _line[i++^1] = pix;
    _line[i++^1]   = pix;
  }

  _fillValues(i, _levelBlack, _samplesBlackRight);
  _fillValues(i, _levelBlank, _samplesBack);
}

void Output::_fillLong(int &i)
{
  _fillValues(i, _levelSync, _samplesLine / 2 - _samplesVSyncShort);
  _fillValues(i, _levelBlank, _samplesVSyncShort);
}

void Output::_fillShort(int &i)
{
  _fillValues(i, _levelSync, _samplesVSyncShort);
  _fillValues(i, _levelBlank, _samplesLine / 2 - _samplesVSyncShort);
}

void Output::_fillBlank()
{
  int i = 0;
  _fillValues(i, _levelSync, _samplesSync);
  _fillValues(i, _levelBlank, _samplesBlank);
  _fillValues(i, _levelBlack, _samplesActive);
  _fillValues(i, _levelBlank, _samplesBack);
}

void Output::_fillHalfBlank(int &i)
{
  _fillValues(i, _levelSync, _samplesSync);
  _fillValues(i, _levelBlank, _samplesLine / 2 - _samplesSync);
}

void Output::sendFrameHalfResolution(char ***frame)
{
  //Even Halfframe
  int i = 0;
  _fillLong(i); _fillLong(i);
  _sendLine(); _sendLine();
  i = 0;
  _fillLong(i); _fillShort(i);
  _sendLine();
  i = 0;
  _fillShort(i); _fillShort(i);
  _sendLine(); _sendLine();
  _fillBlank();

  for(int y = 0; y < _linesEvenBlankTop; y++) _sendLine();
  for(int y = 0; y < _targetYresEven; y++) {
    char *pixels = (*frame)[y];
    _fillLine(pixels);
    _sendLine();
  }

  _fillBlank();
  for(int y = 0; y < _linesEvenBlankBottom; y++) _sendLine();
  i = 0;
  _fillShort(i); _fillShort(i);
  _sendLine(); _sendLine();

  i = 0;
  _fillShort(i);

  //odd half frame
  _fillLong(i);
  _sendLine();
  i = 0;
  _fillLong(i); _fillLong(i);
  _sendLine(); _sendLine();
  i = 0;
  _fillShort(i); _fillShort(i);
  _sendLine(); _sendLine();
  _fillShort(i); _fillValues(i, _levelBlank, _samplesLine / 2);
  _sendLine();

  _fillBlank();
  for(int y = 0; y < _linesOddBlankTop; y++) _sendLine();
  for(int y = 0; y < _targetYresOdd; y++) {
    char *pixels = (*frame)[y];
    _fillLine(pixels);
    _sendLine();
  }

  _fillBlank();
  for(int y = 0; y < _linesOddBlankBottom; y++) _sendLine();
  i = 0;
  _fillHalfBlank(i); _fillShort(i);
  _sendLine();
  i = 0;
  _fillShort(i); _fillShort(i);
  _sendLine(); _sendLine();
}