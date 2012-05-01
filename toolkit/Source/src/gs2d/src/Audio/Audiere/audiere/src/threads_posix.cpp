#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "threads.h"
#include "utility.h"


namespace audiere {

  struct ThreadInternal {
    AI_ThreadRoutine routine;
    void* opaque;
  };


  void* ThreadRoutine(void* arg) {
    ThreadInternal* ti = (ThreadInternal*)arg;
    ti->routine(ti->opaque);
    delete ti;
    return NULL;
  }


  bool AI_CreateThread(AI_ThreadRoutine routine, void* opaque, int priority) {
    ThreadInternal* ti = new ThreadInternal;
    ti->routine = routine;
    ti->opaque  = opaque;

    pthread_attr_t attr;
    if (pthread_attr_init(&attr)) {
      delete ti;
      return false;
    }

    // get default scheduling policy
    int policy;
    if (pthread_attr_getschedpolicy(&attr, &policy)) {
      pthread_attr_destroy(&attr);
      delete ti;
      return false;
    }

    int min_prio = sched_get_priority_min(policy);
    int max_prio = sched_get_priority_max(policy);

    // get default scheduling parameters
    sched_param sched;
    if (pthread_attr_getschedparam(&attr, &sched)) {
        pthread_attr_destroy(&attr);
        delete ti;
        return false;
    }

    // treat the specified priority as an offset from the default one
    sched.sched_priority = clamp(
        min_prio,
        sched.sched_priority + priority,
        max_prio);

    if (pthread_attr_setschedparam(&attr, &sched)) {
        pthread_attr_destroy(&attr);
        delete ti;
        return false;
    }

    pthread_t thread;
    int result = pthread_create(&thread, &attr, ThreadRoutine, ti);
    if (result != 0) {
      pthread_attr_destroy(&attr);
      delete ti;
      return false;
    }

    pthread_attr_destroy(&attr);
    return true;
  }


  void AI_Sleep(unsigned milliseconds) {
    int seconds = milliseconds / 1000;
    int useconds = (milliseconds % 1000) * 1000;

    sleep(seconds);
    usleep(useconds);
  }


  struct Mutex::Impl {
    pthread_mutex_t mutex;
  };

  Mutex::Mutex() {
    m_impl = new Impl;
    int result = pthread_mutex_init(&m_impl->mutex, 0);
    if (result != 0) {
      delete m_impl;
      m_impl = 0;
      ADR_LOG("pthread_mutex_init() failed in Mutex::Mutex()");
      abort();
    }
  }

  Mutex::~Mutex() {
    pthread_mutex_destroy(&m_impl->mutex);
    delete m_impl;
  }

  void Mutex::lock() {
    pthread_mutex_lock(&m_impl->mutex);
  }

  void Mutex::unlock() {
    pthread_mutex_unlock(&m_impl->mutex);
  }


  struct CondVar::Impl {
    pthread_cond_t cond;
  };

  CondVar::CondVar() {
    m_impl = new Impl;
    int result = pthread_cond_init(&m_impl->cond, 0);
    if (result != 0) {
        delete m_impl;
        m_impl = 0;
        ADR_LOG("pthread_cond_init() failed in CondVar::CondVar()");
        abort();
    }
  }

  CondVar::~CondVar() {
    pthread_cond_destroy(&m_impl->cond);
    delete m_impl;
  }

  void CondVar::wait(Mutex& mutex, float seconds) {
    double ds = seconds;  // May need greater than float precision.
    
    timeval tv;
    gettimeofday(&tv, 0);
    ds += tv.tv_sec + tv.tv_usec / 1000000000.0;
    
    timespec ts;
    ts.tv_sec  = int(ds);
    ts.tv_nsec = int((ds - floor(ds)) * 1000000000);
    pthread_cond_timedwait(&m_impl->cond, &mutex.m_impl->mutex, &ts);
  }

  void CondVar::notify() {
    pthread_cond_signal(&m_impl->cond);
  }

}
