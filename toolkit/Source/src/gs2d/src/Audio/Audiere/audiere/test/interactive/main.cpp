// these tests aren't really interactive yet

#include <iostream>
#include <memory>
#include <string>
#include "audiere.h"
using namespace std;
using namespace audiere;


#ifdef WIN32

#include <windows.h>
void passOut(int milliseconds) {
  Sleep(milliseconds);
}

#else  // assume POSIX

#include <unistd.h>
void passOut(int milliseconds) {
  int seconds = milliseconds / 1000;
  int useconds = (milliseconds % 1000) * 1000;
  sleep(seconds);
  usleep(useconds);
}

#endif


#define arraysize(array) (sizeof(array) / sizeof(*(array)))


void testDriver(const char* driver) {
  string sound_names[] = {"kclick.wav", "knock.wav", "laugh.wav", "shot.wav"};
  const int sound_count = arraysize(sound_names);
  RefPtr<OutputStream> sounds[sound_count];

  cout << "testDriver " << driver << "\n--" << endl;

  RefPtr<AudioDevice> device(OpenDevice(driver));
  if (!device.get()) {
    cout << "Error opening output device" << endl;
    return;
  }

  for (int i = 0; i < sound_count; ++i) {
    string name = sound_names[i];
    sounds[i] = OpenSound(device.get(), name.c_str());
    if (!sounds[i]) {
      cout << "Error opening sound: " << name << endl;
      return;
    }
  }

  for (int i = 0; i < sound_count; ++i) {
    cout << "Playing " << sound_names[i] << "...";
    sounds[i]->play();
    while (sounds[i]->isPlaying()) {
      passOut(50);
    }
    cout << "Done" << endl;
  }

  cout << endl;
}


int main() {
  testDriver("");
  testDriver("null");  // default

  // VC++ 6 sucks
  return 0;
}
