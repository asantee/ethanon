#include "resampler.h"


namespace audiere {

  Resampler::Resampler(SampleSource* source, int rate) {
    m_source = source;
    m_rate = rate;
    m_source->getFormat(
      m_native_channel_count,
      m_native_sample_rate,
      m_native_sample_format);

    m_shift = 1;

    fillBuffers();
    resetState();
  }

  void
  Resampler::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = 2;
    sample_rate = m_rate;
    sample_format = SF_S16;
  }

  int
  Resampler::read(const int frame_count, void* buffer) {
    s16* out = (s16*)buffer;
    int left = frame_count;
    sample_t tmp_l[BUFFER_SIZE];
    sample_t tmp_r[BUFFER_SIZE];
    float delta = m_shift * m_native_sample_rate / m_rate;
    if (m_shift == 0) {  // If shift is zero, which shouldn't be the case, use a shift of 1.
      delta = float(m_native_sample_rate / m_rate);
    }
    while (left > 0) {
      int transfer = std::min(left, int(BUFFER_SIZE));
      memset(tmp_l, 0, transfer * sizeof(sample_t));
      int rv = dumb_resample(&m_resampler_l, tmp_l, transfer, 1.0, delta);
      if (rv == 0) {
        fillBuffers();
        if (m_buffer_length == 0) {
          return frame_count - left;
        } else {
          m_resampler_l.pos = m_resampler_l.subpos = m_resampler_l.start = 0;
          m_resampler_l.end = m_buffer_length;
          m_resampler_l.dir = 1;
          m_resampler_r.pos = m_resampler_r.subpos = m_resampler_r.start = 0;
          m_resampler_r.end = m_buffer_length;
          m_resampler_r.dir = 1;
          continue;
        }
      }
      if (m_native_channel_count == 2) {
        memset(tmp_r, 0, transfer * sizeof(sample_t));
        int rv2 = dumb_resample(&m_resampler_r, tmp_r, transfer, 1.0,
                                delta);
        ADR_ASSERT(rv == rv2, "resamplers returned different sample counts");
        for (int i = 0; i < rv; i++) {
          *out++ = clamp(-32768, tmp_l[i], 32767);
          *out++ = clamp(-32768, tmp_r[i], 32767);
        }
      } else {
        for (int i = 0; i < rv; i++) {
          s16 sample = clamp(-32768, tmp_l[i], 32767);
          *out++ = sample;
          *out++ = sample;
        }
      }
      left -= rv;
    }
    return frame_count;
  }

  void
  Resampler::reset() {
    m_source->reset();
    fillBuffers();
    resetState();
  }


  inline s16 u8tos16(u8 u) {
    return (s16(u) - 128) * 256;
  }

  void
  Resampler::fillBuffers() {
    // we only support channels in [1, 2] and bits in [8, 16] now
    u8 initial_buffer[BUFFER_SIZE * 4];
    unsigned read = m_source->read(BUFFER_SIZE, initial_buffer);

    sample_t* out_l = m_native_buffer_l;
    sample_t* out_r = m_native_buffer_r;

    if (m_native_channel_count == 1) {
      if (m_native_sample_format == SF_U8) {

        // channels = 1, bits = 8
        u8* in = initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          s16 sample = u8tos16(*in++);
          *out_l++ = sample;
        }

      } else {

        // channels = 1, bits = 16
        s16* in = (s16*)initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          s16 sample = *in++;
          *out_l++ = sample;
        }

      }
    } else {
      if (m_native_sample_format == SF_U8) {

        // channels = 2, bits = 8
        u8* in = initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          *out_l++ = u8tos16(*in++);
          *out_r++ = u8tos16(*in++);
        }

      } else {

        // channels = 2, bits = 16
        s16* in = (s16*)initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          *out_l++ = *in++;
          *out_r++ = *in++;
        }
      
      }
    }

    m_buffer_length = read;
  }

  void
  Resampler::resetState() {
    dumb_reset_resampler(&m_resampler_l, m_native_buffer_l, 0, 0,
                         m_buffer_length);
    if (m_native_channel_count == 2) {
      dumb_reset_resampler(&m_resampler_r, m_native_buffer_r, 0, 0,
                           m_buffer_length);
    }
  }

  bool
  Resampler::isSeekable() {
    return m_source->isSeekable();
  }

  int
  Resampler::getLength() {
    return m_source->getLength();
  }

  void
  Resampler::setPosition(int position) {
    m_source->setPosition(position);
    fillBuffers();
    resetState();
  }

  int
  Resampler::getPosition() {
    int position = m_source->getPosition() - m_buffer_length +
                   m_resampler_l.pos;
    while (position < 0) {
      position += m_source->getLength();
    }
    return position;
  }

  bool
  Resampler::getRepeat() {
    return m_source->getRepeat();
  }

  void
  Resampler::setRepeat(bool repeat) {
    /// @todo if we've already read to the end, do we try to read more?
    m_source->setRepeat(repeat);
  }

  int Resampler::getTagCount()              { return m_source->getTagCount();  }
  const char* Resampler::getTagKey(int i)   { return m_source->getTagKey(i);   }
  const char* Resampler::getTagValue(int i) { return m_source->getTagValue(i); }
  const char* Resampler::getTagType(int i)  { return m_source->getTagType(i);  }

  void
  Resampler::setPitchShift(float shift) {
    m_shift = shift;
  }

  float
  Resampler::getPitchShift() {
    return m_shift;
  }

}
