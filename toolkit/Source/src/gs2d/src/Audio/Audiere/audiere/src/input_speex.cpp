#include "debug.h"
#include "input_speex.h"


namespace audiere {

  class FileReader : public speexfile::Reader {
  private:
    FilePtr m_file;
    bool m_seekable;

  public:
    FileReader(FilePtr file) {
      m_file = file;

      // Hacky test to see whether we can seek in the file.
      m_seekable = m_file->seek(0, File::BEGIN);
    }

    int read(void* ptr, int size) {
      return m_file->read(ptr, size);
    }

    speexfile::offset_t seek(speexfile::offset_t offset) {
      m_file->seek(static_cast<int>(offset), File::BEGIN);
      return get_position();
    }

    speexfile::offset_t get_position() {
      return m_file->tell();
    }

    speexfile::offset_t get_length() {
      speexfile::offset_t c = get_position();
      m_file->seek(0, File::END);
      speexfile::offset_t l = get_position();
      m_file->seek(static_cast<int>(c), File::BEGIN);
      return l;
    }

    bool can_seek() {
      return m_seekable;
    }
  };


  SpeexInputStream::SpeexInputStream() {
    m_speexfile = 0;
    m_position = 0;
  }


  SpeexInputStream::~SpeexInputStream() {
    delete m_speexfile;
  }


  /// @todo  this really should be replaced with a factory function
  bool
  SpeexInputStream::initialize(FilePtr file) {
#if defined(_MSC_VER) && (_MSC_VER <= 1200)
    m_reader = std::auto_ptr<speexfile::Reader>(new FileReader(file));
#else
    m_reader.reset(new FileReader(file));
#endif
    m_speexfile = new speexfile::speexfile(m_reader.get());

    // @todo How should we handle files with multiple streams?
    if (m_speexfile->get_streams() != 1) {
      delete m_speexfile;
      m_speexfile = 0;
      return false;
    }

    speexfile::int32_t rate     = m_speexfile->stream_get_samplerate();
    speexfile::int32_t channels = m_speexfile->stream_get_channels();
    if (rate == 0 || channels == 0) {
      delete m_speexfile;
      m_speexfile = 0;
      return false;
    }

    for (int i = 0; i < m_speexfile->stream_get_tagcount(); ++i) {
      const speexfile::speextags* tag = m_speexfile->stream_get_tags()[i];
      addTag(
        tag->item ? tag->item : "",
        tag->value ? tag->value : "",
        "Speex");
    }

    return true;
  }


  void
  SpeexInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_speexfile->stream_get_channels();
    sample_rate   = m_speexfile->stream_get_samplerate();
    sample_format = SF_S16;
  }


  int
  SpeexInputStream::doRead(int frame_count, void* buffer) {
    s16* out = (s16*)buffer;

    int total_read = 0;
    while (frame_count > 0) {
      // If the buffer is empty, decode a little from the speex file.
      if (m_read_buffer.getSize() == 0) {
        float decode_buffer[2000];  // Size defined by speexfile API.
        int speex_read = m_speexfile->decode(decode_buffer);
        if (speex_read == 0) {
          break;
        }

        m_read_buffer.write(decode_buffer, speex_read * sizeof(float));
      }

      const int BUFFER_SIZE = 1024;
      float read_buffer[BUFFER_SIZE];

      int should_read = std::min(frame_count, BUFFER_SIZE);
      int actual_read = m_read_buffer.read(read_buffer, should_read * sizeof(float)) / sizeof(float);
      ADR_ASSERT(actual_read != 0, "Read queue should have data");

      for (int i = 0; i < actual_read; ++i) {
        out[i] = s16(read_buffer[i] * 32767);
      }

      frame_count -= actual_read;
      total_read += actual_read;
      out += actual_read;
    }
    m_position += total_read;
    return total_read;
  }


  void
  SpeexInputStream::reset() {
    setPosition(0);  // need to update m_position
  }


  bool
  SpeexInputStream::isSeekable() {
    return true;
  }


  int
  SpeexInputStream::getLength() {
    return static_cast<int>(m_speexfile->get_samples());
  }


  void
  SpeexInputStream::setPosition(int position) {
    m_speexfile->seek_sample(position);
    m_position = position;
  }


  int
  SpeexInputStream::getPosition() {
    return m_position;
  }

}
