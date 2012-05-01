#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <audiere.h>
using namespace std;
using namespace audiere;


#ifdef WIN32

#include <windows.h>
inline void PassOut(int seconds) {
  Sleep(1000 * seconds);
}

#else  // assume POSIX

#include <unistd.h>
inline void PassOut(int seconds) {
  sleep(seconds);
}

#endif


AudioDevicePtr  g_device;
OutputStreamPtr g_sound;
OutputStreamPtr g_stream;
StopCallbackPtr g_callback;


class MyStopCallback : public RefImplementation<StopCallback> {
public:
  void ADR_CALL streamStopped(StopEvent* event) {
    cout << "Stopped!  Reason: ";
    if (event->getReason() == StopEvent::STOP_CALLED) {
      cout << "Stop Called";
    } else if (event->getReason() == StopEvent::STREAM_ENDED) {
      cout << "Stream Ended";
    } else {
      cout << "Unknown";
    }
    cout << endl;
    if (event->getOutputStream() == g_sound) {
      cout << "Deleting sound" << endl;
      g_sound = 0;
    } else if (event->getOutputStream() == g_stream) {
      cout << "Deleting stream" << endl;
      g_stream = 0;
    }
  }
};


bool loadStreams(bool loop = false) {
  g_sound  = OpenSound(g_device, "data/laugh.wav", false);
  g_stream = OpenSound(g_device, "data/laugh.wav", true);
  g_sound ->setRepeat(loop);
  g_stream->setRepeat(loop);

  if (!g_sound || !g_stream) {
    cerr << "Opening data/laugh.wav failed" << endl;
    return false;
  }
  return true;
}


int main(int argc, const char** argv) {
  if (argc != 1 && argc != 2) {
    cerr << "usage: callback [<device>]" << endl;
    return EXIT_FAILURE;
  }

  const char* device_name = "";
  if (argc == 2) {
    device_name = argv[1];
  }

  g_device = OpenDevice(device_name);
  if (!g_device) {
    cerr << "OpenDevice() failed" << endl;
    return EXIT_FAILURE;
  }

  g_callback = new MyStopCallback;
  g_device->registerCallback(g_callback.get());

#define LOAD_STREAMS(x) if (!loadStreams x) { return EXIT_FAILURE; }

  LOAD_STREAMS(())

  g_sound->play();
  g_stream->play();

  PassOut(3);
  LOAD_STREAMS(())

  g_sound->play();
  g_stream->play();
  g_sound->stop();
  g_stream->stop();

  PassOut(3);
  LOAD_STREAMS((true))

  g_sound->play();
  g_stream->play();

  PassOut(3);
  LOAD_STREAMS(())

  g_sound->play();
  g_sound->reset();
  g_stream->play();
  g_stream->reset();

  return EXIT_SUCCESS;
}
