/**
 * This file is roughly based on the WAVInputStream implementation,
 * since AIFF and WAV files are so alike.
 */

#include <string.h>
#include "debug.h"
#include "input_aiff.h"
#include "utility.h"


namespace audiere {

  static inline bool isValidSampleSize(u32 size) {
    return (size == 8 || size == 16);
  }


  AIFFInputStream::AIFFInputStream() {
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
  AIFFInputStream::initialize(FilePtr file) {
    ADR_GUARD("AIFFInputStream::initialize");

    m_file = file;

    u8 header[12];
    if (file->read(header, 12) != 12) {
      ADR_LOG("Failed to read AIFF header");
      m_file = 0;
      return false;
    }

    if (memcmp(header, "FORM", 4) != 0 ||
        read32_be(header + 4) == 0 ||
        memcmp(header + 8, "AIFF", 4) != 0)
    {
      ADR_LOG("Invalid AIFF header");
      m_file = 0;
      return false;
    }

    if (findCommonChunk() && findSoundChunk()) {
      return true;
    } else {
      m_file = 0;
      return false;
    }
  }


  void
  AIFFInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }


  int
  AIFFInputStream::doRead(int frame_count, void* buffer) {
    if (m_frames_left_in_chunk == 0) {
      return 0;
    }

    const int frames_to_read = std::min(frame_count, m_frames_left_in_chunk);
    const int frame_size = m_channel_count * GetSampleSize(m_sample_format);
    const int bytes_to_read = frames_to_read * frame_size;
  
    const int read = m_file->read(buffer, bytes_to_read);
    const int frames_read = read / frame_size;

#ifndef WORDS_BIGENDIAN
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
  AIFFInputStream::reset() {
    // seek to the beginning of the data chunk
    m_frames_left_in_chunk = m_data_chunk_length;
    if (!m_file->seek(m_data_chunk_location, File::BEGIN)) {
      ADR_LOG("Seek in AIFFInputStream::reset");
    }
  }


  bool
  AIFFInputStream::isSeekable() {
    return true;
  }


  int
  AIFFInputStream::getLength() {
    return m_data_chunk_length;
  }


  void
  AIFFInputStream::setPosition(int position) {
    int frame_size = m_channel_count * GetSampleSize(m_sample_format);
    m_frames_left_in_chunk = m_data_chunk_length - position;
    m_file->seek(m_data_chunk_location + position * frame_size, File::BEGIN);
  }


  int
  AIFFInputStream::getPosition() {
    return m_data_chunk_length - m_frames_left_in_chunk;
  }


  bool
  AIFFInputStream::findCommonChunk() {
    ADR_GUARD("AIFFInputStream::findCommonChunk");

    // seek to just after the IFF header
    m_file->seek(12, File::BEGIN);

    // search for a common chunk
    for (;;) {
      u8 chunk_header[8];
      if (m_file->read(chunk_header, 8) != 8) {
        return false;
      }
      u32 chunk_length = read32_be(chunk_header + 4);

      // if we found a format chunk, excellent!
      if (memcmp(chunk_header, "COMM", 4) == 0 && chunk_length >= 18) {
        ADR_LOG("Found common chunk");

        // read common chunk
        u8 chunk[18];
        if (m_file->read(chunk, 18) != 18) {
          return false;
        }

        chunk_length -= 18;

        // parse the memory into useful information
        u16 channel_count   = read16_be(chunk + 0);
        //u32 frame_count     = read32_be(chunk + 2);
        u16 bits_per_sample = read16_be(chunk + 6);
        u32 sample_rate     = readLD_be(chunk + 8);

        // we only support mono and stereo, 8-bit or 16-bit
        if (channel_count > 2 ||
            !isValidSampleSize(bits_per_sample)) {
          ADR_LOG("Invalid AIFF");
          return false;
        }

        // skip the rest of the chunk
        if (!skipBytes(chunk_length)) {
          ADR_LOG("failed skipping rest of common chunk");
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

        // store the other important attributes
        m_channel_count = channel_count;
        m_sample_rate   = sample_rate;
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
  AIFFInputStream::findSoundChunk() {
    ADR_GUARD("AIFFInputStream::findSoundChunk");

    // seek to just after the IFF header
    m_file->seek(12, File::BEGIN);

    // search for a sound chunk
    while (true) {
      u8 chunk_header[8];
      if (m_file->read(chunk_header, 8) != 8) {
        ADR_LOG("Couldn't read SSND chunk header");
        return false;
      }
      u32 chunk_length = read32_be(chunk_header + 4);

      // if we found a data chunk, excellent!
      if (memcmp(chunk_header, "SSND", 4) == 0) {
        ADR_LOG("Found sound chunk");

        u8 chunk_contents[8];
        if (m_file->read(chunk_contents, 8) != 8) {
          ADR_LOG("Couldn't read SSND chunk contents");
          return false;
        }
        if (read32_be(chunk_contents + 0) != 0 ||
            read32_be(chunk_contents + 4) != 0)
        {
          ADR_LOG("Block-aligned AIFF files not supported!");
          return false;
        }

        // calculate the frame size so we can truncate the data chunk
        int frame_size = m_channel_count * GetSampleSize(m_sample_format);

        m_data_chunk_location  = m_file->tell();
        m_data_chunk_length    = (chunk_length - 8) / frame_size;
        m_frames_left_in_chunk = m_data_chunk_length;
        return true;

      } else {

        ADR_IF_DEBUG {
          const u8* ci = chunk_header;
          char str[80];
          sprintf(str, "Skipping: %d bytes in chunk '%c%c%c%c'",
                  (int)chunk_length, ci[0], ci[1], ci[2], ci[3]);
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
  AIFFInputStream::skipBytes(int size) {
    return m_file->seek(size, File::CURRENT);
  }

}
