/*
  MP3 input for Audiere by Matt Campbell <mattcampbell@pobox.com>, based on
  libavcodec from ffmpeg (http://ffmpeg.sourceforge.net/).  I hope this will
  turn out better than mpegsound did.
*/

#include <string.h>
#include "input_mp3.h"
#include "utility.h"
#include "debug.h"


namespace audiere {


  MP3InputStream::MP3InputStream() {
    m_eof = false;

    m_channel_count = 2;
    m_sample_rate = 44100;
    m_sample_format = SF_S16;

    m_context = 0;

    m_input_position = 0;
    m_input_length = 0;
    m_decode_buffer = 0;
    m_first_frame = true;

    m_seekable = false;
    m_length = 0;
    m_position = 0;
  }

  
  MP3InputStream::~MP3InputStream() {
    delete[] m_decode_buffer;
    if (m_context) {
      mpaudec_clear(m_context);
      delete m_context;
    }
  }


  bool
  MP3InputStream::initialize(FilePtr file) {
    m_file = file;
    m_seekable = m_file->seek(0, File::END);
    readID3v1Tags();
    readID3v2Tags();
    m_file->seek(0, File::BEGIN);
    m_eof = false;

    m_context = new MPAuDecContext;
    if (!m_context)
      return false;
    if (mpaudec_init(m_context) < 0) {
      delete m_context;
      m_context = 0;
      return false;
    }

    m_input_position = 0;
    m_input_length = 0;
    m_decode_buffer = new u8[MPAUDEC_MAX_AUDIO_FRAME_SIZE];
    if (!m_decode_buffer)
        return false;
    m_first_frame = true;

    if (m_seekable) {
      // Scan the file to determine the length.
      m_context->parse_only = 1;
      while (!m_eof) {
        if (!decodeFrame())
          return false;
        if (!m_eof)
          m_frame_sizes.push_back(m_context->frame_size);
          int frame_offset = m_file->tell() -
                             (m_input_length - m_input_position) -
                             m_context->coded_frame_size;
          m_frame_offsets.push_back(frame_offset);
          m_length += m_context->frame_size;
      }
      reset();
    }

    // this should fill in the audio format if it isn't set already
    return decodeFrame();
  }

  bool
  MP3InputStream::isSeekable() {
    return m_seekable;
  }

  int
  MP3InputStream::getPosition() {
     return m_position;
  }

  void
  MP3InputStream::setPosition(int position) {
    if (!m_seekable || position > m_length)
      return;
    int scan_position = 0;
    int target_frame = 0;
    int frame_count = m_frame_sizes.size();
    while (target_frame < frame_count) {
      int frame_size = m_frame_sizes[target_frame];
      if (position <= scan_position + frame_size)
        break;
      else {
        scan_position += frame_size;
        target_frame++;
      }
    }
    // foobar2000's MP3 input plugin decodes and throws away the 10 frames
    // before the target frame whenever possible, presumably to ensure correct
    // output when jumping into the middle of a stream.  So we'll do that here.
    const int MAX_FRAME_DEPENDENCY = 10;
    target_frame = std::max(0, target_frame - MAX_FRAME_DEPENDENCY);
    reset();
    m_file->seek(m_frame_offsets[target_frame], File::BEGIN);
    int i;
    for (i = 0; i < target_frame; i++) {
      m_position += m_frame_sizes[i];
    }
    if (!decodeFrame() || m_eof) {
      reset();
      return;
    }
    int frames_to_consume = position - m_position; // PCM frames now
    if (frames_to_consume > 0) {
      u8 *buf = new u8[frames_to_consume * GetFrameSize(this)];
      doRead(frames_to_consume, buf);
      delete[] buf;
    }
  }

  int
  MP3InputStream::getLength() {
    return m_length;
  }

  void
  MP3InputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate = m_sample_rate;
    sample_format = m_sample_format;
  }

  
  int
  MP3InputStream::doRead(int frame_count, void* samples) {
    ADR_GUARD("MP3InputStream::doRead");

    const int frame_size = GetFrameSize(this);

    int frames_read = 0;
    u8* out = (u8*)samples;

    while (frames_read < frame_count) {

      // no more samples?  ask the MP3 for more
      if (m_buffer.getSize() < frame_size) {
        if (!decodeFrame() || m_eof) {
          // done decoding?
          return frames_read;
        }

        // if the buffer is still empty, we are done
        if (m_buffer.getSize() < frame_size) {
          return frames_read;
        }
      }

      const int frames_left = frame_count - frames_read;
      const int frames_to_read = std::min(
        frames_left,
        m_buffer.getSize() / frame_size);

      m_buffer.read(out, frames_to_read * frame_size);
      out += frames_to_read * frame_size;
      frames_read += frames_to_read;
      m_position += frames_to_read;
    }

    return frames_read;
  }


  void
  MP3InputStream::reset() {
    ADR_GUARD("MP3InputStream::reset");

    m_file->seek(0, File::BEGIN);
    m_eof = false;

    m_buffer.clear();

    mpaudec_clear(m_context);
    mpaudec_init(m_context);

    m_input_position = 0;
    m_input_length = 0;
    m_position = 0;
  }


  bool
  MP3InputStream::decodeFrame() {
    int output_size = 0;
    while (output_size == 0) {
      if (m_input_position == m_input_length) {
        m_input_position = 0;
        m_input_length = m_file->read(m_input_buffer, INPUT_BUFFER_SIZE);
        if (m_input_length == 0) {
          m_eof = true;
          return true;
        }
      }
      int rv = mpaudec_decode_frame(
          m_context, (s16*)m_decode_buffer,
          &output_size,
          (unsigned char*)m_input_buffer + m_input_position,
          m_input_length - m_input_position);
      if (rv < 0)
        return false;
      m_input_position += rv;
    }
    if (m_first_frame) {
      m_channel_count = m_context->channels;
      m_sample_rate = m_context->sample_rate;
      m_sample_format = SF_S16;
      m_first_frame = false;
    } else if (m_context->channels != m_channel_count ||
               m_context->sample_rate != m_sample_rate) {
      // Can't handle format changes mid-stream.
      return false;
    }
    if (!m_context->parse_only) {
      if (output_size < 0) {
        // Couldn't decode this frame.  Too bad, already lost it.
        // This should only happen when seeking.
        output_size = m_context->frame_size;
        memset(m_decode_buffer, 0, output_size * GetFrameSize(this));
      }
      m_buffer.write(m_decode_buffer, output_size);
    }
    return true;
  }


  const char* getGenre(u8 code) {
    const char* genres[] = {
      // From Appendix A.3 at http://www.id3.org/id3v2-00.txt and

      "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
      "Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies", "Other",
      "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial",
      "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
      "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
      "Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
      "Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
      "Soul", "Punk", "Space", "Meditative", "Instrumental Pop",
      "Instrumental Rock", "Ethnic", "Gothic", "Darkwave",
      "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance",
      "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40",
      "Christian Rap", "Pop/Funk", "Jungle", "Native American",
      "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes",
      "Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka",
      "Retro", "Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk-Rock",
      "National Folk", "Swing", "Fast Fusion", "Bebob", "Latin", "Revival",
      "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock",
      "Progressive Rock", "Psychedelic Rock", "Symphonic Rock",
      "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic",
      "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata",
      "Symphony", "Booty Bass", "Primus", "Porn Groove", "Satire",
      "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad",
      "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet", "Punk Rock",
      "Drum Solo", "Acapella", "Euro-House", "Dance Hall",
      
      // http://lame.sourceforge.net/doc/html/id3.html

      "Goa", "Drum & Bass", "Club-House", "Hardcore", "Terror", "Indie",
      "BritPop", "Negerpunk", "Polsk Punk", "Beat", "Christian Gangsta",
      "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
      "Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime",
      "JPop", "SynthPop",
    };
    const int genre_count = sizeof(genres) / sizeof(*genres);

    return (code < genre_count ? genres[code] : "");
  }


  // Return a null-terminated std::string from the beginning of 'buffer'
  // up to 'maxlen' chars in length.
  std::string getString(u8* buffer, int maxlen) {
    char* begin = reinterpret_cast<char*>(buffer);
    int end = 0;
    for (; end < maxlen && begin[end]; ++end) {
    }
    return std::string(begin, begin + end);
  }


  void
  MP3InputStream::readID3v1Tags() {
    // Actually, this function reads both ID3v1 and ID3v1.1.

    if (!m_file->seek(-128, File::END)) {
      return;
    }

    u8 buffer[128];
    if (m_file->read(buffer, 128) != 128) {
      return;
    }

    // Verify that it's really an ID3 tag.
    if (memcmp(buffer + 0, "TAG", 3) != 0) {
      return;
    }

    std::string title   = getString(buffer + 3,  30);
    std::string artist  = getString(buffer + 33, 30);
    std::string album   = getString(buffer + 63, 30);
    std::string year    = getString(buffer + 93, 4);
    std::string comment = getString(buffer + 97, 30);
    std::string genre   = getGenre(buffer[127]);

    addTag("title",   title,   "ID3v1");
    addTag("artist",  artist,  "ID3v1");
    addTag("album",   album,   "ID3v1");
    addTag("year",    year,    "ID3v1");
    addTag("comment", comment, "ID3v1");
    addTag("genre",   genre,   "ID3v1");

    // This is the ID3v1.1 part.
    if (buffer[97 + 28] == 0 && buffer[97 + 29] != 0) {
      char track[20];
      sprintf(track, "%d", int(buffer[97 + 29]));
      addTag("track", track, "ID3v1.1");
    }
  }

  
  void
  MP3InputStream::readID3v2Tags() {
    // ID3v2 is super complicated.
  }

}
