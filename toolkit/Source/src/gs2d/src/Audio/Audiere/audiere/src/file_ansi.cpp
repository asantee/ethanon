#include <stdio.h>
#include "debug.h"
#include "default_file.h"


namespace audiere {

  class CFile : public RefImplementation<File> {
  public:
    CFile(FILE* file) {
      ADR_ASSERT(file, "FILE* handle not valid");
      m_file = file;
    }

    ~CFile() {
      fclose(m_file);
    }

    int ADR_CALL read(void* buffer, int size) {
      ADR_ASSERT(buffer, "buffer pointer not valid");
      ADR_ASSERT(size >= 0, "can't read negative number of bytes");
      return fread(buffer, 1, size, m_file);
    }

    bool ADR_CALL seek(int position, SeekMode mode) {
      int m;
      switch (mode) {
        case BEGIN:   m = SEEK_SET; break;
        case CURRENT: m = SEEK_CUR; break;
        case END:     m = SEEK_END; break;
        default: return false;
      }

      return (fseek(m_file, position, m) == 0);
    }

    int ADR_CALL tell() {
      return ftell(m_file);
    }

  private:
    FILE* m_file;
  };


  ADR_EXPORT(File*) AdrOpenFile(const TChar* filename, bool writeable) {
	  FILE* file = 
#if defined(ETH_UNICODE) || defined(_UNICODE) || defined(UNICODE)
		_wfopen(filename, writeable ? L"wb" : L"rb");
#else
		fopen(filename, writeable ? "wb" : "rb");
#endif // ETH_UNICODE || _UNICODE || UNICODE
    return (file ? new CFile(file) : 0);
  }

}
