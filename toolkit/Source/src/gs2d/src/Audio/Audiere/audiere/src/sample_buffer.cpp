#include "audiere.h"
#include "basic_source.h"
#include "internal.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class BufferStream : public BasicSource {
  public:
    BufferStream(SampleBuffer* buffer) {
      m_buffer = buffer;

      // get the sample format so we can calculate the size of each frame
      int channel_count, sample_rate;
      SampleFormat sample_format;
      buffer->getFormat(channel_count, sample_rate, sample_format);

      m_frame_size  = channel_count * GetSampleSize(sample_format);
      m_frame_count = m_buffer->getLength();
      m_samples     = (const u8*)m_buffer->getSamples();

      m_position = 0;
    }


    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format)
    {
      m_buffer->getFormat(channel_count, sample_rate, sample_format);
    }


    int doRead(int frame_count, void* buffer) {
      int to_read = std::min(frame_count, m_frame_count - m_position);
      memcpy(
        buffer,
        m_samples + m_position * m_frame_size,
        to_read * m_frame_size);
      m_position += to_read;
      return to_read;
    }


    void ADR_CALL reset() {
      m_position = 0;
    }


    bool ADR_CALL isSeekable()              { return true;           }
    int ADR_CALL getLength()                { return m_frame_count;  }
    void ADR_CALL setPosition(int position) { m_position = position; }
    int ADR_CALL getPosition()              { return m_position;     }

  private:
    RefPtr<SampleBuffer> m_buffer;
    int m_frame_size;
    int m_frame_count;
    const u8* m_samples;

    int m_position;  // in frames
  };


  class SampleBufferImpl : public RefImplementation<SampleBuffer> {
  public:
    SampleBufferImpl(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format)
    {
      const int frame_size = channel_count * GetSampleSize(sample_format);
      const int buffer_size = frame_count * frame_size;
      m_samples = new u8[buffer_size];
      if (samples) {
        memcpy(m_samples, samples, buffer_size);
      } else {
        memset(m_samples, 0, buffer_size);
      }

      m_frame_count   = frame_count;
      m_channel_count = channel_count;
      m_sample_rate   = sample_rate;
      m_sample_format = sample_format;
    }

    ~SampleBufferImpl() {
      delete[] m_samples;
    }

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format)
    {
      channel_count = m_channel_count;
      sample_rate   = m_sample_rate;
      sample_format = m_sample_format;
    }

    int ADR_CALL getLength() {
      return m_frame_count;
    }

    const void* ADR_CALL getSamples() {
      return m_samples;
    }

    SampleSource* ADR_CALL openStream() {
      return new BufferStream(this);
    }

  private:
    u8* m_samples;
    int m_frame_count;
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;
  };


  ADR_EXPORT(SampleBuffer*) AdrCreateSampleBuffer(
    void* samples,
    int frame_count,
    int channel_count,
    int sample_rate,
    SampleFormat sample_format)
  {
    return new SampleBufferImpl(
      samples, frame_count,
      channel_count, sample_rate, sample_format);
  }

  ADR_EXPORT(SampleBuffer*) AdrCreateSampleBufferFromSource(
    SampleSource* source)
  {
    // if there is no source or it isn't seekable, we can't make a
    // buffer from it
    if (!source || !source->isSeekable()) {
      return 0;
    }

    int length = source->getLength();
    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    int stream_length_bytes = length *
      channel_count * GetSampleSize(sample_format);
    u8* buffer = new u8[stream_length_bytes];

    source->setPosition(0);
    source->read(length, buffer);

    SampleBuffer* sb = CreateSampleBuffer(
      buffer, length, channel_count, sample_rate, sample_format);

    delete[] buffer;
    return sb;
  }

}
