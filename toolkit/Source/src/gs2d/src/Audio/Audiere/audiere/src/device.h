#ifndef DEVICE_H
#define DEVICE_H


#include <queue>
#include "audiere.h"
#include "threads.h"


namespace audiere {
  
  /// Basic StopEvent implementation.
  class StopEventImpl : public RefImplementation<StopEvent> {
  public:
    StopEventImpl(OutputStream* os, Reason reason) {
      m_stream = os;
      m_reason = reason;
    }

    OutputStream* ADR_CALL getOutputStream() {
      return m_stream.get();
    }

    Reason ADR_CALL getReason() {
      return m_reason;
    }

  private:
    OutputStreamPtr m_stream;
    Reason m_reason;
  };


  /// Contains default implementation of functionality common to all devices.
  class AbstractDevice : public RefImplementation<AudioDevice> {
  protected:
    AbstractDevice();
    ~AbstractDevice();

  public:
    void ADR_CALL registerCallback(Callback* callback);
    void ADR_CALL unregisterCallback(Callback* callback);
    void ADR_CALL clearCallbacks();

  protected:
    void fireStopEvent(OutputStream* stream, StopEvent::Reason reason);
    void fireStopEvent(const StopEventPtr& event);

  private:
    static void eventThread(void* arg);
    void eventThread();
    void processEvent(Event* event);

    volatile bool m_thread_exists;
    volatile bool m_thread_should_die;

    Mutex m_event_mutex;
    CondVar m_events_available;
    typedef std::queue<EventPtr> EventQueue;
    EventQueue m_events;

    std::vector<CallbackPtr> m_callbacks;
  };

}


#endif
