/**
 * @file
 *
 * Internal platform-specific threads
 */

#ifndef THREADS_H
#define THREADS_H


#include "debug.h"


namespace audiere {

  typedef void (*AI_ThreadRoutine)(void* opaque);

  // threads
  bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority = 0);

  // waiting
  void AI_Sleep(unsigned milliseconds);


  class Mutex {
  public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

  private:
    struct Impl;
    Impl* m_impl;

    friend class CondVar;
  };


  class CondVar {
  public:
    CondVar();
    ~CondVar();

    void wait(Mutex& mutex, float seconds);
    void notify();

  private:
    struct Impl;
    Impl* m_impl;
  };


  class ScopedLock {
  public:
    ScopedLock(Mutex& mutex): m_mutex(mutex) {
      m_mutex.lock();
    }

    ScopedLock(Mutex* mutex): m_mutex(*mutex) {
      m_mutex.lock();
    }

    ~ScopedLock() {
      m_mutex.unlock();
    }

  private:
    Mutex& m_mutex;
  };


  #define SYNCHRONIZED(on) ScopedLock lock_obj__(on)

}

#endif
