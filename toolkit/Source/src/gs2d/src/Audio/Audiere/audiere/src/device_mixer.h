#ifndef MIXER_H
#define MIXER_H


#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <list>
#include "audiere.h"
#include "device.h"
#include "resampler.h"
#include "threads.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class MixerStream;


  /// Always produce 16-bit, stereo audio at the specified rate.
  class MixerDevice : public AbstractDevice, public Mutex {
  public:
    MixerDevice(int rate);

    // update() must be implementated by the specific device to call read()
    // and write the samples to the output device.

    OutputStream* ADR_CALL openStream(SampleSource* source);

    OutputStream* ADR_CALL openBuffer(
      void* samples,
      int frame_count,
      int channel_count,
      int sample_rate,
      SampleFormat sample_format);

  protected:
    int read(int sample_count, void* samples);

  private:
    std::list<MixerStream*> m_streams;
    int m_rate;

    friend class MixerStream;
  };


  class MixerStream : public RefImplementation<OutputStream> {
  public:
    MixerStream(MixerDevice* device, SampleSource* source, int rate);
    ~MixerStream();

    void  ADR_CALL play();
    void  ADR_CALL stop();
    bool  ADR_CALL isPlaying();
    void  ADR_CALL reset();

    void  ADR_CALL setRepeat(bool repeat);
    bool  ADR_CALL getRepeat();
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
    void read(int frame_count, s16* buffer);

  private:
    RefPtr<MixerDevice> m_device;

    RefPtr<Resampler> m_source;
    s16 m_last_l;
    s16 m_last_r;
    bool m_is_playing;
    int m_volume;
    int m_pan;

    friend class MixerDevice;
  };

}

#endif
