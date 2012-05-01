#include <memory>
#include <string.h>
#include "debug.h"
#include "default_file.h"
#ifndef NO_FLAC
#include "input_flac.h"
#endif
#ifndef NO_DUMB
#include "input_mod.h"
#endif
#ifndef NO_MP3
#include "input_mp3.h"
#endif
#ifndef NO_OGG
#include "input_ogg.h"
#endif
#ifndef NO_SPEEX
#include "input_speex.h"
#endif
#include "input_wav.h"
#include "input_aiff.h"
#include "internal.h"
#include "utility.h"


namespace audiere {


  ADR_EXPORT(const char*) AdrGetSupportedFileFormats() {
    return
      "AIFF Files:aiff,aifc"  ";"
#ifndef NO_MP3
      "MP3 Files:mp3,mp2"  ";"
#endif
#ifndef NO_OGG
      "Ogg Vorbis Files:ogg"  ";"
#endif
#ifndef NO_FLAC
      "FLAC Files:flac"  ";"
#endif
#ifndef NO_DUMB
      "Mod Files:mod,s3m,xm,it"  ";"
#endif
#ifndef NO_SPEEX
      "Speex Files:spx"  ";"
#endif
      "WAV Files:wav";
  }


  template<typename T>
  static T* TryInputStream(const FilePtr& file) {

    // initialize should never close the file

    T* source = new T();
    if (source->initialize(file)) {
      return source;
    } else {
      delete source;
      return 0;
    }
  }


#define TRY_SOURCE(source_type) {                             \
  source_type* source = TryInputStream<source_type>(file);    \
  if (source) {                                               \
    return source;                                            \
  } else {                                                    \
    file->seek(0, File::BEGIN);                               \
  }                                                           \
}


#define TRY_OPEN(format) {                                    \
  SampleSource* source = OpenSource(file, filename, format);  \
  if (source) {                                               \
    return source;                                            \
  }                                                           \
}


  bool end_is(const char* begin, const char* ext) {
    const char* end = begin + strlen(begin);
    int ext_length = strlen(ext);
    if (ext_length > end - begin) {
      return false;
    } else {
      return (strcmp_case(end - ext_length, ext) == 0);
    }
  }


  FileFormat GuessFormat(const char* filename) {
    if (end_is(filename, ".aiff")) {
      return FF_AIFF;
    } else if (end_is(filename, ".wav")) {
      return FF_WAV;
    } else if (end_is(filename, ".ogg")) {
      return FF_OGG;
    } else if (end_is(filename, ".flac")) {
      return FF_FLAC;
    } else if (end_is(filename, ".mp3")) {
      return FF_MP3;
    } else if (end_is(filename, ".it") ||
               end_is(filename, ".xm") ||
               end_is(filename, ".s3m") ||
               end_is(filename, ".mod")) {
      return FF_MOD;
    } else if (end_is(filename, ".spx")) {
      return FF_SPEEX;
    } else {
      return FF_AUTODETECT;
    }
  }


  bool end_is(const wchar_t* begin, const wchar_t* ext) {
    const wchar_t* end = begin + wcslen(begin);
    int ext_length = wcslen(ext);
    if (ext_length > end - begin) {
      return false;
    } else {
      return (wcscmp_case(end - ext_length, ext) == 0);
    }
  }

  FileFormat GuessFormat(const wchar_t* filename) {
    if (end_is(filename, L".aiff")) {
      return FF_AIFF;
    } else if (end_is(filename, L".wav")) {
      return FF_WAV;
    } else if (end_is(filename, L".ogg")) {
      return FF_OGG;
    } else if (end_is(filename, L".flac")) {
      return FF_FLAC;
    } else if (end_is(filename, L".mp3")) {
      return FF_MP3;
    } else if (end_is(filename, L".it") ||
               end_is(filename, L".xm") ||
               end_is(filename, L".s3m") ||
               end_is(filename, L".mod")) {
      return FF_MOD;
    } else if (end_is(filename, L".spx")) {
      return FF_SPEEX;
    } else {
      return FF_AUTODETECT;
    }
  }

  /**
   * The internal implementation of OpenSampleSource.
   *
   * @param file         the file to load from.  cannot be 0.
   * @param filename     the name of the file, or 0 if it is not available
   * @param file_format  the format of the file or FF_AUTODETECT
   */
  SampleSource* OpenSource(
    const FilePtr& file,
    const TChar* filename,
    FileFormat file_format)
  {
    ADR_GUARD("OpenSource");
    ADR_ASSERT(file != 0, "file must not be null");

    switch (file_format) {
      case FF_AUTODETECT:
        
        // if filename is available, use it as a hint
        if (filename) {
          FileFormat format = GuessFormat(filename);
          if (format != FF_AUTODETECT) {
            TRY_OPEN(format);
          }
        }

        // autodetect otherwise, in decreasing order of possibility of failure
        TRY_OPEN(FF_AIFF);
        TRY_OPEN(FF_WAV);
        TRY_OPEN(FF_OGG);
        TRY_OPEN(FF_FLAC);
        TRY_OPEN(FF_SPEEX);
        TRY_OPEN(FF_MP3);
        TRY_OPEN(FF_MOD);
        return 0;

#ifndef NO_DUMB
      case FF_MOD:
        TRY_SOURCE(MODInputStream);
        return 0;
#endif

      case FF_AIFF:
        TRY_SOURCE(AIFFInputStream);
        return 0;

      case FF_WAV:
        TRY_SOURCE(WAVInputStream);
        return 0;

#ifndef NO_OGG
      case FF_OGG:
        TRY_SOURCE(OGGInputStream);
        return 0;
#endif

#ifndef NO_MP3
      case FF_MP3:
        TRY_SOURCE(MP3InputStream);
        return 0;
#endif

#ifndef NO_FLAC
      case FF_FLAC:
        TRY_SOURCE(FLACInputStream);
        return 0;
#endif

#ifndef NO_SPEEX
      case FF_SPEEX:
        TRY_SOURCE(SpeexInputStream);
        return 0;
#endif

      default:
        return 0;
    }
  }

  ADR_EXPORT(SampleSource*) AdrOpenSampleSource(
    const TChar* filename,
    FileFormat file_format)
  {
    if (!filename) {
      return 0;
    }
    FilePtr file = OpenFile(filename, false);
    if (!file) {
      return 0;
    }
    return OpenSource(file, filename, file_format);
  }


  ADR_EXPORT(SampleSource*) AdrOpenSampleSourceFromFile(
    File* file,
    FileFormat file_format)
  {
    if (!file) {
      return 0;
    }
    return OpenSource(file, 0, file_format);
  }

}
