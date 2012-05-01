#include <algorithm>
#include <stdlib.h>
#include "basic_source.h"
#include "internal.h"
#include "types.h"

namespace audiere {

  class WhiteNoise : public BasicSource {
  public:
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
      s16* out = (s16*)buffer;
      for (int i = 0; i < frame_count; ++i) {
        *out++ = (rand() % 65536 - 32768);
      }
      return frame_count;
    }

    void ADR_CALL reset() {
    }
  };

  ADR_EXPORT(SampleSource*) AdrCreateWhiteNoise() {
    return new WhiteNoise();
  }


  // pink noise generation thanks to PortAudio (http://portaudio.com)
  static const int MAX_RANDOM_ROWS = 30;
  static const int RANDOM_BITS = 24;
  static const int RANDOM_SHIFT = sizeof(long) * 8 - RANDOM_BITS;


  class PinkNoise : public BasicSource {
  public:
    PinkNoise() {
      doReset();
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
      s16* out = (s16*)buffer;
      for (int i = 0; i < frame_count; ++i) {
        *out++ = s16(generate() * 32767 - 16384);
      }
      return frame_count;
    }

    void ADR_CALL reset() {
      doReset();
    }

  private:
    void doReset() {
      static const int numRows = 12;

      m_seed = 22222;

      m_index = 0;
      m_index_mask = (1 << numRows) - 1;

      // Calculate maximum possible signed random value.
      // Extra 1 for white noise always added.
      long pmax = (numRows + 1) * (1 << (RANDOM_BITS - 1));
      m_scalar = 1.0f / pmax;
      
      std::fill(m_rows, m_rows + numRows, 0);
      m_running_sum = 0;
    }

    float generate() {
      // Increment and mask index.
      m_index = (m_index + 1) & m_index_mask;

      // If index is zero, don't update any random values.
      if (m_index) {
        // Determine how many trailing zeros in PinkIndex.
        // This algorithm will hang if n==0 so test first.
        int numZeros = 0;
        int n = m_index;
        while ((n & 1) == 0) {
          n = n >> 1;
          numZeros++;
        }
        // Replace the indexed ROWS random value.
        // Subtract and add back to RunningSum instead of adding all the random
        // values together. Only one changes each time.
        m_running_sum -= m_rows[numZeros];
        long newRandom = generateRandom() >> RANDOM_SHIFT;
        m_running_sum += newRandom;
        m_rows[numZeros] = newRandom;
      }

      // Add extra white noise value.
      long newRandom = generateRandom() >> RANDOM_SHIFT;
      long sum = m_running_sum + newRandom;
      // Scale to range of -1.0 to 0.9999.
      return m_scalar * sum;
    }

    long generateRandom() {
      m_seed = (m_seed * 196314165) + 907633515;
      return m_seed;
    }

    long  m_rows[MAX_RANDOM_ROWS];
    long  m_running_sum;
    int   m_index;
    int   m_index_mask;
    float m_scalar;

    long m_seed;
  };

  ADR_EXPORT(SampleSource*) AdrCreatePinkNoise() {
    return new PinkNoise();
  }

}
