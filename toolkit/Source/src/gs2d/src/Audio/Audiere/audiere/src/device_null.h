#ifndef DEVICE_NULL_H
#define DEVICE_NULL_H


#include <list>
#include "audiere.h"
#include "device.h"
#include "internal.h"
#include "threads.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class NullOutputStream;

  class NullAudioDevice : public AbstractDevice, public Mutex {
  public:
    static NullAudioDevice* create(const ParameterList& parameters);

  private:
    NullAudioDevice();
    ~NullAudioDevice();

  public:
    void ADR_CALL update();
    OutputStream* ADR_CALL openStream(SampleSource* source);
    OutputStream* ADR_CALL openBuffer(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format);
    const char* ADR_CALL getName();

  private:
    void removeStream(NullOutputStream* stream);

    typedef std::list<NullOutputStream*> StreamList;
    StreamList m_streams;

    friend class NullOutputStream;
  };

  class NullOutputStream : public RefImplementation<OutputStream> {
  private:
    NullOutputStream(NullAudioDevice* device, SampleSource* source);
    ~NullOutputStream();

  public:
    void ADR_CALL play();
    void ADR_CALL stop();
    void ADR_CALL reset();
    bool ADR_CALL isPlaying();

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
    void doStop(bool internal);
    void resetTimer();
    void update();
    int dummyRead(int samples_to_read);

    RefPtr<NullAudioDevice> m_device;
                          
    SampleSourcePtr m_source;
    int m_channel_count;           //
    int m_sample_rate;             // cached stream format
    SampleFormat m_sample_format;  //

    bool m_is_playing;
    float m_volume;
    float m_pan;
    float m_shift;

    u64 m_last_update;

    friend class NullAudioDevice;
  };

}


#endif
