#include <math.h>
#include "basic_source.h"
#include "internal.h"
#include "types.h"

namespace audiere {

  static const double PI = 3.14159265358979323846;

  class SineWave : public BasicSource {
  public:
    SineWave(double frequency) {
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
        double h = sin(2 * PI * m_frequency / 44100 * elapsed++);
        out[i] = normal_to_s16(h);
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

    s16 normal_to_s16(double d) {
      d = (d + 1) / 2; // convert from [-1, 1] to [0, 1]
      return s16(d * 32767 - 16384);
    }

    double m_frequency;
    long elapsed;
  };


  ADR_EXPORT(SampleSource*) AdrCreateTone(double frequency) {
    return new SineWave(frequency);
  }

}
