#ifndef Properties_h
#define Properties_h

enum Mode
{
  PAL,
  NTSC
};

typedef struct
{
  float lineMicros;
  float syncMicros;
  float blankEndMicros;
  float backMicros;
  float shortVSyncMicros;
  float overscanLeftMicros;
  float overscanRightMicros;
  float syncVolts;
  float blankVolts;
  float blackVolts;
  float whiteVolts;
  short lines;
  short linesFirstTop;
  short linesOverscanTop;
  short linesOverscanBottom;
  float imageAspect;
} TechProperties;

const TechProperties PALProperties = {
  .lineMicros = 64,
  .syncMicros = 4.7,
  .blankEndMicros = 10.4,
  .backMicros = 1.65,
  .shortVSyncMicros = 2.35,
  .overscanLeftMicros = 1.6875,
  .overscanRightMicros = 1.6875,
  .syncVolts = -0.3,
  .blankVolts = 0.0,
  .blackVolts =  0.005,//specs 0.0,
  .whiteVolts = 0.7,
  .lines = 625,
  .linesFirstTop = 23,
  .linesOverscanTop = 9,
  .linesOverscanBottom = 9,
  .imageAspect = 4./3.
};

const TechProperties NTSCProperties = {
  .lineMicros = 63.492,
  .syncMicros = 4.7,
  .blankEndMicros = 9.2,
  .backMicros = 1.5,
  .shortVSyncMicros = 2.3,
  .overscanLeftMicros = 0,//1.3,
  .overscanRightMicros = 0,//1,
  .syncVolts = -0.286,
  .blankVolts = 0.0,
  .blackVolts = 0.05, //specs 0.054,
  .whiteVolts = 0.714,
  .lines = 525,
  .linesFirstTop = 20,
  .linesOverscanTop = 6,
  .linesOverscanBottom = 9,
  .imageAspect = 4./3.
};

#endif