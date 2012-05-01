#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "audiere.h"
using namespace std;
using namespace audiere;


int main(int argc, const char** argv) {
  const char* device_name = "";
  if (argc >= 2) {
    device_name = argv[1];
  }

  AudioDevicePtr device(OpenDevice(device_name));
  if (!device) {
    cerr << "OpenDevice() failed" << endl;
    return EXIT_FAILURE;
  }

  OutputStreamPtr tone = device->openStream(CreateTone(440));
  if (!tone) {
    cerr << "openStream() failed" << endl;
    return EXIT_FAILURE;
  }
  tone->play();

  clock_t start = clock();
  for (int i = 0; i < 100000; ++i) {
    assert(tone->isPlaying());
  }
  clock_t end = clock();

  double duration = double(end - start) / CLOCKS_PER_SEC;
  cout << "Total time for 100,000 isPlaying() calls = "
       << duration << " seconds" << endl;

  cout << "That is " << duration / 100000 << " seconds per call." << endl;

  return EXIT_SUCCESS;
}
