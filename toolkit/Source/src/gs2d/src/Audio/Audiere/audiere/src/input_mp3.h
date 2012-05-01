#ifndef INPUT_MP3_H
#define INPUT_MP3_H

#include "mpaudec/mpaudec.h"
#include "audiere.h"
#include "basic_source.h"
#include "types.h"
#include "utility.h"

namespace audiere {

  class MP3InputStream : public BasicSource {
  public:
    MP3InputStream();
    ~MP3InputStream();

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int doRead(int frame_count, void* samples);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();


  private:
    void readID3v1Tags();
    void readID3v2Tags();
    bool decodeFrame();

    FilePtr m_file;
    bool m_eof;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    MPAuDecContext* m_context;
    
    QueueBuffer m_buffer;

    enum { INPUT_BUFFER_SIZE = 4096 };
    u8 m_input_buffer[INPUT_BUFFER_SIZE];
    int m_input_position;
    int m_input_length;
    u8* m_decode_buffer;
    bool m_first_frame;

    bool m_seekable;
    int m_length;
    int m_position;
    std::vector<int> m_frame_sizes;
    std::vector<int> m_frame_offsets;
  };

}

#endif
