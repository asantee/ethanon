#include "device_ds_buffer.h"
#include "device_ds.h"


namespace audiere {

  DSOutputBuffer::DSOutputBuffer(
    DSAudioDevice* device,
    IDirectSoundBuffer* buffer,
    int length,
    int frame_size)
  {
    ADR_GUARD("DSOutputBuffer::DSOutputBuffer");

    m_device     = device; // keep the device alive while the buffer exists
    m_buffer     = buffer;
    m_length     = length;
    m_frame_size = frame_size;

    DWORD frequency;
    m_buffer->GetFrequency(&frequency);
    m_base_frequency = frequency;

    m_repeating  = false;
    m_volume     = 1;
    m_pan        = 0;

    m_stop_event = 0;

    // Set up the stop notification if we can.
    IDirectSoundNotify* notify;
    HRESULT rv = m_buffer->QueryInterface(
        IID_IDirectSoundNotify, (void**)&notify);
    if (SUCCEEDED(rv) && notify) {
      m_stop_event = CreateEvent(0, FALSE, FALSE, 0);
      if (!m_stop_event) {
        ADR_LOG("DSOutputBuffer stop event creation failed.  Not firing stop events.");
      } else {
        DSBPOSITIONNOTIFY n;
        n.dwOffset = DSBPN_OFFSETSTOP;
        n.hEventNotify = m_stop_event;
        notify->SetNotificationPositions(1, &n);
      }
    }
  }


  DSOutputBuffer::~DSOutputBuffer() {
    ADR_GUARD("DSOutputBuffer::~DSOutputBuffer");

    m_device->removeBuffer(this);
    m_buffer->Release();
    if (m_stop_event) {
      CloseHandle(m_stop_event);
    }
  }


  void
  DSOutputBuffer::play() {
    m_buffer->Play(0, 0, m_repeating ? DSBPLAY_LOOPING : 0);
  }


  void
  DSOutputBuffer::stop() {
    m_buffer->Stop();
    m_device->fireStopEvent(this, StopEvent::STOP_CALLED);
  }


  bool
  DSOutputBuffer::isPlaying() {
    DWORD status;
    HRESULT rv = m_buffer->GetStatus(&status);
    return (SUCCEEDED(rv) && status & DSBSTATUS_PLAYING);
  }


  void
  DSOutputBuffer::reset() {
    setPosition(0);
  }


  void
  DSOutputBuffer::setRepeat(bool repeat) {
    m_repeating = repeat;
    if (isPlaying()) {
      // reset the repeating status by calling Play() on the buffer
      play();
    }
  }


  bool
  DSOutputBuffer::getRepeat() {
    return m_repeating;
  }


  void
  DSOutputBuffer::setVolume(float volume) {
    m_volume = volume;
    m_buffer->SetVolume(DSAudioDevice::Volume_AudiereToDirectSound(volume));
  }


  float
  DSOutputBuffer::getVolume() {
    return m_volume;
  }


  void
  DSOutputBuffer::setPan(float pan) {
    m_pan = pan;
    m_buffer->SetPan(DSAudioDevice::Pan_AudiereToDirectSound(pan));
  }


  float
  DSOutputBuffer::getPan() {
    return m_pan;
  }


  void
  DSOutputBuffer::setPitchShift(float shift) {
    m_buffer->SetFrequency(DWORD(m_base_frequency * shift));
  }


  float
  DSOutputBuffer::getPitchShift() {
    DWORD frequency;
    m_buffer->GetFrequency(&frequency);
    return float(frequency) / m_base_frequency;
  }


  bool
  DSOutputBuffer::isSeekable() {
    return true;
  }


  int
  DSOutputBuffer::getLength() {
    return m_length;
  }


  void
  DSOutputBuffer::setPosition(int position) {
    m_buffer->SetCurrentPosition(position * m_frame_size);
  }


  int
  DSOutputBuffer::getPosition() {
    DWORD play;
    m_buffer->GetCurrentPosition(&play, 0);
    return play / m_frame_size;
  }

  void
  DSOutputBuffer::update() {
    if (m_stop_event) {
      DWORD result = WaitForSingleObject(m_stop_event, 0);
      if (result == WAIT_OBJECT_0) {
        m_device->fireStopEvent(this, StopEvent::STREAM_ENDED);
      }
    }
  }

}
