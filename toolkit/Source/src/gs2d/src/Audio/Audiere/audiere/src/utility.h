#ifndef UTILITY_H
#define UTILITY_H


#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <map>
#include <string>
#include <utility>
#include "audiere.h"
#include "types.h"


#if defined(_MSC_VER) && _MSC_VER <= 1200 && !defined(_STLPORT_VERSION)

  // std::min and std::max are broken in VC++ 6, so define our own.
  // Unfortunately, this means we must include utility.h to use
  // std::min and std::max
  namespace std {

    #ifdef min
      #undef min
    #endif

    #ifdef max
      #undef max
    #endif

    template<typename T>
    inline T min(T a, T b) {
      return (a < b ? a : b);
    }

    template<typename T>
    inline T max(T a, T b) {
      return (a > b ? a : b);
    }
  }

#else

  #include <algorithm>

#endif


namespace audiere {


  template<typename T>
  T clamp(T min, T x, T max) {
    return std::max(std::min(x, max), min);
  }


  class ParameterList {
  public:
    ParameterList(const char* parameters);
    std::string getValue(const std::string& key, const std::string& defValue) const;
    bool getBoolean(const std::string& key, bool def) const;
    int getInt(const std::string& key, int def) const;

  private:
    std::map<std::string, std::string> m_values;
  };

  int strcmp_case(const char* a, const char* b);
  int wcscmp_case(const wchar_t* a, const wchar_t* b);


  inline int GetFrameSize(SampleSource* source) {
    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);
    return GetSampleSize(sample_format) * channel_count;
  }
  
  inline int GetFrameSize(const SampleSourcePtr& source) {
    return GetFrameSize(source.get());
  }


  inline u16 read16_le(const u8* b) {
    return b[0] + (b[1] << 8);
  }

  inline u16 read16_be(const u8* b) {
    return (b[0] << 8) + b[1];
  }

  inline u32 read32_le(const u8* b) {
    return read16_le(b) + (read16_le(b + 2) << 16);
  }

  inline u32 read32_be(const u8* b) {
    return (read16_be(b) << 16) + read16_be(b + 2);
  }

  /// Converts an 80-bit IEEE 754 floating point number to a u32.
  inline u32 readLD_be(const u8* b) {
    u32 mantissa = read32_be(b + 2);
    u8 exp = 30 - b[1];
    u32 last = 0;
    while (exp--) {
      last = mantissa;
      mantissa >>= 1;
    }
    if (last & 0x1) {
      mantissa++;
    }
    return mantissa;
  }


  inline int GetFileLength(File* file) {
    int pos = file->tell();
    file->seek(0, File::END);
    int length = file->tell();
    file->seek(pos, File::BEGIN);
    return length;
  }


  inline SampleSource* OpenBufferStream(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return CreateSampleBuffer(
      samples, sample_count,
      channel_count, sample_rate, sample_format)->openStream();
  }


  class QueueBuffer {
  public:
    QueueBuffer() {
      m_capacity = 256;
      m_size = 0;

      m_buffer = (u8*)malloc(m_capacity);
    }

    ~QueueBuffer() {
      m_buffer = (u8*)realloc(m_buffer, 0);
    }

    int getSize() {
      return m_size;
    }

    void write(const void* buffer, int size) {
      bool need_realloc = false;
      while (size + m_size > m_capacity) {
        m_capacity *= 2;
        need_realloc = true;
      }

      if (need_realloc) {
        m_buffer = (u8*)realloc(m_buffer, m_capacity);
      }

      memcpy(m_buffer + m_size, buffer, size);
      m_size += size;
    }

    int read(void* buffer, int size) {
      int to_read = (std::min)(size, m_size); // aqui
      memcpy(buffer, m_buffer, to_read);
      memmove(m_buffer, m_buffer + to_read, m_size - to_read);
      m_size -= to_read;
      return to_read;
    }

    void clear() {
      m_size = 0;
    }

  private:
    u8* m_buffer;
    int m_capacity;
    int m_size;

    // private and unimplemented to prevent their use
    QueueBuffer(const QueueBuffer&);
    QueueBuffer& operator=(const QueueBuffer&);
  };


  class SizedBuffer {
  public:
    SizedBuffer() {
      m_capacity = 256;
      m_buffer = malloc(m_capacity);
    }

    ~SizedBuffer() {
      m_buffer = realloc(m_buffer, 0);
    }

    void ensureSize(int size) {
      bool need_realloc = false;
      while (m_capacity < size) {
        m_capacity *= 2;
        need_realloc = true;
      }
      if (need_realloc) {
        m_buffer = realloc(m_buffer, m_capacity);
      }
    }

    void* get() {
      return m_buffer;
    }

  private:
    void* m_buffer;
    int m_capacity;

    // private and unimplemented to prevent their use
    SizedBuffer(const SizedBuffer&);
    SizedBuffer& operator=(const SizedBuffer&);
  };

}


#endif
