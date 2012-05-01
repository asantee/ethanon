#include "basic_source.h"
#include "internal.h"
#include "types.h"

namespace audiere {

  class SquareWave : public BasicSource {
  public:
    SquareWave(double frequency) {
      m_frequency = frequency;
      doReset(); // not supposed to call virtual functions in constructors
    }

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format)
    {
      channel_count = 1;
      sample_rate   = 44100;
      sample_format = SF_S16;
    }

    int doRead(int frame_count, void* buffer) {
      // if frequency is 0 Hz, use silence
      if (m_frequency == 0) {
        memset(buffer, 0, frame_count * 2);
        return frame_count;
      }

      s16* out = (s16*)buffer;
      for (int i = 0; i < frame_count; ++i) {
        int value = (int)(elapsed++ * m_frequency / 44100);
        *out++ = (value % 2 ? -32678 : 32767);
      }
      return frame_count;
    }

    void ADR_CALL reset() {
      doReset();
    }

  private:
    void doReset() {
      elapsed = 0;
    }

    double m_frequency;
    long elapsed;
  };


  ADR_EXPORT(SampleSource*) AdrCreateSquareWave(double frequency) {
    return new SquareWave(frequency);
  }

}
