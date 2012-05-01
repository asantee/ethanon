#include <stdlib.h>
#include "input_mod.h"
#include "debug.h"


namespace audiere {

  MODInputStream::MODInputStream() {
    m_duh = 0;
    m_renderer = 0;
  }


  MODInputStream::~MODInputStream() {
    if (m_renderer) {
      duh_end_sigrenderer(m_renderer);
      m_renderer = 0;
    }

    if (m_duh) {
      unload_duh(m_duh);
      m_duh = 0;
    }
  }


  bool
  MODInputStream::initialize(FilePtr file) {
    // first time we run, initialize DUMB
    static bool initialized = false;
    if (!initialized) {
      ADR_GUARD("Initializing DUMB");

      atexit(dumb_exit);

      static DUMBFILE_SYSTEM dfs = {
        dfs_open,
        dfs_skip,
        dfs_getc,
        dfs_getnc,
        dfs_close,
      };
      register_dumbfile_system(&dfs);

      // cubic simply takes too much of the CPU...
      dumb_resampling_quality = DUMB_RQ_LINEAR;

      initialized = true;
    }

    m_file = file;

    m_duh = openDUH();
    if (!m_duh) {
      return false;
    }

    m_renderer = duh_start_sigrenderer(m_duh, 0, 2, 0);
    if (!m_renderer) {
      unload_duh(m_duh);
      m_duh = 0;
      return false;
    }

    DUMB_IT_SIGRENDERER* renderer = duh_get_it_sigrenderer(m_renderer);
    dumb_it_set_loop_callback(renderer, &MODInputStream::loopCallback, this);

    return true;
  }


  void
  MODInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = 2;
    sample_rate   = 44100;
    sample_format = SF_S16;
  }


  void
  MODInputStream::reset() {
    ADR_GUARD("MOD_Reset");
    DUH_SIGRENDERER* renderer = duh_start_sigrenderer(m_duh, 0, 2, 0);
    if (renderer) {
      if (m_renderer) {
        duh_end_sigrenderer(m_renderer);
      }
      m_renderer = renderer;

      DUMB_IT_SIGRENDERER* renderer = duh_get_it_sigrenderer(m_renderer);
      dumb_it_set_loop_callback(renderer, &MODInputStream::loopCallback, this);
    }
  }

  
  int
  MODInputStream::doRead(int frame_count, void* buffer) {
    return duh_render(m_renderer, 16, 0, 1.0f, 65536.0f / 44100,
                      frame_count, buffer);
  }


  DUH*
  MODInputStream::openDUH() {
    const char* filename = (const char*)m_file.get();

    DUH* duh = dumb_load_it(filename);
    if (duh) return duh;
    m_file->seek(0, File::BEGIN);

    duh = dumb_load_xm(filename);
    if (duh) return duh;
    m_file->seek(0, File::BEGIN);

    duh = dumb_load_s3m(filename);
    if (duh) return duh;
    m_file->seek(0, File::BEGIN);

    return dumb_load_mod(filename);
  }

  
  void*
  MODInputStream::dfs_open(const char* filename) {
    return const_cast<char*>(filename);
  }

  
  int
  MODInputStream::dfs_skip(void* f, long n) {
    File* file = (File*)f;
    if (file->seek(n, File::CURRENT)) {
      return 0;
    } else {
      return -1;
    }
  }


  int
  MODInputStream::dfs_getc(void* f) {
    File* file = (File*)f;
    unsigned char c;
    if (file->read(&c, 1) == 1) {
      return c;
    } else {
      return -1;
    }
  }


  long
  MODInputStream::dfs_getnc(char* ptr, long n, void* f) {
    File* file = (File*)f;
    return file->read(ptr, n);
  }


  void
  MODInputStream::dfs_close(void* f) {
    // nothing
  }

  int
  MODInputStream::loopCallback(void* ptr) {
    MODInputStream* This = (MODInputStream*)ptr;
    return (This->getRepeat() ? 0 : -1);
  }

}
