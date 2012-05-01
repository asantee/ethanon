#ifndef INPUT_FLAC_H
#define INPUT_FLAC_H


#include <FLAC/seekable_stream_decoder.h>
#include "audiere.h"
#include "basic_source.h"
#include "utility.h"


namespace audiere {

  class FLACInputStream : public BasicSource {
  public:
    FLACInputStream();
    ~FLACInputStream();

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count, 
      int& sample_rate, 
      SampleFormat& sampleformat);
    int doRead(int frame_count, void* samples);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    FLAC__StreamDecoderWriteStatus write(
      const FLAC__Frame* frame,
      const FLAC__int32* const buffer[]);

    static FLAC__SeekableStreamDecoderReadStatus read_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      FLAC__byte buffer[],
      unsigned* bytes,
      void* client_data);
    static FLAC__SeekableStreamDecoderSeekStatus seek_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      FLAC__uint64 absolute_byte_offset,
      void* client_data);
    static FLAC__SeekableStreamDecoderTellStatus tell_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      FLAC__uint64* absolute_byte_offset,
      void* client_data);
    static FLAC__SeekableStreamDecoderLengthStatus length_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      FLAC__uint64* stream_length,
      void* client_data);
    static FLAC__bool eof_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      void* client_data);
    static FLAC__StreamDecoderWriteStatus write_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      const FLAC__Frame* frame,
      const FLAC__int32* const buffer[],
      void* client_data);
    static void metadata_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      const FLAC__StreamMetadata* metadata,
      void* client_data);
    static void error_callback(
      const FLAC__SeekableStreamDecoder* decoder,
      FLAC__StreamDecoderErrorStatus status,
      void* client_data);

    static FLACInputStream* getStream(void* client_data);
    static File* getFile(void* client_data);


    FilePtr m_file;

    FLAC__SeekableStreamDecoder* m_decoder;

    /**
     * This is the buffer used to combine the different channels from FLAC
     * into an interleaved stream.  It is stored in the class so we don't
     * have to do a costly allocation whenever FLAC has more data.
     */
    SizedBuffer m_multiplexer;

    /**
     * this stores a queue of sample data coming from FLAC and being read by
     * the client of the stream
     */
    QueueBuffer m_buffer;

    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    int m_length;
    int m_position;
  };

}


#endif
