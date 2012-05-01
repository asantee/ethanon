#include <windows.h>
#include <process.h>
#include "threads.h"


namespace audiere {

  struct ThreadInternal {
    AI_ThreadRoutine routine;
    void*            opaque;
  };

  static unsigned WINAPI InternalThreadRoutine(void* opaque);


  bool SupportsThreadPriority() {
    // For some reason or another, Windows 95, 98, and ME always
    // deadlock when Audiere tries to use a thread priority other
    // than 0.  Therefore, disable the higher priority on those
    // operating systems.
    OSVERSIONINFO info;
    info.dwOSVersionInfoSize = sizeof(info);
    if (GetVersionEx(&info) &&
        info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
      return false;
    } else {
      return true;
    }
  }


  int GetWin32Priority(int priority) {
    if (priority < -2) {
      return THREAD_PRIORITY_IDLE;
    } else if (priority == -2) {
      return THREAD_PRIORITY_LOWEST;
    } else if (priority == -1) {
      return THREAD_PRIORITY_BELOW_NORMAL;
    } else if (priority == 0) {
      return THREAD_PRIORITY_NORMAL;
    } else if (priority == 1) {
      return THREAD_PRIORITY_ABOVE_NORMAL;
    } else {
      return THREAD_PRIORITY_HIGHEST;
    }
  }


  bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority) {
    // create internal thread data
    ThreadInternal* internal = new ThreadInternal;
    internal->routine  = routine;
    internal->opaque   = opaque;
    
    // create the actual thread
    unsigned threadid;
    HANDLE handle = (HANDLE)_beginthreadex(
      0, 0, InternalThreadRoutine, internal, CREATE_SUSPENDED, &threadid);
    if (handle) {
      if (SupportsThreadPriority()) {
        SetThreadPriority(handle, GetWin32Priority(priority));
      }
      ResumeThread(handle);
      CloseHandle(handle);
      return true;
    } else {
      return false;
    }
  }


  unsigned WINAPI InternalThreadRoutine(void* opaque) {
    ThreadInternal* internal = static_cast<ThreadInternal*>(opaque);

    // call the function passed 
    internal->routine(internal->opaque);
    delete internal;
    return 0;
  }


  void AI_Sleep(unsigned milliseconds) {
    ::Sleep(milliseconds);
  }


  struct Mutex::Impl {
    CRITICAL_SECTION cs;
  };

  Mutex::Mutex() {
    m_impl = new Impl;
    InitializeCriticalSection(&m_impl->cs);
  }

  Mutex::~Mutex() {
    DeleteCriticalSection(&m_impl->cs);
    delete m_impl;
  }

  void Mutex::lock() {
    EnterCriticalSection(&m_impl->cs);
  }

  void Mutex::unlock() {
    LeaveCriticalSection(&m_impl->cs);
  }


  // This probably isn't a safe way to implement a condition variable.  I suspect
  // wait() isn't totally thread-safe, i.e. multiple threads waiting on a cvar could
  // unlock the mutex at bad times.

  struct CondVar::Impl {
    HANDLE event;
  };

  CondVar::CondVar() {
    ADR_GUARD("CondVar::CondVar");
    m_impl = new Impl;
    m_impl->event = CreateEvent(0, FALSE, FALSE, 0);
    if (!m_impl->event) {
      ADR_LOG("CreateEvent() failed");
      abort();
    }
  }

  CondVar::~CondVar() {
    CloseHandle(m_impl->event);
    delete m_impl;
  }

  void CondVar::wait(Mutex& mutex, float seconds) {
    mutex.unlock();
    WaitForSingleObject(m_impl->event, int(seconds * 1000));
    mutex.lock();
  }

  void CondVar::notify() {
    SetEvent(m_impl->event);
  }

}
