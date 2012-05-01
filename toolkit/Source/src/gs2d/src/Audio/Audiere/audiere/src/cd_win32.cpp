#include "audiere.h"
#include "internal.h"
#include "mci_device.h"
#include "utility.h"


namespace audiere {

  class CDDeviceWin32 : public RefImplementation<CDDevice>, public MCIDevice {
  public:
    CDDeviceWin32(const char* name)
      : MCIDevice(std::string(name) + "\\")
    {
      m_name = name;

      sendCommand("open", "type cdaudio");

      // Needed for play()
      sendCommand("set", "time format tmsf");
    }

    ~CDDeviceWin32() {
      stop();
    }

    const char* ADR_CALL getName() {
      return m_name.c_str();
    }

    int ADR_CALL getTrackCount() {
      return atoi(sendCommand("status", "number of tracks").c_str());
    }

    void ADR_CALL play(int track) {
      char from[1000];
      char to[1000];
      sprintf(from, "%d:00:00:00", track + 1);
      sprintf(to,   "%d:00:00:00", track + 2);

      sendCommand("play", std::string("from ") + from + " to " + to);
    }

    void ADR_CALL stop() {
      sendCommand("stop");
      sendCommand("seek", "to start");
    }

    void ADR_CALL pause() {
      sendCommand("pause");
    }

    void ADR_CALL resume() {
      sendCommand("resume");
    }

    bool ADR_CALL isPlaying() {
      std::string status = sendCommand("status", "mode");
      return strcmp_case(status.c_str(), "playing") == 0;
    }

    bool ADR_CALL containsCD() {
      std::string status = sendCommand("status", "media present");
      return strcmp_case(status.c_str(), "true") == 0;
    }

    bool ADR_CALL isDoorOpen() {
      std::string status = sendCommand("status", "mode");
      return strcmp_case(status.c_str(), "open") == 0;
    }

    void ADR_CALL openDoor() {
      sendCommand("set", "door open");
    }

    void ADR_CALL closeDoor() {
      sendCommand("set", "door closed");
    }

  private:
    std::string m_name;
  };

  
  namespace hidden {

    ADR_EXPORT(const char*) AdrEnumerateCDDevices() {
      // Static data bad, but I can't think of a better way.  :(
      static char devices[26 * 3];  // Space for each drive letter, colons, and zeroes.
      char* out = devices;

      for (char device = 'A'; device <= 'Z'; ++device) {
        char name[] = {device, ':', 0};
        UINT type = GetDriveType(name);
        if (type == DRIVE_CDROM) {
          *out++ = device;
          *out++ = ':';
          *out++ = 0;
        }
      }

      return devices;
    }

    ADR_EXPORT(CDDevice*) AdrOpenCDDevice(const char* name) {
      if (GetDriveType(name) == DRIVE_CDROM) {
        return new CDDeviceWin32(name);
      } else {
        return 0;
      }
    }

  }

}
