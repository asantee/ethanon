#ifndef DEVICE_AL_H
#define DEVICE_AL_H


extern "C" {
  #include <dmedia/audio.h>
}

#include "audiere.h"
#include "device_mixer.h"
#include "resampler.h"


namespace audiere {

  class ALAudioDevice : public MixerDevice {
  public:
    static ALAudioDevice* create(const ParameterList& parameters);

  private:
    ALAudioDevice(ALport port, int rate);
    ~ALAudioDevice();

  public:
    void ADR_CALL update();
    const char* ADR_CALL getName();

  private:
    ALport m_port;

    friend class ALOutputStream;
  };

}


#endif
