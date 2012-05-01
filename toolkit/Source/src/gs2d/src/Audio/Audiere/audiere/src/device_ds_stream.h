#ifndef DEVICE_DS_STREAM_H
#define DEVICE_DS_STREAM_H


#include <windows.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include <dsound.h>
#include "audiere.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

  class DSAudioDevice;

  class DSOutputStream
    : public RefImplementation<OutputStream>
    , public Mutex
  {
  public:
    DSOutputStream(
      DSAudioDevice* device,
      IDirectSoundBuffer* buffer,
      int buffer_length, // in frames
      SampleSource* source);
    ~DSOutputStream();

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
    void doStop(bool internal);   ///< differentiates between internal and external calls
    void doReset();  ///< thread-unsafe version of reset()
    void fillStream();
    void update();
    int streamRead(int samples_to_read, void* buffer);
    void fillSilence(int sample_count, void* buffer);
  
  private:
    RefPtr<DSAudioDevice> m_device;

    IDirectSoundBuffer* m_buffer;
    int m_buffer_length;  // in samples
    int m_next_read;  // offset (in frames) where we will read next
    int m_last_play;  // offset (in frames) where the play cursor was
    int m_base_frequency;  // in Hz

    bool m_is_playing;

    SampleSourcePtr m_source;
    int m_frame_size;  // convenience: bytes per sample * channel count

    int m_total_read;    // total number of frames read from the stream
    int m_total_played;  // total number of frames played 

    float m_volume;
    float m_pan;
    ::BYTE* m_last_frame; // the last frame read (used for clickless silence)

    friend class DSAudioDevice;
  };

}


#endif
