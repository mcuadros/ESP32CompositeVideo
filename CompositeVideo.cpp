#include <CompositeVideo.h>
#include <soc/rtc.h>

CompositeVideo::CompositeVideo(Mode mode, int width, int height):
  GFX(width, height)
{
  Output foo = Output(mode, width * 2, height * 2, 3.3);
    Serial.printf("%d,%d\n",width, height);

  _output = &foo;
}


void CompositeVideo::draw()
{
  //for( ;; ) _output->sendFrameHalfResolution(getFrame());
}

static void _task(void* composite)
{
  CompositeVideo* c = (CompositeVideo*)composite;
  c->draw();
}

void CompositeVideo::setup()
{
  //highest clockspeed needed
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);

  //_output->setup();
  setup();
getFrame();
  //running composite output pinned to first core
  //xTaskCreatePinnedToCore(_task, "composite", 1024, this, 1, NULL, 0); 
}
