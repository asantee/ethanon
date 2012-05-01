#ifndef DEVICE_DS_BUFFER_H
#define DEVICE_DS_BUFFER_H


#include <windows.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include <dsound.h>
#include "audiere.h"


namespace audiere {

  class DSAudioDevice;

  class DSOutputBuffer : public RefImplementation<OutputStream> {
  public:
    DSOutputBuffer(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int length,
      int frame_size);
    ~DSOutputBuffer();

    void ADR_CALL play();
    void ADR_CALL stop();
    bool ADR_CALL isPlaying();
    void ADR_CALL reset();

    void ADR_CALL setRepeat(bool repeat);
    bool ADR_CALL getRepeat();

    void  ADR_CALL setVolume(float volume);
    float ADR_CALL getVolume();

    void  ADR_CALL setPan(float pan);
    float ADR_CALL getPan();

    void  ADR_CALL setPitchShift(float shift);
    float ADR_CALL getPitchShift();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    void update(); ///< Solely for processing events.

    RefPtr<DSAudioDevice> m_device;
    IDirectSoundBuffer* m_buffer;
    int m_length;
    int m_frame_size;

    int m_base_frequency;

    bool  m_repeating;
    float m_volume;
    float m_pan;

    HANDLE m_stop_event;

    friend class DSAudioDevice;
  };

}


#endif
