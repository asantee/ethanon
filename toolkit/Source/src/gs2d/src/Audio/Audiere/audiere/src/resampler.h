#ifndef RESAMPLER_H
#define RESAMPLER_H


#include "audiere.h"
#include "debug.h"
#include "dumb_resample.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class Resampler : public RefImplementation<SampleSource> {
  public:
    Resampler(SampleSource* source, int rate);

    // for now, resamplers always return (2, rate, 16-bit)
    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);

    int ADR_CALL read(int frame_count, void* buffer);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

    bool ADR_CALL getRepeat();
    void ADR_CALL setRepeat(bool repeat);

    int ADR_CALL getTagCount();
    const char* ADR_CALL getTagKey(int i);
    const char* ADR_CALL getTagValue(int i);
    const char* ADR_CALL getTagType(int i);

    void  setPitchShift(float shift);
    float getPitchShift();

  private:
    void fillBuffers();
    void resetState();

  private:
    RefPtr<SampleSource> m_source;
    int m_rate;
    int m_native_channel_count;
    int m_native_sample_rate;
    SampleFormat m_native_sample_format;

    enum { BUFFER_SIZE = 4096 };
    sample_t m_native_buffer_l[BUFFER_SIZE];
    sample_t m_native_buffer_r[BUFFER_SIZE];
    DUMB_RESAMPLER m_resampler_l;
    DUMB_RESAMPLER m_resampler_r;
    int m_buffer_length; // number of samples read into each buffer

    float m_shift;
  };

}

#endif
