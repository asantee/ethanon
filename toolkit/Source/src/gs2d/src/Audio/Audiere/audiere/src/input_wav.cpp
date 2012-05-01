#include <string.h>
#include "debug.h"
#include "input_wav.h"
#include "utility.h"


namespace audiere {

  static inline bool IsValidSampleSize(u32 size) {
    return (size == 8 || size == 16);
  }


  WAVInputStream::WAVInputStream() {
    m_file = 0;

    m_channel_count = 0;
    m_sample_rate   = 0;
    m_sample_format = SF_U8;  // reasonable default?

    m_data_chunk_location = 0;
    m_data_chunk_length   = 0;

    m_frames_left_in_chunk = 0;
  }


  /// @todo  this really should be replaced with a factory function
  bool
  WAVInputStream::initialize(FilePtr file) {
    m_file = file;

    // read the RIFF header
    char riff_id[4];
    u8   riff_length_buffer[4];
    char riff_datatype[4];

    u32 size = 0;
    size += file->read(riff_id, 4);
    size += file->read(riff_length_buffer, 4);
    size += file->read(riff_datatype, 4);

    int riff_length = read32_le(riff_length_buffer);

    if (size != 12 ||
        memcmp(riff_id, "RIFF", 4) != 0 ||
        riff_length == 0 ||
        memcmp(riff_datatype, "WAVE", 4) != 0) {

      ADR_LOG("Couldn't read RIFF header");

      // so we don't destroy the file
      m_file = 0;
      return false;
    }

    if (findFormatChunk() && findDataChunk()) {
      return true;
    } else {
      m_file = 0;
      return false;
    }
  }


  void
  WAVInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }


  int
  WAVInputStream::doRead(int frame_count, void* buffer) {
    if (m_frames_left_in_chunk == 0) {
      return 0;
    }

    const int frames_to_read = std::min(frame_count, m_frames_left_in_chunk);
    const int frame_size = m_channel_count * GetSampleSize(m_sample_format);
    const int bytes_to_read = frames_to_read * frame_size;
  
    const int read = m_file->read(buffer, bytes_to_read);
    const int frames_read = read / frame_size;

#if WORDS_BIGENDIAN
    if (m_sample_format == SF_S16) {
      // make little endian into host endian
      u8* out = (u8*)buffer;
      for (int i = 0; i < frames_read * m_channel_count; ++i) {
        std::swap(out[0], out[1]);
        out += 2;
      }
    }
#endif

    // assume that if we didn't get a full read, we're done
    if (read != bytes_to_read) {
      m_frames_left_in_chunk = 0;
      return frames_read;
    }

    m_frames_left_in_chunk -= frames_read;
    return frames_read;
  }


  void
  WAVInputStream::reset() {
    // seek to the beginning of the data chunk
    m_frames_left_in_chunk = m_data_chunk_length;
    m_file->seek(m_data_chunk_location, File::BEGIN);
  }


  bool
  WAVInputStream::isSeekable() {
    return true;
  }


  int
  WAVInputStream::getLength() {
    return m_data_chunk_length;
  }


  void
  WAVInputStream::setPosition(int position) {
    int frame_size = m_channel_count * GetSampleSize(m_sample_format);
    m_frames_left_in_chunk = m_data_chunk_length - position;
    m_file->seek(m_data_chunk_location + position * frame_size, File::BEGIN);
  }


  int
  WAVInputStream::getPosition() {
    return m_data_chunk_length - m_frames_left_in_chunk;
  }


  bool
  WAVInputStream::findFormatChunk() {
    ADR_GUARD("WAVInputStream::findFormatChunk");

    // seek to just after the RIFF header
    m_file->seek(12, File::BEGIN);

    // search for a format chunk
    for (;;) {
      char chunk_id[4];
      u8   chunk_length_buffer[4];

      int size = m_file->read(chunk_id, 4);
      size    += m_file->read(chunk_length_buffer, 4);
      u32 chunk_length = read32_le(chunk_length_buffer);

      // if we couldn't read enough, we're done
      if (size != 8) {
        return false;
      }

      // if we found a format chunk, excellent!
      if (memcmp(chunk_id, "fmt ", 4) == 0 && chunk_length >= 16) {

        ADR_LOG("Found format chunk");

        // read format chunk
        u8 chunk[16];
        size = m_file->read(chunk, 16);

        // could we read the entire format chunk?
        if (size < 16) {
          return false;
        }

        chunk_length -= size;

        // parse the memory into useful information
        u16 format_tag         = read16_le(chunk + 0);
        u16 channel_count      = read16_le(chunk + 2);
        u32 samples_per_second = read32_le(chunk + 4);
        //u32 bytes_per_second   = read32_le(chunk + 8);
        //u16 block_align        = read16_le(chunk + 12);
        u16 bits_per_sample    = read16_le(chunk + 14);

        // format_tag must be 1 (WAVE_FORMAT_PCM)
        // we only support mono and stereo
        if (format_tag != 1 ||
            channel_count > 2 ||
            !IsValidSampleSize(bits_per_sample)) {
          ADR_LOG("Invalid WAV");
          return false;
        }

        // skip the rest of the chunk
        if (!skipBytes(chunk_length)) {
          // oops, end of stream
          return false;
        }

        // figure out the sample format
        if (bits_per_sample == 8) {
          m_sample_format = SF_U8;
        } else if (bits_per_sample == 16) {
          m_sample_format = SF_S16;
        } else {
          return false;
        }

        // store the other important .wav attributes
        m_channel_count = channel_count;
        m_sample_rate   = samples_per_second;
        return true;

      } else {

        // skip the rest of the chunk
        if (!skipBytes(chunk_length)) {
          // oops, end of stream
          return false;
        }

      }
    }
  }


  bool
  WAVInputStream::findDataChunk() {
    ADR_GUARD("WAVInputStream::findDataChunk");

    // seek to just after the RIFF header
    m_file->seek(12, File::BEGIN);

    // search for a data chunk
    while (true) {
      char chunk_id[4];
      u8   chunk_length_buffer[4];

      int size = m_file->read(chunk_id, 4);
      size    += m_file->read(chunk_length_buffer, 4);
      u32 chunk_length = read32_le(chunk_length_buffer);

      // if we couldn't read enough, we're done
      if (size != 8) {
        ADR_LOG("Couldn't read chunk header");
        return false;
      }

      // if we found a data chunk, excellent!
      if (memcmp(chunk_id, "data", 4) == 0) {

        ADR_LOG("Found data chunk");

        // calculate the frame size so we can truncate the data chunk
        int frame_size = m_channel_count * GetSampleSize(m_sample_format);

        m_data_chunk_location  = m_file->tell();
        m_data_chunk_length    = chunk_length / frame_size;
        m_frames_left_in_chunk = m_data_chunk_length;
        return true;

      } else {

        ADR_IF_DEBUG {
          char str[80];
          sprintf(str, "Skipping: %d bytes in chunk '%c%c%c%c'",
                  (int)chunk_length,
                  chunk_id[0], chunk_id[1], chunk_id[2], chunk_id[3]);
          ADR_LOG(str);
        }

        // skip the rest of the chunk
        if (!skipBytes(chunk_length)) {
          // oops, end of stream
          return false;
        }

      }
    }
  }


  bool
  WAVInputStream::skipBytes(int size) {
    return m_file->seek(size, File::CURRENT);
  }


}
