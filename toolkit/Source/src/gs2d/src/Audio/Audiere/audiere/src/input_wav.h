#ifndef INPUT_WAV_H
#define INPUT_WAV_H


#include "audiere.h"
#include "basic_source.h"
#include "types.h"


namespace audiere {

  class WAVInputStream : public BasicSource {
  public:
    WAVInputStream();

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int doRead(int frame_count, void* buffer);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    bool findFormatChunk();
    bool findDataChunk();
    bool skipBytes(int size);

  private:
    FilePtr m_file;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    // from data chunk
    int m_data_chunk_location; // bytes
    int m_data_chunk_length;   // in frames

    int m_frames_left_in_chunk;
  };

}


#endif
