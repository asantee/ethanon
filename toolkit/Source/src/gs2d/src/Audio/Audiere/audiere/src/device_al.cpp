#include <errno.h>
#include <unistd.h>
#include "device_al.h"
#include "debug.h"


namespace audiere {

  ALAudioDevice*
  ALAudioDevice::create(const ParameterList& parameters) {
    ADR_GUARD("ALAudioDevice::create");

    // if anything goes wrong, assume 44100 Hz
    int rate = 44100;

    ADR_LOG("Getting resource");

    int device = alGetResourceByName(AL_SYSTEM, "DefaultOut", AL_DEVICE_TYPE);
    if (device) {
      ADR_LOG("Getting sampling rate");

      ALpv pv;
      pv.param = AL_RATE;
      if (-1 == alGetParams(device, &pv, 1)) {
        fprintf(stderr, "Couldn't get rate: %s\n",
                alGetErrorString(oserror()));
      }
      rate = pv.value.i;
    } else {
      fprintf(stderr, "Couldn't get DefaultOut resource: %s\n",
              alGetErrorString(oserror()));
    }

    ADR_LOG("Creating config");

    ALconfig config = alNewConfig();
    if (!config) {
      fprintf(stderr, "Couldn't create ALconfig: %s\n",
              alGetErrorString(oserror()));
      return 0;
    }

    ADR_LOG("Setting channels");

    // stereo
    int result = alSetChannels(config, 2);
    if (result != 0) {
      fprintf(stderr, "Couldn't set channels: %s\n",
              alGetErrorString(oserror()));
      alFreeConfig(config);
      return 0;
    }

    ADR_LOG("Opening port");

    ALport port = alOpenPort("Audiere Output Port", "w", config);
    if (!port) {
      fprintf(stderr, "Couldn't open port: %s\n",
              alGetErrorString(oserror()));
      alFreeConfig(config);
      return 0;
    }

    ADR_LOG("Creating audio device");

    alFreeConfig(config);
    return new ALAudioDevice(port, rate);
  }


  ALAudioDevice::ALAudioDevice(ALport port, int rate)
    : MixerDevice(rate)
  {
    ADR_GUARD("ALAudioDevice::ALAudioDevice");

    m_port = port;
  }


  ALAudioDevice::~ALAudioDevice() {
    ADR_GUARD("ALAudioDevice::~ALAudioDevice");

    alClosePort(m_port);
  }


  void
  ALAudioDevice::update() {
    ADR_GUARD("ALAudioDevice::update");

    // how much data can we write?
    const int filled = alGetFilled(m_port);
    int can_write = 5000 - filled;  // empty portion of the buffer

    // write 1024 frames at a time
    static const int BUFFER_SIZE = 1024;
    u8 buffer[BUFFER_SIZE * 4];
    while (can_write > 0) {
      int transfer_count = std::min(can_write, BUFFER_SIZE);

      ADR_LOG("reading");

      read(transfer_count, buffer);

      ADR_LOG("writing");

      alWriteFrames(m_port, buffer, transfer_count);
      can_write -= transfer_count;
    }

    usleep(50000);  // 50 milliseconds
  }


  const char* ADR_CALL
  ALAudioDevice::getName() {
    return "sgial";
  }

}
