#ifndef INPUT_SPEEX_H
#define INPUT_SPEEX_H


#include <memory>
#include "speexfile/speexfile.h"
#include "audiere.h"
#include "basic_source.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class SpeexInputStream : public BasicSource {
  public:
    SpeexInputStream();
    ~SpeexInputStream();

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
    // Defined by speexfile API.
    enum { BUFFER_SIZE = 2000 };

    std::auto_ptr<speexfile::Reader> m_reader;

    speexfile::speexfile* m_speexfile;
    int m_position;  // Need to remember this because m_speexfile doesn't.

    QueueBuffer m_read_buffer;
  };

}


#endif
