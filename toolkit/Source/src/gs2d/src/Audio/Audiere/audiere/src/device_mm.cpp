#include "device_mm.h"


namespace audiere {

  static const int RATE = 44100;


  MMAudioDevice*
  MMAudioDevice::create(const ParameterList& parameters) {
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = 2;
    wfx.nSamplesPerSec  = RATE;
    wfx.nAvgBytesPerSec = RATE * 4;
    wfx.nBlockAlign     = 4;
    wfx.wBitsPerSample  = 16;
    wfx.cbSize          = sizeof(WAVEFORMATEX);

    HWAVEOUT handle;
    MMRESULT result = waveOutOpen(&handle, WAVE_MAPPER, &wfx, 0, 0, 0);
    if (result != MMSYSERR_NOERROR) {
      return 0;
    }

    return new MMAudioDevice(handle, RATE);
  }


  MMAudioDevice::MMAudioDevice(HWAVEOUT device, int rate)
    : MixerDevice(rate)
  {
    ADR_GUARD("MMAudioDevice::MMAudioDevice");

    m_device = device;
    m_current_buffer = 0;

    // fill each buffer with samples and prepare it for output
    for (int i = 0; i < BUFFER_COUNT; ++i) {
      WAVEHDR& wh = m_buffers[i];
      memset(&wh, 0, sizeof(wh));
      wh.lpData         = (char*)m_samples + i * BUFFER_LENGTH;
      wh.dwBufferLength = BUFFER_LENGTH;

      read(BUFFER_LENGTH / 4, wh.lpData);

      MMRESULT result = waveOutPrepareHeader(m_device, &wh, sizeof(wh));
      if (result != MMSYSERR_NOERROR) {
        ADR_LOG("waveOutPrepareHeader failed");
      }

      result = waveOutWrite(m_device, &wh, sizeof(wh));
      if (result != MMSYSERR_NOERROR) {
        ADR_LOG("waveOutWrite failed");
      }
    }
  }


  MMAudioDevice::~MMAudioDevice() {
    waveOutReset(m_device);

    for (int i = 0; i < BUFFER_COUNT; ++i) {
      WAVEHDR& wh = m_buffers[i];
      if (wh.dwFlags & WHDR_PREPARED || wh.dwFlags & WHDR_DONE) {
        waveOutUnprepareHeader(m_device, &wh, sizeof(wh));
      }
    }

    waveOutClose(m_device);
  }


  void
  MMAudioDevice::update() {
    ADR_GUARD("MMAudioDevice::update");

    // if a buffer is done playing, add it to the queue again
    for (int i = 0; i < BUFFER_COUNT; ++i) {
      WAVEHDR& wh = m_buffers[i];
      if (wh.dwFlags & WHDR_DONE) {

        // unprepare
        MMRESULT result = waveOutUnprepareHeader(m_device, &wh, sizeof(wh));
        if (result != MMSYSERR_NOERROR) {
          ADR_LOG("waveOutUnprepareHeader failed");
        }

        // fill with new samples
        read(BUFFER_LENGTH / 4, wh.lpData);
        wh.dwFlags = 0;

        // prepare
        result = waveOutPrepareHeader(m_device, &wh, sizeof(wh));
        if (result != MMSYSERR_NOERROR) {
          ADR_LOG("waveOutPrepareHeader failed");
        }

        // write
        result = waveOutWrite(m_device, &wh, sizeof(wh));
        if (result != MMSYSERR_NOERROR) {
          ADR_LOG("waveOutWrite failed");
        }
      }
    }
    Sleep(10);
  }


  const char*
  MMAudioDevice::getName() {
    return "winmm";
  }

}
