#include <stdlib.h>
#include <string.h>
#include "speexfile.h"

namespace speexfile {

// -------------------------------------

static bool is_speex(Reader *r)
{
    ogg_sync_state   oy;
    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;

    ogg_sync_init ( &oy );

    char *data = ogg_sync_buffer ( &oy, 200 );
    if ( !data ) {
        ogg_sync_clear ( &oy );
        return false;
    }

    int read = r->read ( data, 200 );
    if ( read <= 0 ) {
        ogg_sync_clear ( &oy );
        return false;
    }

    ogg_sync_wrote ( &oy, read );

    bool init = false;
    bool is_our_file = false;

    while ( ogg_sync_pageout (&oy, &og) == 1 && !is_our_file ) {
        if ( !init ) {
            ogg_stream_init ( &os, ogg_page_serialno (&og) );
            init = true;
        }

        ogg_stream_pagein ( &os, &og );

        while ( ogg_stream_packetout (&os, &op) == 1 ) {
            if ( !memcmp (op.packet, "Speex ", 6) ) {
                is_our_file = true;
                break;
            }
        }
    }

    if ( init ) ogg_stream_clear ( &os );
    ogg_sync_clear ( &oy );

    r->seek ( 0 );

    return is_our_file;
}


speexfile::speexfile ( Reader *r )
{
    initialized = false;

    stream              = 0;
    streamcount         = 0;
    offset              = 0;
    decoder             = 0;
    bitrate             = 0;
    seektosample        = -1;
    current_stream      = 0;
    speex_last_error[0] = '\0';
    current_serial      = -1;

    if ( !r ) return;
    if ( !is_speex(r) ) return;
    if ( init_decoder() != 0 ) return;

    pReader = r;
    seekable = (r->can_seek() != 0);

    if ( initfile() != 0 ) return;

    initialized = true;
}

speexfile::~speexfile ()
{
    close_decoder ();
    free_tags ();

    for ( int32_t i = 0; i < streamcount; i++ ) {
        for ( int32_t j = 0; j < stream[i]->sicount; j++ ) {
            if ( stream[i]->seekinfo[j] ) free ( stream[i]->seekinfo[j] );
        }

        if ( stream[i]->seekinfo ) free ( stream[i]->seekinfo );
        if ( stream[i]->header   ) free ( stream[i]->header   );
        free ( stream[i] );
    }

    if ( stream ) {
        free ( stream );
        stream = 0;
    }
}

// -------------------------------------

int speexfile::seek_sample ( int64_t samplepos )
{
    if ( !seekable ) return 0;

    pReader->seek ( 0 );

    offset              = 0;
    decoder->decoderpos = 0;
    decoder->samplepos  = 0;
    seektosample        = samplepos;
    current_stream      = 0;

    return 1;


    offset_t offs     = 0;
    int32_t  streamno = 0;
    int64_t  sample   = 0;
    bool     found    = false;

    while ( streamno < streamcount ) {
        if ( samplepos >= stream_get_firstsample (streamno) &&
             samplepos <= stream_get_lastsample  (streamno) ) {
            found = true;
            break;
        }
        streamno++;
    }

    if ( !found ) return 0;
    if ( stream[streamno]->sicount == 0 ) return 0;
    found = false;

    if ( samplepos >= stream_get_firstsample (streamno) &&
         samplepos <= stream[streamno]->seekinfo[0]->sample ) {
        offs   = stream[streamno]->seekinfo[0]->offset;
        sample = stream_get_firstsample ( streamno );
        found  = true;
    }

    if ( !found ) {
        for ( int32_t i = 1; i < stream[streamno]->sicount; i++ ) {
            if ( samplepos  > stream[streamno]->seekinfo[i-1]->sample &&
                 samplepos <= stream[streamno]->seekinfo[i  ]->sample ) {
                offs   = stream[streamno]->seekinfo[i-1]->offset;
                sample = stream[streamno]->seekinfo[i-1]->sample; //+1;
                found  = true;
                break;
            }
        }
    }

    if ( !found ) return 0;

    pReader->seek ( offs );

    offset = offs;
    decoder->decoderpos = 0;
    decoder->samplepos  = sample;
    seektosample        = samplepos;

    if ( decoder && (current_stream != streamno) ) {
        if ( decoder->st )    // free old decoder
            speex_decoder_destroy ( decoder->st );
        int rate;
        decoder->st = header_to_decoder ( stream[streamno]->header, decoder->enh_enabled, &decoder->frame_size, &rate, &decoder->nframes, decoder->forceMode, &decoder->channels, &decoder->stereo );
        if ( !decoder->st )
            return 0;
    }

    current_stream = streamno;

    return 1;
}

int speexfile::seek_time ( double timepos )
{
    if ( !seekable )
        return 0;

    double  time   = 0;
    int64_t sample = 0;

    for ( int32_t i = 0; i < streamcount; i++ ) {
        if ( timepos >= time &&
             timepos  < time + stream_get_duration (i) ) {
            double delta_s = (timepos - time) * stream_get_samplerate (i);
            return seek_sample ( int64_t(sample + delta_s) );
        }
        time   += stream_get_duration ( i );
        sample += stream_get_samples  ( i );
    }

    return seek_sample ( sample );
}

// -------------------------------------

int speexfile::init_decoder ()
{
    if ( decoder != NULL )
        return -1;

    decoder = (speexdecoder *)calloc ( 1, sizeof (speexdecoder) );
    if ( !decoder ) {
        strcpy ( speex_last_error, "Memory allocation failed" );
        return -1;
    }

    decoder->st             = NULL;
    decoder->enh_enabled    = 1;    // default since 1.0beta4
    decoder->nframes        = 2;
    decoder->forceMode      = -1;
    decoder->channels       = -1;
    decoder->stereo.balance = 1;
    decoder->stereo.e_ratio = 0.5;

    speex_bits_init ( &decoder->bits );

    return 0;
}

int speexfile::close_decoder ()
{
    if ( !decoder )
        return 0;

    speex_bits_destroy ( &decoder->bits );
    if ( decoder->st ) {
        speex_decoder_destroy ( decoder->st );
        decoder->st = NULL;
    }

    free ( decoder );
    decoder = NULL;

    return 0;
}

int speexfile::decode ( audio_sample *buffer )
{
    float decode_buffer[2000];

    if      ( decoder->decoderpos == 1 ) goto decpos1;
    else if ( decoder->decoderpos == 2 ) goto decpos2;

    ogg_sync_init ( &decoder->oy );

    decoder->init = false;
    decoder->eos  = true;
    decoder->eof  = false;

    while ( !decoder->eof ) {
        // Get the ogg buffer for writing
        decoder->data = ogg_sync_buffer ( &decoder->oy, 200 );

        // Read bitstream from input file
        decoder->nb_read = pReader->read ( decoder->data, 200 );
        if ( decoder->nb_read < 200 )
            decoder->eof = true;

        ogg_sync_wrote ( &decoder->oy, decoder->nb_read );

        while ( ogg_sync_pageout (&decoder->oy, &decoder->og) == 1 ) {
            if ( !decoder->init ) {
                current_serial = ogg_page_serialno ( &decoder->og );
                ogg_stream_init ( &decoder->os, current_serial );
                decoder->init = true;
            }

            // Add page to the bitstream
            ogg_stream_pagein ( &decoder->os, &decoder->og );

            // Extract all available packets
            while ( ogg_stream_packetout (&decoder->os, &decoder->op) == 1 ) {
                if ( decoder->op.b_o_s || (current_serial != ogg_page_serialno (&decoder->og)) ) {
                    if ( !decoder->eos ) { // previous stream in chain was broken
                        decoder->init = false;
                        current_stream++;
                    }
                    decoder->eos = false;

                    if ( !seekable ) {
                        void *stream_b = (void *)stream;
                        stream = (speexstream_t **)realloc ( stream, (streamcount+1) * sizeof (speexstream_t*) );
                        if ( !stream ) {
                            stream = (speexstream_t **)stream_b;
                            strcpy ( speex_last_error, "Memory allocation failed" );
                            return -1;
                        }
                    
                        stream[streamcount] = (speexstream_t *)calloc ( 1, sizeof (speexstream_t) );
                        if ( !stream[streamcount] ) {
                            strcpy ( speex_last_error, "Memory allocation failed" );
                            return -1;
                        }
                    
                        stream[streamcount]->seekinfo = (speexseekinfo_t **)calloc ( 1, sizeof (speexseekinfo_t*) );
                        if ( !stream[streamcount]->seekinfo ) {
                            strcpy ( speex_last_error, "Memory allocation failed" );
                            return -1;
                        }

                        stream[streamcount]->header = speex_packet_to_header ( (char *)decoder->op.packet, decoder->op.bytes );
                        if ( !stream[streamcount]->header ) {
                            //strcpy ( speex_last_error, "Cannot read header" );
                            return -1;
                        }

                        stream[streamcount]->tags = NULL;
                        stream[streamcount]->tagcount = 0;

                        streamcount++;
                    }

                    int rate;
                    if ( decoder->st )    // free old decoder
                        speex_decoder_destroy ( decoder->st );

                    decoder->st = header_to_decoder ( stream[current_stream]->header, decoder->enh_enabled, &decoder->frame_size, &rate, &decoder->nframes, decoder->forceMode, &decoder->channels, &decoder->stereo );
                    if ( !decoder->st )
                        return -1;
                }
                if ( (!seekable) && decoder->op.packetno == 1 ) {  // extract tags if not done so yet
                    if ( readtags ((char *)decoder->op.packet, decoder->op.bytes) != 0 ) {
                        stream_free_tags ( get_stream () );
                    }
                }
                if ( decoder->op.packetno > 1 ) {  // header + comments skipped
                    // Copy Ogg packet to Speex bitstream
                    speex_bits_read_from ( &decoder->bits, (char *)decoder->op.packet, decoder->op.bytes );
                    decoder->decoderpos = 1;
                    decoder->j = 0;
decpos1:
                    while ( decoder->j < decoder->nframes ) {
                        decoder->j++;
                        decoder->samplepos += decoder->frame_size;
                        //decoder->samplepos = decoder->op.granulepos;
                        //if ( current_stream > 0 ) decoder->samplepos += stream_get_lastsample ( current_stream-1 );

                        long skipsamples = 0;
                        if ( seektosample > 0 ) skipsamples = (long)(seektosample+decoder->frame_size - decoder->samplepos);
                        if ( skipsamples >= decoder->frame_size ) {
                            skipsamples -= decoder->frame_size;
                            continue;
                        }

                        //Decode frame
                        speex_decode ( decoder->st, &decoder->bits, decode_buffer );
                        if ( decoder->channels == 2 )
                            speex_decode_stereo ( decode_buffer, decoder->frame_size, &decoder->stereo );

                        //speex_decoder_ctl ( decoder->st, SPEEX_GET_BITRATE, &bitrate );

                        for ( int i = 0; i < decoder->frame_size*decoder->channels; i++ ) {
                            buffer[i] = (audio_sample)decode_buffer[i] / 32768.0f;
                        }

                        if ( skipsamples > 0 ) {
                            long ret = (decoder->frame_size - skipsamples);
                            audio_sample *bufptr = (audio_sample *)(buffer + skipsamples * decoder->channels);
                            memmove ( buffer, bufptr, (decoder->frame_size - skipsamples) * decoder->channels * sizeof (audio_sample) );
                            skipsamples = 0;
                            return ret;
                        }

                        return decoder->frame_size;
                    }

                    decoder->decoderpos = 2;
decpos2:
                    // End of stream condition
                    if ( decoder->op.e_o_s ) {
                        decoder->eos  = true;
                        decoder->init = false;
                        current_stream++;
                    }
                }
            }
        }

        offset += decoder->nb_read;
    }

    if ( decoder->init) ogg_stream_clear ( &decoder->os );
    ogg_sync_clear ( &decoder->oy );
    decoder->eof = true;

    return 0;
}

// -------------------------------------

int32_t speexfile::get_stream ()
{
    return current_stream;
}

int32_t speexfile::get_streams ()
{
    return streamcount;
}

int64_t speexfile::get_samples ()
{
    return stream_get_lastsample ( get_streams ()-1 );
}

double speexfile::get_duration ()
{
    double dur = 0;

    for ( int32_t i = 0; i < streamcount; i++ ) {
        dur += stream_get_duration ( i );
    }

    return dur;
}

double speexfile::get_bitrate ()
{
    if ( !seekable )
        return 0.;

    int64_t size = 0;
    for ( int32_t i = 0; i < streamcount; i++ ) {
        size += stream_get_size ( i );
    }

    double duration = get_duration ();
    if ( duration == 0. )
        return 0.;

    return (double)(size * 8.) / (double)duration;
}

// -------------------------------------

int32_t speexfile::stream_get_samplerate ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;
    if ( !stream[_stream]->header ) return 0;

    return stream[_stream]->header->rate;
}

int32_t speexfile::stream_get_channels ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;
    if ( !stream[_stream]->header ) return 0;

    return stream[_stream]->header->nb_channels;
}

int64_t speexfile::stream_get_firstsample ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;
    if ( _stream == 0 ) return 0; // first sample in first stream is 0
    if ( stream[_stream-1]->sicount == 0 ) return 0;

    return stream[_stream-1]->seekinfo[stream[_stream-1]->sicount-1]->sample;
}

int64_t speexfile::stream_get_lastsample ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;
    if ( stream[_stream]->sicount == 0 ) return 0;

    return stream[_stream]->seekinfo[stream[_stream]->sicount-1]->sample;
}

int64_t speexfile::stream_get_samples ( int32_t _stream )
{
    return stream_get_lastsample ( _stream ) - stream_get_firstsample ( _stream );
}

double speexfile::stream_get_duration ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;
    if ( stream_get_samplerate (_stream) == 0 ) return 0;

    return double(stream_get_samples ( _stream ) / stream_get_samplerate ( _stream ));
}

double speexfile::stream_get_bitrate ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0.;

    double dur = stream_get_duration ( _stream );
    if ( dur == 0. ) return 0.;
    offset_t size = stream_get_size ( _stream );
    return (double)(size * 8.) / (double)dur;
}

offset_t speexfile::stream_get_size ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;

    return stream[_stream]->streamsize;
}

const SpeexHeader *speexfile::stream_get_speexheader ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;

    return stream[_stream]->header;
}

const speextags **speexfile::stream_get_tags ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return NULL;

    return (const speextags **)stream[_stream]->tags;
}

int32_t speexfile::stream_get_tagcount ( int32_t _stream )
{
    if ( _stream < 0 ) _stream = get_stream ();
    if ( _stream >= streamcount ) return 0;

    return stream[_stream]->tagcount;
}

// -------------------------------------

#define readint(buf, base) ( ((buf[base+3] << 24) & 0xff000000) | \
                             ((buf[base+2] << 16) & 0x00ff0000) | \
                             ((buf[base+1] <<  8) & 0x0000ff00) | \
                             ((buf[base+0] <<  0) & 0x000000ff) )

int _speex_tagfield_lengths ( const char *str, int len, int *itemlen, int *valuelen )
{
    const char *p   = str;
    const char *end = (char *)(str+len);

    while ( p < end && *p ) {
        char ch = *p++;
        if ( ch == '=' ) {
            *itemlen = p - str - 1;
            *valuelen = len - (p - str);
            return 1;
        }
    }

    *itemlen  = len;
    *valuelen = 0;

    return 0;
}

int speexfile::readtags ( char *tagdata, long size )
{
    if ( streamcount < 1 )
        return -1;
    
    if ( size <= 0 )
        return -1;

    if ( stream[streamcount-1]->tags ) free ( stream[streamcount-1]->tags );
    
    stream[streamcount-1]->tagcount = 0;
    stream[streamcount-1]->tags = (speextags **)malloc ( sizeof (speextags *) );
    if ( stream[streamcount-1]->tags == NULL ) {
        strcpy ( speex_last_error, "Memory allocation failed" );
        return -1;
    }

    char *c = tagdata;
    char *item, *value;
    char *end = (char *)(tagdata+size);
    int len, ilen, vlen, nb_fields;

    if ( c + 4 > end )
        return -1;
    len = readint ( c, 0 );
    if ( c + len > end )
        return -1;
    c += 4;

    if ( _speex_tagfield_lengths (c, len, &ilen, &vlen) != 0 ) {
        item  = c;
        value = c + ilen + 1;
    } else { // first tag field is comment
        item  = "Comment";
        value = c;
        vlen  = len;
        ilen  = strlen ( item );
    }

    /*
    if ( item ) {
        stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item = (char *)malloc ( ilen+1 );
        if ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item == NULL ) {
            strcpy ( speex_last_error, "Memory allocation failed" );
            return -1;
        }
        memcpy ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item, item, ilen );
        stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item[ilen] = '\0';
    } else {
        stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item = NULL;
    }

    if ( value ) {
        stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value = (char *)malloc ( vlen+1 );
        if ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value == NULL ) {
            strcpy ( speex_last_error, "Memory allocation failed" );
            return -1;
        }
        memcpy ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value, value, vlen );
        stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value[vlen] = '\0';
    } else {
        stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value = NULL;
    }
    stream[streamcount-1]->tagcount++;
    */
    c += len;

    if ( c + 4 > end )
        return -1;
    nb_fields = readint ( c, 0 );
    c += 4;

    if ( nb_fields > 0 ) {
        void *tags_b = (void *)stream[streamcount-1]->tags;
        stream[streamcount-1]->tags = (speextags **)realloc ( stream[streamcount-1]->tags, (nb_fields+1) * sizeof (speextags *) );
        if ( stream[streamcount-1]->tags == NULL ) {
            strcpy ( speex_last_error, "Memory allocation failed" );
            return -1;
        }

        for ( int i = 0; i < nb_fields; i++ ) {
            if ( c + 4 > end )
                return -1;
            len = readint ( c, 0 );
            if ( c + len > end )
                return -1;
            c += 4;
            stream[streamcount-1]->tags[stream[streamcount-1]->tagcount] = (speextags *)calloc ( 1, sizeof (speextags) );
            if ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount] == NULL ) {
                strcpy ( speex_last_error, "Memory allocation failed" );
                return -1;
            }

            if ( _speex_tagfield_lengths (c, len, &ilen, &vlen) != 0 ) {
                item  = c;
                value = c + ilen + 1;
            } else {
                item  = c;
                value = NULL;
            }

            if ( item ) {
                stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item = (char *)malloc ( ilen+1 );
                if ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item == NULL ) {
                    strcpy ( speex_last_error, "Memory allocation failed" );
                    return -1;
                }
                memcpy ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item, item, ilen );
                stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item[ilen] = '\0';
            } else {
                stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->item = NULL;
            }

            if ( value ) {
                stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value = (char *)malloc ( vlen+1 );
                if ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value == NULL ) {
                    strcpy ( speex_last_error, "Memory allocation failed" );
                    return -1;
                }
                memcpy ( stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value, value, vlen );
                stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value[vlen] = '\0';
            } else {
                stream[streamcount-1]->tags[stream[streamcount-1]->tagcount]->value = NULL;
            }
            c += len;
            stream[streamcount-1]->tagcount++;
        }
    }

    return 0;
}

void speexfile::stream_free_tags ( int32_t _stream )
{
    if ( _stream >= streamcount || !stream )
        return;

    for ( int32_t i = 0; i < stream[_stream]->tagcount; i++ ) {
        if ( stream[_stream]->tags[i] ) {
            if ( stream[_stream]->tags[i]->item  ) {
                free ( stream[_stream]->tags[i]->item  );
                stream[_stream]->tags[i]->item = NULL;
            }

            if ( stream[_stream]->tags[i]->value ) {
                free ( stream[_stream]->tags[i]->value );
                stream[_stream]->tags[i]->value = NULL;
            }

            free ( stream[_stream]->tags[i] );
            stream[_stream]->tags[i] = NULL;
        }
    }

    if ( stream[_stream]->tags ) {
        free ( stream[_stream]->tags );
        stream[_stream]->tags = NULL;
    }
}

void speexfile::free_tags ()
{
    for ( int32_t i = 0; i < streamcount; i++ ) {
        stream_free_tags ( i );
    }
}

int speexfile::initfile ()
{
    char *data;
    uint32_t nb_read;

    bool init = false;
    bool eos = true;
    bool eof = false;

    ogg_sync_state   oy;
    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;

    int64_t chainsamples = 0;
    
    ogg_sync_init ( &oy );

    if ( !seekable ) return 0;

    while ( !eof ) {
        // Get the ogg buffer for writing
        data = ogg_sync_buffer ( &oy, 200 );

        // Read bitstream from input file
        nb_read = pReader->read ( data, 200 );
        if ( nb_read < 200 )
            eof = true;

        ogg_sync_wrote ( &oy, nb_read );

        while ( ogg_sync_pageout (&oy, &og) == 1 ) {
            if ( !init ) {
                ogg_stream_init ( &os, ogg_page_serialno (&og) );
                init = true;
            }

            // Add page to the bitstream
            ogg_stream_pagein ( &os, &og );

            // Extract all available packets
            while ( ogg_stream_packetout (&os, &op) == 1 ) {
                if ( op.b_o_s ) {
                    if ( !eos ) { // previous stream in chain was broken
                    }
                    eos = false;

                    void *stream_b = (void *)stream;
                    stream = (speexstream_t **)realloc ( stream, (streamcount+1) * sizeof (speexstream_t*) );
                    if ( !stream ) {
                        stream = (speexstream_t **)stream_b;
                        strcpy ( speex_last_error, "Memory allocation failed" );
                        return -1;
                    }
                    
                    stream[streamcount] = (speexstream_t *)calloc ( 1, sizeof (speexstream_t) );
                    if ( !stream[streamcount] ) {
                        strcpy ( speex_last_error, "Memory allocation failed" );
                        return -1;
                    }
                    
                    stream[streamcount]->seekinfo = (speexseekinfo_t **)calloc ( 1, sizeof (speexseekinfo_t*) );
                    if ( !stream[streamcount]->seekinfo ) {
                        strcpy ( speex_last_error, "Memory allocation failed" );
                        return -1;
                    }

                    stream[streamcount]->header = speex_packet_to_header ( (char *)op.packet, op.bytes );
                    if ( !stream[streamcount]->header ) {
                        //strcpy ( speex_last_error, "Cannot read header" );
                        return -1;
                    }

                    stream[streamcount]->tags = NULL;
                    stream[streamcount]->tagcount = 0;

                    if ( (streamcount > 0) && (stream[streamcount-1]->sicount > 0) ) {
                        chainsamples += stream[streamcount-1]->seekinfo[stream[streamcount-1]->sicount-1]->sample;
                    }

                    streamcount++;
                }
                if ( op.e_o_s ) {
                    eos  = true;
                    if ( init ) ogg_stream_clear ( &os );
                    init = false;
                }
                if ( op.packetno == 1 ) {   // extract tags
                    if ( readtags ((char *)op.packet, op.bytes) != 0 ) {
                        stream_free_tags ( streamcount-1 );
                    }
                }
                else if ( (op.packetno > 1) && (op.granulepos > 0) && (streamcount > 0) ) {
                    const int32_t spos = streamcount - 1;

                    void *seekinfo_b = (void *)stream[spos]->seekinfo;
                    stream[spos]->seekinfo = (speexseekinfo_t **)realloc ( stream[spos]->seekinfo, (stream[spos]->sicount+1) * sizeof (speexseekinfo_t*) );
                    if ( !stream[spos]->seekinfo ) {
                        stream[spos]->seekinfo = (speexseekinfo_t **)seekinfo_b;
                        strcpy ( speex_last_error, "Memory allocation failed" );
                        return -1;
                    }

                    stream[spos]->seekinfo[stream[spos]->sicount] = (speexseekinfo_t *)malloc ( sizeof (speexseekinfo_t) );
                    if ( !stream[spos]->seekinfo[stream[spos]->sicount] ) {
                        strcpy ( speex_last_error, "Memory allocation failed" );
                        return -1;
                    }

                    stream[spos]->seekinfo[stream[spos]->sicount]->offset = offset;
                    stream[spos]->seekinfo[stream[spos]->sicount]->sample = chainsamples + op.granulepos;

                    stream[spos]->streamsize += og.header_len + og.body_len;

                    stream[spos]->sicount++;
                }
            }
        }

        offset += nb_read;
    }

    if ( init ) ogg_stream_clear ( &os );
    ogg_sync_clear ( &oy );

    if ( streamcount == 0 ) {
        //strcpy ( speex_last_error, "Not a Speex stream" );
        return -1;
    }

    pReader->seek ( 0 );
    offset = 0;

    return 0;
}

void *speexfile::header_to_decoder ( SpeexHeader *header, int enh_enabled, int *frame_size, int *rate, int *nframes, int forceMode, int *channels, SpeexStereoState *stereo )
{
    void *st;
    SpeexMode *mode;
    SpeexCallback callback;
    int modeID;

    if ( !header ) {
        strcpy ( speex_last_error, "Cannot read header" );
        return NULL;
    }

    if ( header->mode >= SPEEX_NB_MODES ) {
        //sprintf ( speex_last_error, "Mode number %d does not (any longer) exist in this version", header->mode );
        strcpy ( speex_last_error, "Mode does not exist (any longer) in this version" );
        return NULL;
    }

    modeID = header->mode;
    if ( forceMode != -1 )
        modeID = forceMode;
    mode = speex_mode_list[modeID];

    if ( mode->bitstream_version < header->mode_bitstream_version ) {
        strcpy ( speex_last_error, "The file was encoded with a newer version of Speex.\nYou need to upgrade in order to play it." );
        return NULL;
    }
    if ( mode->bitstream_version > header->mode_bitstream_version ) {
        strcpy ( speex_last_error, "The file was encoded with an older version of Speex.\nYou would need to downgrade the version in order to play it." );
        return NULL;
    }

    st = speex_decoder_init ( mode );
    speex_decoder_ctl ( st, SPEEX_SET_ENH, &enh_enabled );
    speex_decoder_ctl ( st, SPEEX_GET_FRAME_SIZE, frame_size );

    callback.callback_id = SPEEX_INBAND_STEREO;
    callback.func = speex_std_stereo_request_handler;
    callback.data = stereo;
    speex_decoder_ctl(st, SPEEX_SET_HANDLER, &callback);

    *rate = header->rate;
    if ( forceMode != -1 ) {
        if ( header->mode < forceMode )
            *rate <<= (forceMode - header->mode);
        if ( header->mode > forceMode )
            *rate >>= (header->mode - forceMode);
    }

    *nframes = header->frames_per_packet;
    if ( *channels == -1 )
        *channels = header->nb_channels;

    return st;
}

}
