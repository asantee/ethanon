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


typedef unsigned char u8;

int Min(int a, int b) {
  return (a < b ? a : b);
}


class ToneBuffer : public RefImplementation<SampleSource> {
public:
  ToneBuffer(double frequency) {
    SampleSourcePtr tone(CreateTone(frequency));
    tone->getFormat(m_channel_count, m_sample_rate, m_sample_format);

    m_block_size = m_channel_count * GetSampleSize(m_sample_format);

    m_position = 0;
    m_buffer_length = 88200;
    m_buffer = new u8[m_block_size * m_buffer_length];
    memset(m_buffer, 0, m_block_size * m_buffer_length);
    tone->read(m_buffer_length, m_buffer);
  }

  ~ToneBuffer() {
    delete[] m_buffer;
  }

  void ADR_CALL getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
    m_repeat = false;
  }

  int ADR_CALL read(int sample_count, void* samples) {
    int to_read = Min(sample_count, m_buffer_length - m_position);
    memcpy(
      samples,
      m_buffer + m_position * m_block_size,
      to_read * m_block_size);
    m_position += to_read;
    return to_read;
  }

  void ADR_CALL reset() {
    setPosition(0);
  }

  bool ADR_CALL isSeekable() {
    return true;
  }

  int ADR_CALL getLength() {
    return m_buffer_length;
  }

  void ADR_CALL setPosition(int position) {
    m_position = position;
  }

  int ADR_CALL getPosition() {
    return m_position;
  }

  bool ADR_CALL getRepeat() {
    return m_repeat;
  }

  void ADR_CALL setRepeat(bool repeat) {
    m_repeat = repeat;
  }

  int ADR_CALL getTagCount()            { return 0; }
  const char* ADR_CALL getTagKey(int)   { return 0; }
  const char* ADR_CALL getTagValue(int) { return 0; }
  const char* ADR_CALL getTagType(int)  { return 0; }

private:
  int m_channel_count;
  int m_sample_rate;
  SampleFormat m_sample_format;
  int m_block_size; // m_channel_count * GetSampleSize(m_sample_format)

  u8* m_buffer;
  int m_buffer_length;  // in samples
  int m_position;       // in samples

  bool m_repeat;
};


int main(int argc, char** argv) {
  std::string device_name;
  if (argc == 2) {
    device_name = argv[1];
  }

  AudioDevicePtr device(OpenDevice(device_name.c_str()));
  if (!device) {
    cerr << "Opening output device failed" << endl;
    return EXIT_FAILURE;
  }

  OutputStreamPtr s1(OpenSound(device, new ToneBuffer(256)));
  OutputStreamPtr s2(OpenSound(device, new ToneBuffer(512)));
  OutputStreamPtr s3(OpenSound(device, new ToneBuffer(513)));
  OutputStreamPtr s4(OpenSound(device, new ToneBuffer(514)));

  if (!s1 || !s2 || !s3 || !s4) {
    cerr << "OpenSound() failed" << endl;
    return EXIT_FAILURE;
  }

  s1->play();
  s2->play();
  s3->play();
  s4->play();

  // wait for five seconds
  PassOut(5);

  return EXIT_SUCCESS; // VC++ 6 sucks
}
