#ifndef DEVICE_MM_H
#define DEVICE_MM_H


#include <windows.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include <queue>
#include "internal.h"
#include "device_mixer.h"
#include "utility.h"


namespace audiere {

  class MMAudioDevice : public MixerDevice
  {
  public:
    static MMAudioDevice* create(const ParameterList& parameters);

  private:
    MMAudioDevice(HWAVEOUT device, int rate);
    ~MMAudioDevice();

  public:
    void ADR_CALL update();
    const char* ADR_CALL getName();

  private:
    // 16 buffers of 1000 frames is 4000 frames at 44100 Hz is about
    // 364 milliseconds of audio
    enum {
      BUFFER_COUNT  = 16,
      BUFFER_LENGTH = 1000 * 4, // 1000 samples, 4000 bytes
    };

    HWAVEOUT m_device;

    int m_current_buffer;
    WAVEHDR m_buffers[BUFFER_COUNT];
    u8 m_samples[BUFFER_COUNT * BUFFER_LENGTH];

    friend class MMOutputStream;
  };

}


#endif
