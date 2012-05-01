#ifndef DEVICE_DS_H
#define DEVICE_DS_H


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <windows.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include <dsound.h>
#include <list>
#include "audiere.h"
#include "device.h"
#include "internal.h"
#include "threads.h"
#include "utility.h"


namespace audiere {

  class DSOutputBuffer;
  class DSOutputStream;

  class DSAudioDevice : public AbstractDevice, public Mutex {
  public:
    static DSAudioDevice* create(const ParameterList& parameters);

  private:
    DSAudioDevice(
      bool global_focus,
      int stream_buffer_length,
      int min_buffer_length,
      HWND anonymous_window,
      IDirectSound* direct_sound);
    ~DSAudioDevice();

public:
    void ADR_CALL update();
    OutputStream* ADR_CALL openStream(SampleSource* source);
    OutputStream* ADR_CALL openBuffer(
      void* samples, int frame_count,
      int channel_count, int sample_rate, SampleFormat sample_format);
    const char* ADR_CALL getName();

    /**
     * DirectSound treats volumes and pan levels as decibels (exponential
     * growth like the Richter scale).  We want a linear ramp.  Do
     * the conversion!
     */
    static int Volume_AudiereToDirectSound(float volume);
    static int Pan_AudiereToDirectSound(float pan);

  private:
    typedef std::list<DSOutputStream*> StreamList;
    typedef std::list<DSOutputBuffer*> BufferList;

    void removeStream(DSOutputStream* stream);
    void removeBuffer(DSOutputBuffer* buffer);

    IDirectSound* m_direct_sound;
    StreamList    m_open_streams;
    BufferList    m_open_buffers;

    bool m_global_focus;
    int m_buffer_length;     ///< length of streaming buffer in milliseconds
    int m_min_buffer_length; ///< minimum buffer size in frames

    HWND m_anonymous_window;

    friend class DSOutputBuffer;
    friend class DSOutputStream;
  };


}


#endif
