#include <algorithm>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include "device_oss.h"
#include "debug.h"


namespace audiere {

  OSSAudioDevice*
  OSSAudioDevice::create(const ParameterList& parameters) {
    std::string device = parameters.getValue("device", "/dev/dsp");

    // attempt to open output device
    int output_device = open(device.c_str(), O_WRONLY);
    if (output_device == -1) {
      perror(device.c_str());
      return 0;
    }

    int format = AFMT_S16_LE;
    if (ioctl(output_device, SNDCTL_DSP_SETFMT, &format) == -1) {
      perror("SNDCTL_DSP_SETFMT");
      return 0;
    }
    if (format != AFMT_S16_LE) {
      // unsupported format
      return 0;
    }

    int stereo = 1;
    if (ioctl(output_device, SNDCTL_DSP_STEREO, &stereo) == -1) {
      perror("SNDCTL_DSP_STEREO");
      return 0;
    }
    if (stereo != 1) {
      // unsupported channel number
      return 0;
    }

    int speed = 44100;
    if (ioctl(output_device, SNDCTL_DSP_SPEED, &speed) == -1) {
      perror("SNDCTL_DSP_SPEED");
      return 0;
    }
    if (abs(44100 - speed) > 2205) {
      // unsupported sampling rate
      return 0;
    }

    int fragsize = 0x0004000b; // 4 buffers of 2048 bytes each
    if (ioctl(output_device, SNDCTL_DSP_SETFRAGMENT, &fragsize) == -1) {
      perror("SNDCTL_DSP_SETFRAGMENT");
      return 0;
    }

    return new OSSAudioDevice(output_device);
  }


  OSSAudioDevice::OSSAudioDevice(int output_device)
    : MixerDevice(44100)
  {
    m_output_device = output_device;
  }


  OSSAudioDevice::~OSSAudioDevice() {
    ADR_GUARD("OSSAudioDevice::~OSSAudioDevice");
    close(m_output_device);
  }


  void ADR_CALL
  OSSAudioDevice::update() {
    static const int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE * 4];
    read(BUFFER_SIZE, buffer);
    write(m_output_device, buffer, BUFFER_SIZE * 4);
  }


  const char* ADR_CALL
  OSSAudioDevice::getName() {
    return "oss";
  }

}
