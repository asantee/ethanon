#include "input_flac.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  FLACInputStream::FLACInputStream() {
    m_decoder = 0;

    m_channel_count = 0;
    m_sample_rate   = 0;
    m_sample_format = SF_S16;

    m_length = 0;
    m_position = 0;
  }


  FLACInputStream::~FLACInputStream() {
    if (m_decoder) {
      FLAC__seekable_stream_decoder_finish(m_decoder);
      FLAC__seekable_stream_decoder_delete(m_decoder);
      m_decoder = 0;
    }
  }


  bool
  FLACInputStream::initialize(FilePtr file) {
    m_file = file;

    // initialize the decoder
    m_decoder = FLAC__seekable_stream_decoder_new();
    if (!m_decoder) {
      m_file = 0;
      return false;
    }

#define SET_CALLBACK(name)                                   \
  FLAC__seekable_stream_decoder_set_##name##_callback(       \
    m_decoder,                                               \
    name##_callback)

    // set callbacks
    FLAC__seekable_stream_decoder_set_client_data      (m_decoder, this);
    SET_CALLBACK(read);
    SET_CALLBACK(seek);
    SET_CALLBACK(tell);
    SET_CALLBACK(length);
    SET_CALLBACK(eof);
    SET_CALLBACK(write);
    SET_CALLBACK(metadata);
    SET_CALLBACK(error);

    FLAC__SeekableStreamDecoderState state =
      FLAC__seekable_stream_decoder_init(m_decoder);
    if (state != FLAC__SEEKABLE_STREAM_DECODER_OK) {
      FLAC__seekable_stream_decoder_finish(m_decoder);
      FLAC__seekable_stream_decoder_delete(m_decoder);
      m_decoder = 0;
      m_file = 0;
      return false;
    }

    // make sure we have metadata before we return!
    if (!FLAC__seekable_stream_decoder_process_until_end_of_metadata(m_decoder)) {
      FLAC__seekable_stream_decoder_finish(m_decoder);
      FLAC__seekable_stream_decoder_delete(m_decoder);
      m_decoder = 0;
      m_file = 0;
      return false;
    }

    // process one frame so we can do something!
    if (!FLAC__seekable_stream_decoder_process_single(m_decoder)) {
      FLAC__seekable_stream_decoder_finish(m_decoder);
      FLAC__seekable_stream_decoder_delete(m_decoder);
      m_decoder = 0;
      m_file = 0;
      return false;
    }

    // get info about the flac file
    m_channel_count = FLAC__seekable_stream_decoder_get_channels(m_decoder);
    m_sample_rate   = FLAC__seekable_stream_decoder_get_sample_rate(m_decoder);
    int bps         = FLAC__seekable_stream_decoder_get_bits_per_sample(m_decoder);
    if (bps == 16) {
      m_sample_format = SF_S16;
    } else if (bps == 8) {
      m_sample_format = SF_U8;
    } else {
      return false;
    }

    return true;
  }


  void
  FLACInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }


  int
  FLACInputStream::doRead(int frame_count, void* samples) {
    const int frame_size = m_channel_count * GetSampleSize(m_sample_format);
    u8* out = (u8*)samples;
    
    // we keep reading till we finish topping up!
    int frames_read = 0;
    while (frames_read < frame_count) {
      
      // if the buffer is empty, ask FLAC to fill it p
      if (m_buffer.getSize() < frame_size) {
        if (!FLAC__seekable_stream_decoder_process_single(m_decoder)) {
          return frames_read;
        }

        // if the buffer still has a size of 0, we are probably at the
        // end of the stream
        if (m_buffer.getSize() < frame_size) {
          return frames_read;
        }
      }

      // read what we've got!
      const int to_read = std::min(
        frame_count - frames_read,
        m_buffer.getSize() / frame_size);
      m_buffer.read(out, to_read * frame_size);
      out += to_read * frame_size;
      frames_read += to_read;
    }

    return frames_read;
  }


  void
  FLACInputStream::reset() {
    m_file->seek(0, File::BEGIN);
    FLAC__seekable_stream_decoder_seek_absolute(m_decoder, 0);
    m_position = 0;
    m_buffer.clear();
  }


  bool
  FLACInputStream::isSeekable() {
    return (m_length != 0);
  }


  int
  FLACInputStream::getLength() {
    return m_length;
  }


  void
  FLACInputStream::setPosition(int position) {
    if (FLAC__seekable_stream_decoder_seek_absolute(m_decoder, position)) {
      m_position = position;
    }
  }


  int
  FLACInputStream::getPosition() {
    int bytes_per_frame = m_channel_count * GetSampleSize(m_sample_format);
    return m_position - (m_buffer.getSize() / bytes_per_frame);
  }


  FLAC__StreamDecoderWriteStatus
  FLACInputStream::write(
    const FLAC__Frame* frame,
    const FLAC__int32* const buffer[])
  {
    int channel_count = frame->header.channels;
    int samples_per_channel = frame->header.blocksize;
    int bytes_per_sample = frame->header.bits_per_sample / 8;
    int total_size = channel_count * samples_per_channel * bytes_per_sample;

    m_multiplexer.ensureSize(total_size);

    // do the multiplexing/interleaving
    if (bytes_per_sample == 1) {
      u8* out = (u8*)m_multiplexer.get();
      for (int s = 0; s < samples_per_channel; ++s) {
        for (int c = 0; c < channel_count; ++c) {
          // is this right?
          *out++ = (u8)buffer[c][s];
        }
      }
    } else if (bytes_per_sample == 2) {
      s16* out = (s16*)m_multiplexer.get();
      for (int s = 0; s < samples_per_channel; ++s) {
        for (int c = 0; c < channel_count; ++c) {
          *out++ = (s16)buffer[c][s];
        }
      }
    } else {
      return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    m_buffer.write(m_multiplexer.get(), total_size);
    m_position += samples_per_channel;
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
  }


  FLAC__SeekableStreamDecoderReadStatus FLACInputStream::read_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    FLAC__byte buffer[],
    unsigned *bytes,
    void* client_data)
  {
    *bytes = getFile(client_data)->read(buffer, *bytes);
    if (*bytes == 0) {
      return FLAC__SEEKABLE_STREAM_DECODER_READ_STATUS_ERROR;
    } else {
      return FLAC__SEEKABLE_STREAM_DECODER_READ_STATUS_OK;
    }
  }


  FLAC__SeekableStreamDecoderSeekStatus FLACInputStream::seek_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    FLAC__uint64 absolute_byte_offset,
    void* client_data)
  {
    if (getFile(client_data)->seek(static_cast<int>(absolute_byte_offset), File::BEGIN)) {
      return FLAC__SEEKABLE_STREAM_DECODER_SEEK_STATUS_OK;
    } else {
      return FLAC__SEEKABLE_STREAM_DECODER_SEEK_STATUS_ERROR;
    }
  }


  FLAC__SeekableStreamDecoderTellStatus FLACInputStream::tell_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    FLAC__uint64* absolute_byte_offset,
    void* client_data)
  {
    *absolute_byte_offset = getFile(client_data)->tell();
    return FLAC__SEEKABLE_STREAM_DECODER_TELL_STATUS_OK;
  }


  FLAC__SeekableStreamDecoderLengthStatus FLACInputStream::length_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    FLAC__uint64* stream_length,
    void* client_data)
  {
    *stream_length = GetFileLength(getFile(client_data));
    return FLAC__SEEKABLE_STREAM_DECODER_LENGTH_STATUS_OK;
  }


  FLAC__bool FLACInputStream::eof_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    void* client_data)
  {
    File* file = getFile(client_data);
    return (file->tell() == GetFileLength(file));
  }


  FLAC__StreamDecoderWriteStatus FLACInputStream::write_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    const FLAC__Frame* frame,
    const FLAC__int32* const buffer[],
    void* client_data)
  {
    return getStream(client_data)->write(frame, buffer);
  }


  void FLACInputStream::metadata_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    const FLAC__StreamMetadata *metadata,
    void* client_data)
  {
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
      FLAC__uint64 length = metadata->data.stream_info.total_samples;
      getStream(client_data)->m_length = static_cast<int>(length);
    }
  }


  void FLACInputStream::error_callback(
    const FLAC__SeekableStreamDecoder* decoder,
    FLAC__StreamDecoderErrorStatus status,
    void* client_data)
  {
    // don't handle any errors
  }


  FLACInputStream* FLACInputStream::getStream(void* client_data) {
    return static_cast<FLACInputStream*>(client_data);
  }

  File* FLACInputStream::getFile(void* client_data) {
    return getStream(client_data)->m_file.get();
  }
}
