// Speex helper library
// Copyright (C) 2002-2003 Janne Hyvarinen
// Parts from speexdec (C) Jean Marc-Valin
//
// Changes:
//  (2003-04-12): Now reports length as double and seek position is specified as double
//  (2002-12-25): Added tag reading, fixed seeking bugs, some other minor changes
//  (2002-11-19): Initial version

#ifndef _speexfile_h
#define _speexfile_h

//#define SL_READER   // uncomment to provide own reader

extern "C" {
#include <speex.h>
#include <speex_header.h>
#include <speex_stereo.h>
#include <speex_callbacks.h>
//#include <misc.h>
#include <ogg/ogg.h>
}

#include "../types.h"

namespace speexfile {
    typedef audiere::u64            uint64_t;
    typedef audiere::s64            int64_t;
    typedef audiere::u32            uint32_t;
    typedef audiere::s32            int32_t;
    typedef unsigned long           ulong_t;
    typedef long double             ldouble_t;
    typedef int64_t                 offset_t;
    typedef float                   audio_sample;

    // -------------------------------------

    class Reader {
    public:
        Reader() {}
        virtual ~Reader() {}
        virtual int read ( void *ptr, int size ) = 0;
        //virtual int write ( void *ptr, int size ) = 0;
        virtual offset_t seek ( offset_t offset ) = 0;
        virtual offset_t get_position() = 0;
        virtual offset_t get_length() = 0;
        //virtual int set_eof ( offset_t ofs ) { return 0; }
        virtual bool can_seek() { return 0; }
    };

    // -------------------------------------

    typedef struct {
        char                *item;          // tag item name
        char                *value;         // tag value (can be NULL)
    } speextags;

    class speexfile {
    private:
        typedef struct {
            int                 frame_size;
            int                 packet_count;
            int                 stream_init;
            int                 enh_enabled;
            int                 nframes;
            int                 forceMode;
            int64_t             samplepos;
            void                *st;
            SpeexBits           bits;
            SpeexStereoState    stereo;
            int                 channels;
            ogg_sync_state      oy;
            ogg_page            og;
            ogg_packet          op;
            ogg_stream_state    os;

            char                *data;
            int                 i, j;
            uint32_t            nb_read;

            bool                init;
            bool                eos, eof;

            int                 decoderpos;
        } speexdecoder;

        typedef struct {
            offset_t            offset;         // byte offset in file
            int64_t             sample;         // sample number in chain
        } speexseekinfo_t;

        typedef struct {
            SpeexHeader         *header;        // Speex header
            speextags           **tags;         // tag fields
            speexseekinfo_t     **seekinfo;     // info to help seeking
            int32_t             tagcount;       // number of tag fields
            int32_t             sicount;        // number of seekpositions
            offset_t            streamsize;     // size in bytes
        } speexstream_t;

    private:
        Reader              *pReader;       // reader
        speexstream_t       **stream;       // stream info
        int32_t             streamcount;    // number of streams
        offset_t            offset;         // position in file
        //long                skipsamples;    // number of samples to skip for accurate seek
        int64_t             seektosample;
        int32_t             current_stream;

    public:
        speexdecoder        *decoder;       // Speex decoder
        bool                seekable;       // file is seekable
        bool                initialized;    // file init was successful
        long                bitrate;        // temporary bitrate on vbr files
        char                speex_last_error[512];
        int32_t             current_serial;

    public:
        speexfile ( Reader *r );
        ~speexfile ();

        int  seek_sample ( int64_t samplepos );                     // seek to given sample position
        int  seek_time   ( double timepos   );                      // seek to given position, time in seconds

        int  decode  ( audio_sample *buffer );                      // decode at maximum 2000 samples to buffer

        int32_t   get_stream    ();                                 // return current stream
        int32_t   get_streams   ();                                 // return number of streams
        int64_t   get_samples   ();                                 // return samples in whole chain
        double    get_duration  ();                                 // return duration of whole chain
        double    get_bitrate   ();                                 // return average bitrate of whole chain

        int32_t   stream_get_samplerate ( int32_t _stream = -1 );   // return samplerate of given stream number (0 = first stream in chain)
        int32_t   stream_get_channels   ( int32_t _stream = -1 );   // return number of channels
        int64_t   stream_get_firstsample( int32_t _stream = -1 );   // return first sample in stream
        int64_t   stream_get_lastsample ( int32_t _stream = -1 );   // return last sample in stream
        int64_t   stream_get_samples    ( int32_t _stream = -1 );   // return number of samples
        double    stream_get_duration   ( int32_t _stream = -1 );   // return duration in seconds
        double    stream_get_bitrate    ( int32_t _stream = -1 );   // return average bitrate in bits / second
        offset_t  stream_get_size       ( int32_t _stream = -1 );   // return size in bytes
        int32_t   stream_get_tagcount   ( int32_t _stream = -1 );   // return number of tag fields
        const speextags**  stream_get_tags        ( int32_t _stream = -1 ); // return pointer to tag struct
        const SpeexHeader* stream_get_speexheader ( int32_t _stream = -1 ); // return pointer to header struct

    private:
        int  initfile ();                                           // read file header and scan lengths and seek positions
        int  init_decoder  ();                                      // initialize Speex decoder
        int  close_decoder ();                                      // free Speex decoder
        int  readtags ( char *tagdata, long size );                 // read tags
        void free_tags ();                                          // free all tags from memory
        void stream_free_tags ( int32_t _stream );                  // free tags from specified stream
        //void *process_header ( ogg_packet *op, int enh_enabled, int *frame_size, int *rate, int *nframes, int forceMode, int *channels, SpeexStereoState *stereo );
        void *header_to_decoder ( SpeexHeader *header, int enh_enabled, int *frame_size, int *rate, int *nframes, int forceMode, int *channels, SpeexStereoState *stereo );
    };

    // -------------------------------------

    #ifdef SL_READER

    class Reader_file : public Reader {
    private:
        HANDLE fh;

    public:
        Reader_file ( HANDLE _fh ) {
            fh = _fh;
        }

        ~Reader_file() {
            if ( fh != INVALID_HANDLE_VALUE ) {
                CloseHandle ( fh );
                fh = INVALID_HANDLE_VALUE;
            }
        }

        virtual int read ( void *ptr, int size ) {
            DWORD bytesread;
            if ( !ReadFile (fh, ptr, size, &bytesread, NULL) ) return 0;
            return bytesread;
        }

        virtual int write ( void *ptr, int size ) {
            DWORD byteswritten;
            if ( !WriteFile (fh, ptr, size, &byteswritten, NULL) ) return 0;
            return byteswritten;
        }

        virtual __int64 seek ( __int64 offset ) {
            LONG high = (DWORD)(offset >> 32);
            DWORD low = SetFilePointer ( fh, (LONG)offset, &high, FILE_BEGIN );
            if ( (low == 0xFFFFFFFF) && (GetLastError() != NO_ERROR) ) return -1;
            return ((__int64)high << 32) | low;
        }

        virtual __int64 get_position() {
            LONG high = 0;
            DWORD low = SetFilePointer ( fh, 0, &high, FILE_CURRENT );
            if ( (low == 0xFFFFFFFF) && (GetLastError() != NO_ERROR) ) return -1;
            return ((__int64)high << 32) | low;
        }

        virtual __int64 get_length() {
            ULONG high;
            DWORD low = GetFileSize ( fh, &high );
            if ( (low == INVALID_FILE_SIZE) && (GetLastError() != NO_ERROR) ) return -1;
            return ((__int64)high << 32) | low;
        }

        virtual int set_eof ( __int64 offset ) {
            if ( seek (offset) != offset ) return 0;
            return SetEndOfFile ( fh );
        };

        virtual bool can_seek () {
            if ( GetFileType (fh) == FILE_TYPE_DISK ) return true;
            return false;
        }
    };

    #endif // SL_READER
}

#endif // _speexfile_h
