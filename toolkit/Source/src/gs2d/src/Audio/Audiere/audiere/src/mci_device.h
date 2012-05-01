#ifndef MCI_DEVICE_H
#define MCI_DEVICE_H


#include <windows.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
#include "debug.h"


namespace audiere {

  /**
   * Provides a nice interface to an MCI device name.  Closes the
   * device on destruction.
   */
  class MCIDevice {
  public:
    MCIDevice(const std::string& device) {
      m_device = device;

      const char* windowClassName = "AudiereMCINotification";

      // Register window class for MCI notifications.  Don't worry about
      // failure.  If this fails, the window creation will fail.
      WNDCLASS wc;
      wc.style          = 0;
      wc.lpfnWndProc    = notifyWindowProc;
      wc.cbClsExtra     = 0;
      wc.cbWndExtra     = 0;
      wc.hInstance      = GetModuleHandle(NULL);
      wc.hIcon          = NULL;
      wc.hCursor        = NULL;
      wc.hbrBackground  = NULL;
      wc.lpszMenuName   = NULL;
      wc.lpszClassName  = windowClassName;
      RegisterClass(&wc);

      m_window = CreateWindow(
        windowClassName, "",
        WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, GetModuleHandle(NULL), NULL);
      if (m_window) {
        SetWindowLong(m_window, GWL_USERDATA, reinterpret_cast<LONG>(this));
      } else {
        ADR_LOG("MCI notification window creation failed");
      }
    }

    ~MCIDevice() {
      sendCommand("close");

      if (m_window) {
        DestroyWindow(m_window);
      }
    }

  protected:
    std::string sendCommand(const std::string& request, const std::string& parameters = "", int flags = 0) {
      bool notify = (flags & MCI_NOTIFY) != 0;
      bool wait   = (flags & MCI_WAIT)   != 0;
      return sendString(
        request + " " + m_device + " " + parameters
                + (wait   ? " wait" : "")
                + (notify ? " notify" : ""),
        0,
        (notify ? m_window : NULL));
    }

    static std::string sendString(const std::string& string, bool* error = 0, HWND window = NULL) {
      ADR_LOG("Sending MCI Command: " + string);

      const int bufferLength = 1000;
      char buffer[bufferLength + 1] = {0};
      MCIERROR e = mciSendString(string.c_str(), buffer, 1000, window);
      if (error) {
        *error = (e != 0);
      }

      char errorString[bufferLength + 1] = {0};
      mciGetErrorString(e, errorString, bufferLength);

      if (e) {
        ADR_LOG("Error: " + std::string(errorString));
      }
      if (buffer[0]) {
        ADR_LOG("Result: " + std::string(buffer));
      }
      return buffer;
    }

    /// Called when a 'notify' message is sent from MCI.
    virtual void notify(WPARAM flags) { }

  private:
    static LRESULT CALLBACK notifyWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam) {
      switch (msg) {
        case MM_MCINOTIFY: {
          MCIDevice* This = reinterpret_cast<MCIDevice*>(GetWindowLong(window, GWL_USERDATA));
          if (This) {
            This->notify(wparam);
          }
          return 0;
        }

        default:
          return DefWindowProc(window, msg, wparam, lparam);
      }
    }

    std::string m_device;
    
    HWND m_window;  ///< For notifications (the 'notify' MCI flag).
  };

}


#endif
