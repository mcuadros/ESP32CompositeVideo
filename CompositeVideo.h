#ifndef CompositeVideo_h
#define CompositeVideo_h

#include <Output.h>
#include <GFX.h>

class CompositeVideo : public GFX {
public:
  CompositeVideo(Mode mode,  int width,  int height);
  void setup();
  void draw();

protected:
  Output* _output = NULL;

  void _createTask();
};


#endif