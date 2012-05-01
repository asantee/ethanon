#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H


#include "audiere.h"
#include "types.h"


namespace audiere {

  class MemoryFile : public RefImplementation<File> {
  public:
    MemoryFile(const void* buffer, int size);
    ~MemoryFile();

    int  ADR_CALL read(void* buffer, int size);
    int  ADR_CALL write(const void* buffer, int size);
    bool ADR_CALL seek(int position, SeekMode mode);
    int  ADR_CALL tell();

  private:
    void ensureSize(int min_size);

    u8* m_buffer;
    int m_position;
    int m_size;

    /// The actual size of m_buffer.  Always a power of two.
    int m_capacity;
  };

}


#endif
