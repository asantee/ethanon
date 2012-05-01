#include <algorithm>
#include <sstream>
#include <math.h>
#include "device_ds.h"
#include "device_ds_stream.h"
#include "device_ds_buffer.h"
#include "debug.h"
#include "utility.h"


namespace audiere {

  static const int DEFAULT_BUFFER_LENGTH = 1000;  // one second


  DSAudioDevice*
  DSAudioDevice::create(const ParameterList& parameters) {
    ADR_GUARD("DSAudioDevice::create");

    // parse parameters
    int stream_buffer_length = parameters.getInt("buffer", 0);
    if (stream_buffer_length <= 0) {
      stream_buffer_length = DEFAULT_BUFFER_LENGTH;
    }
    int min_buffer_length = parameters.getInt("min_buffer_size", 0);
    min_buffer_length = std::max(1, min_buffer_length);
    bool global_focus = parameters.getBoolean("global", true);

    // initialize COM
    HRESULT rv = CoInitialize(NULL);
    if (FAILED(rv)) {
      return 0;
    }

    ADR_LOG("COM initialized properly");

    // register anonymous window class
    // don't worry about failure, if it fails, the window creation will fail
    WNDCLASS wc;
    wc.style          = 0;
    wc.lpfnWndProc    = DefWindowProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = GetModuleHandle(NULL);
    wc.hIcon          = NULL;
    wc.hCursor        = NULL;
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "AudiereHiddenWindow";
    RegisterClass(&wc);

    // create anonymous window
    HWND anonymous_window = CreateWindow(
      "AudiereHiddenWindow", "", WS_POPUP,
      0, 0, 0, 0,
      NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!anonymous_window) {
      return false;
    }

    ADR_LOG("Anonymous window created successfully");

    // create the DirectSound object
    IDirectSound* direct_sound;
    rv = CoCreateInstance(
      CLSID_DirectSound,
      NULL,
      CLSCTX_INPROC_SERVER,
      IID_IDirectSound,
      (void**)&direct_sound);
    if (FAILED(rv) || !direct_sound) {
      DestroyWindow(anonymous_window);
      return 0;
    }

    ADR_LOG("Created DS object");

    LPGUID guid = NULL;
    GUID stack_guid;  // so we can point 'guid' to an object that won't be destroyed

    std::string guid_string = parameters.getValue("device_guid", "");
    if (!guid_string.empty()) {
      if (UuidFromString((unsigned char*)guid_string.c_str(), &stack_guid) == RPC_S_OK) {
        guid = &stack_guid;
      }
    }

    // initialize the DirectSound device
    rv = direct_sound->Initialize(guid);
    if (FAILED(rv)) {
      DestroyWindow(anonymous_window);
      direct_sound->Release();
      return 0;
    }

    ADR_LOG("Initialized DS object");

    // set the cooperative level
    rv = direct_sound->SetCooperativeLevel(anonymous_window, DSSCL_NORMAL);
    if (FAILED(rv)) {
      DestroyWindow(anonymous_window);
      direct_sound->Release();
      return 0;
    }

    ADR_LOG("Set cooperative level");

    return new DSAudioDevice(
      global_focus, stream_buffer_length, min_buffer_length,
      anonymous_window, direct_sound);
  }


  DSAudioDevice::DSAudioDevice(
    bool global_focus,
    int stream_buffer_length,
    int min_buffer_length,
    HWND anonymous_window,
    IDirectSound* direct_sound)
  {
    m_global_focus      = global_focus;
    m_buffer_length     = stream_buffer_length;
    m_min_buffer_length = min_buffer_length;
    m_anonymous_window  = anonymous_window;
    m_direct_sound      = direct_sound;
  }


  DSAudioDevice::~DSAudioDevice() {
    ADR_ASSERT(m_open_streams.empty(),
      "DirectSound device should not die with open streams");
    ADR_ASSERT(m_open_buffers.empty(),
      "DirectSound device should not die with open buffers");

    // shut down DirectSound
    if (m_direct_sound) {
      m_direct_sound->Release();
      m_direct_sound = NULL;
    }

    // if the anonymous window is open, close it
    if (m_anonymous_window) {
      DestroyWindow(m_anonymous_window);
      m_anonymous_window = NULL;
    }

    CoUninitialize();
  }


  void
  DSAudioDevice::update() {
    ADR_GUARD("DSAudioDevice::update");

    {
      /* Put the critical section in its own scope so we don't hold the lock
         while sleeping. --MattC */
      SYNCHRONIZED(this);

      // enumerate all open streams
      StreamList::iterator i = m_open_streams.begin();
      while (i != m_open_streams.end()) {
        DSOutputStream* s = *i++;
        s->update();
      }

      // enumerate all open buffers
      BufferList::iterator j = m_open_buffers.begin();
      while (j != m_open_buffers.end()) {
        DSOutputBuffer* b = *j++;
        b->update();
      }
    }

    Sleep(50);
  }


  OutputStream*
  DSAudioDevice::openStream(SampleSource* source) {
    if (!source) {
      return 0;
    }

    ADR_GUARD("DSAudioDevice::openStream");

    int channel_count, sample_rate;
    SampleFormat sample_format;
    source->getFormat(channel_count, sample_rate, sample_format);

    const int frame_size = channel_count * GetSampleSize(sample_format);

    // calculate an ideal buffer size
    const int buffer_length = sample_rate * m_buffer_length / 1000;

    // define the wave format
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = channel_count;
    wfx.nSamplesPerSec  = sample_rate;
    wfx.nAvgBytesPerSec = sample_rate * frame_size;
    wfx.nBlockAlign     = frame_size;
    wfx.wBitsPerSample  = GetSampleSize(sample_format) * 8;
    wfx.cbSize          = sizeof(wfx);

    DSBUFFERDESC dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize        = sizeof(dsbd);
    dsbd.dwFlags       = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
                         DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
    if (m_global_focus) {
      dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
    }
    dsbd.dwBufferBytes = frame_size * buffer_length;
    dsbd.lpwfxFormat   = &wfx;

    // create the DirectSound buffer
    IDirectSoundBuffer* buffer;
    HRESULT result = m_direct_sound->CreateSoundBuffer(&dsbd, &buffer, NULL);
    if (FAILED(result) || !buffer) {
      return 0;
    }

    ADR_LOG("CreateSoundBuffer succeeded");

    // now create the output stream
    DSOutputStream* stream = new DSOutputStream(
      this, buffer, buffer_length, source);

    // add it the list of streams and return
    SYNCHRONIZED(this);
    m_open_streams.push_back(stream);
    return stream;
  }


  OutputStream*
  DSAudioDevice::openBuffer(
    void* samples, int frame_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    ADR_GUARD("DSAudioDevice::openBuffer");

    const int frame_size = channel_count * GetSampleSize(sample_format);

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = channel_count;
    wfx.nSamplesPerSec  = sample_rate;
    wfx.nAvgBytesPerSec = sample_rate * frame_size;
    wfx.nBlockAlign     = frame_size;
    wfx.wBitsPerSample  = GetSampleSize(sample_format) * 8;
    wfx.cbSize          = sizeof(wfx);

    DSBUFFERDESC dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize  = sizeof(dsbd);
    dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPAN |
                   DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
                   DSBCAPS_STATIC | DSBCAPS_CTRLPOSITIONNOTIFY;
    if (m_global_focus) {
      dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
    }

    const int buffer_frame_count = std::max(m_min_buffer_length, frame_count);
    const int buffer_size = buffer_frame_count * frame_size;
    dsbd.dwBufferBytes = buffer_size;
    dsbd.lpwfxFormat   = &wfx;

    // create the DS buffer
    IDirectSoundBuffer* buffer;
    HRESULT result = m_direct_sound->CreateSoundBuffer(
      &dsbd, &buffer, NULL);
    if (FAILED(result) || !buffer) {
      return 0;
    }

    ADR_IF_DEBUG {
      DSBCAPS caps;
      caps.dwSize = sizeof(caps);
      result = buffer->GetCaps(&caps);
      if (FAILED(result)) {
        buffer->Release();
        return 0;
      } else {
        std::ostringstream ss;
        ss << "actual buffer size: " << caps.dwBufferBytes << std::endl
           << "buffer_size: " << buffer_size;
        ADR_LOG(ss.str().c_str());
      }
    }

    void* data;
    DWORD data_size;
    result = buffer->Lock(0, buffer_size, &data, &data_size, 0, 0, 0);
    if (FAILED(result)) {
      buffer->Release();
      return 0;
    }

    ADR_IF_DEBUG {
      std::ostringstream ss;
      ss << "buffer size: " << buffer_size << std::endl
         << "data size:   " << data_size << std::endl
         << "frame count: " << frame_count;
      ADR_LOG(ss.str().c_str());
    }

    const int actual_size = frame_count * frame_size;
    memcpy(data, samples, actual_size);
    memset((u8*)data + actual_size, 0, buffer_size - actual_size);

    buffer->Unlock(data, data_size, 0, 0);

    DSOutputBuffer* b = new DSOutputBuffer(
      this, buffer, buffer_frame_count, frame_size);
    SYNCHRONIZED(this);
    m_open_buffers.push_back(b);
    return b;
  }


  const char* ADR_CALL
  DSAudioDevice::getName() {
    return "directsound";
  }


  void
  DSAudioDevice::removeStream(DSOutputStream* stream) {
    SYNCHRONIZED(this);
    m_open_streams.remove(stream);
  }


  void
  DSAudioDevice::removeBuffer(DSOutputBuffer* buffer) {
    SYNCHRONIZED(this);
    m_open_buffers.remove(buffer);
  }


  int
  DSAudioDevice::Volume_AudiereToDirectSound(float volume) {
    if (volume == 0) {
      return -10000;
    } else {
      double attenuate = 1000 * log(1 / volume);
      return int(-attenuate);
    }
  }


  int
  DSAudioDevice::Pan_AudiereToDirectSound(float pan) {
    if (pan < 0) {
      return -Pan_AudiereToDirectSound(-pan);
    } else {
      return -Volume_AudiereToDirectSound(1 - pan);
    }
  }

}
