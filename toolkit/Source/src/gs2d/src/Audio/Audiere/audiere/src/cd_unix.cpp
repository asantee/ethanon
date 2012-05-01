#include <cdaudio.h>
#include "audiere.h"
#include "debug.h"
#include "internal.h"
#include "utility.h"


namespace audiere {

  class CDDeviceUnix : public RefImplementation<CDDevice> {
  public:
    CDDeviceUnix(int device, const char* name) {
      m_device = device;
      m_name = name;
    }

    ~CDDeviceUnix() {
      stop();
      cd_finish(m_device);
    }

    const char* ADR_CALL getName() {
      return m_name.c_str();
    }

    int ADR_CALL getTrackCount() {
      disc_info disc;
      if (cd_stat(m_device, &disc) == -1) {
	return 0;
      }

      if (containsCD()) {
	return disc.disc_total_tracks;
      } else {
	return 0;
      }
    }

    void ADR_CALL play(int track) {
      cd_play_track(m_device, track + 1, track + 1);
    }

    void ADR_CALL stop() {
      cd_stop(m_device);
    }

    void ADR_CALL pause() {
      cd_pause(m_device);
    }

    void ADR_CALL resume() {
      cd_resume(m_device);
    }

    bool ADR_CALL isPlaying() {
      disc_info disc;
      if (cd_stat(m_device, &disc) == -1) {
	return false;
      }

      return disc.disc_mode == CDAUDIO_PLAYING;
    }

    bool ADR_CALL containsCD() {
      disc_info disc;
      if (cd_stat(m_device, &disc) == -1) {
	return false;
      }

      return disc.disc_present != 0;
    }

    bool ADR_CALL isDoorOpen() {
      ADR_LOG("Warning: libcdaudio does not allow checking the door status.");
      return false;
    }

    void ADR_CALL openDoor() {
      cd_eject(m_device);
    }

    void ADR_CALL closeDoor() {
      cd_close(m_device);
    }

  private:
    int m_device;
    std::string m_name;
  };


  ADR_EXPORT(const char*) AdrEnumerateCDDevices() {
    // There is no clear way to do device enumeration with libcdaudio.
    return "";
  }

  ADR_EXPORT(CDDevice*) AdrOpenCDDevice(const char* name) {
    // Stupid non-const-correct APIs.
    int device = cd_init_device(const_cast<char*>(name));
    if (device == -1) {
      return 0;
    } else {
      return new CDDeviceUnix(device, name);
    }
  }

}
