#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include "device_mixer.h"
#include "resampler.h"
#include "utility.h"


namespace audiere {

  MixerDevice::MixerDevice(int rate) {
    m_rate = rate;
  }


  OutputStream*
  MixerDevice::openStream(SampleSource* source) {
    return (source ? new MixerStream(this, source, m_rate) : 0);
  }


  OutputStream*
  MixerDevice::openBuffer(
    void* samples, int frame_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(OpenBufferStream(
      samples, frame_count,
      channel_count, sample_rate, sample_format));
  }


  int
  MixerDevice::read(const int sample_count, void* samples) {
//    ADR_GUARD("MixerDevice::read");

    SYNCHRONIZED(this);

//    ADR_LOG("done locking mixer device");

    // are any sources playing?
    bool any_playing = false;
    for (std::list<MixerStream*>::iterator i = m_streams.begin();
         i != m_streams.end();
         ++i)
    {
      any_playing |= (*i)->m_is_playing;
    }
  
    // if not, return zeroed samples
    if (!any_playing) {
      memset(samples, 0, 4 * sample_count);
      return sample_count;
    }

    ADR_LOG("at least one stream is playing");

    // buffer in which to mix the audio
    static const int BUFFER_SIZE = 4096;

    // mix the output in chunks of BUFFER_SIZE samples
    s16* out = (s16*)samples;
    int left = sample_count;
    while (left > 0) {
      int to_mix = std::min(BUFFER_SIZE, left);

      s32 mix_buffer[BUFFER_SIZE];
      memset(mix_buffer, 0, sizeof(mix_buffer));
    
      for (std::list<MixerStream*>::iterator s = m_streams.begin();
           s != m_streams.end();
           ++s)
      {
        if ((*s)->m_is_playing) {
          s16 stream_buffer[BUFFER_SIZE * 2];
          (*s)->read(to_mix, stream_buffer);
          for (int i = 0; i < to_mix * 2; ++i) {
            mix_buffer[i] += stream_buffer[i];
          }
        }
      }

      // clamp each value in the buffer to the valid s16 range
      for (int i = 0; i < to_mix * 2; ++i) {
        s32 mixed = mix_buffer[i];
        if (mixed < -32768) {
          mixed = -32768;
        } else if (mixed > 32767) {
          mixed = 32767;
        }
        *out++ = (s16)mixed;
      }

      left -= to_mix;
    }

    return sample_count;
  }


  MixerStream::MixerStream(
    MixerDevice* device,
    SampleSource* source,
    int rate)
  {
    m_device     = device;
    m_source     = new Resampler(source, rate);
    m_last_l     = 0;
    m_last_r     = 0;
    m_is_playing = false;
    m_volume     = 255;
    m_pan        = 0;

    SYNCHRONIZED(m_device.get());
    m_device->m_streams.push_back(this);
  }


  MixerStream::~MixerStream() {
    SYNCHRONIZED(m_device.get());
    m_device->m_streams.remove(this);
  }


  void
  MixerStream::play() {
    SYNCHRONIZED(m_device.get());
    m_is_playing = true;
  }


  void
  MixerStream::stop() {
    SYNCHRONIZED(m_device.get());
    if (m_is_playing) {
      m_is_playing = false;
      m_device->fireStopEvent(this, StopEvent::STOP_CALLED);
    } else {
      m_is_playing = false;
    }
  }


  bool
  MixerStream::isPlaying() {
    SYNCHRONIZED(m_device.get());
    return m_is_playing;
  }


  void
  MixerStream::reset() {
    SYNCHRONIZED(m_device.get());
    m_source->reset();
  }


  void
  MixerStream::setRepeat(bool repeat) {
    SYNCHRONIZED(m_device.get());
    m_source->setRepeat(repeat);
  }


  bool
  MixerStream::getRepeat() {
    SYNCHRONIZED(m_device.get());
    return m_source->getRepeat();
  }


  void
  MixerStream::setVolume(float volume) {
    SYNCHRONIZED(m_device.get());
    m_volume = int(volume * 255.0f + 0.5f);
  }


  float
  MixerStream::getVolume() {
    SYNCHRONIZED(m_device.get());
    return (m_volume / 255.0f);
  }


  void
  MixerStream::setPan(float pan) {
    SYNCHRONIZED(m_device.get());
    m_pan = int(pan * 255.0f);
  }


  float
  MixerStream::getPan() {
    SYNCHRONIZED(m_device.get());
    return m_pan / 255.0f;
  }


  void
  MixerStream::setPitchShift(float shift) {
    SYNCHRONIZED(m_device.get());
    m_source->setPitchShift(shift);
  }


  float
  MixerStream::getPitchShift() {
    SYNCHRONIZED(m_device.get());
    return m_source->getPitchShift();
  }


  bool
  MixerStream::isSeekable() {
    return m_source->isSeekable();
  }


  int
  MixerStream::getLength() {
    return m_source->getLength();
  }


  void
  MixerStream::setPosition(int position) {
    SYNCHRONIZED(m_device.get());
    m_source->setPosition(position);
  }


  int
  MixerStream::getPosition() {
    SYNCHRONIZED(m_device.get());
    return m_source->getPosition();
  }


  void
  MixerStream::read(int frame_count, s16* buffer) {
    unsigned read = m_source->read(frame_count, buffer);
    s16* out = buffer;

    // if we are done with the sample source, stop and reset it
    if (read == 0) {
      m_source->reset();
      if (m_is_playing) {
        m_is_playing = false;
        // let subscribers know that the sound was stopped
        m_device->fireStopEvent(this, StopEvent::STREAM_ENDED);
      } else {
        m_is_playing = false;
      }
    } else {
      // do panning and volume normalization
      int l_volume, r_volume;
      if (m_pan < 0) {
        l_volume = 255;
        r_volume = 255 + m_pan;
      } else {
        l_volume = 255 - m_pan;
        r_volume = 255;
      }

      l_volume *= m_volume;
      r_volume *= m_volume;

      for (unsigned i = 0; i < read; ++i) {
        *out = *out * l_volume / 255 / 255;
        ++out;
        *out = *out * r_volume / 255 / 255;
        ++out;
      }
    }

    // if we ready any frames, we can replace the old values
    // for the last left and right channel states
    int new_l = m_last_l;
    int new_r = m_last_r;
    if (read > 0) {
      new_l = out[-2];
      new_r = out[-1];
    }

    // and apply the last state to the rest of the buffer
    for (int i = read; i < frame_count; ++i) {
      *out++ = m_last_l;
      *out++ = m_last_r;
    }

    m_last_l = new_l;
    m_last_r = new_r;
  }

}
