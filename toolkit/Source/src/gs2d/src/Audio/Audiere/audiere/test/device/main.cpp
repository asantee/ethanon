#include <iostream>
#include <memory>
#include <string>
#include <stdlib.h>
#include "audiere.h"
using namespace std;
using namespace audiere;


#ifdef WIN32

  #include <windows.h>
  void PassOut(int seconds) {
    Sleep(seconds * 1000);
  }

#else

  #include <unistd.h>
  void PassOut(int seconds) {
    sleep(seconds);
  }

#endif


int main(int argc, char** argv) {
  std::string device_name;
  if (argc == 2) {
    device_name = argv[1];
  }

  AudioDevicePtr device(OpenDevice(device_name.c_str()));
  if (!device.get()) {
    cerr << "Opening output device failed" << endl;
    return EXIT_FAILURE;
  }

  OutputStreamPtr stream1(device->openStream(CreateTone(256)));
  OutputStreamPtr stream2(device->openStream(CreateTone(512)));
  OutputStreamPtr stream3(device->openStream(CreateTone(515)));

  if (!stream1 || !stream2 || !stream3) {
    cerr << "openStream() failed" << endl;
    return EXIT_FAILURE;
  }

  stream1->play();
  stream2->play();
  stream3->play();

  // wait for five seconds
  PassOut(5);

  return EXIT_SUCCESS; // VC++ 6 sucks
}
