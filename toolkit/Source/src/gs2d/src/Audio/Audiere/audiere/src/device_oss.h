#ifndef DEVICE_OSS_H
#define DEVICE_OSS_H


#include "audiere.h"
#include "device_mixer.h"


namespace audiere {

  class OSSAudioDevice : public MixerDevice {
  public:
    static OSSAudioDevice* create(const ParameterList& parameters);

  private:
    OSSAudioDevice(int output_device);
    ~OSSAudioDevice();

  public:
    void ADR_CALL update();
    const char* ADR_CALL getName();

  private:
    int m_output_device;
  };

}


#endif
