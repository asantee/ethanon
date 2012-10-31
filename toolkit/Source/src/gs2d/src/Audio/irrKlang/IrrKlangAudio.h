/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this
 software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so, subject to the
 following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 --------------------------------------------------------------------------------------*/

#ifndef GS2D_IRRKLANG_AUDIO_H_
#define GS2D_IRRKLANG_AUDIO_H_

#include "../../Audio.h"

#include <irrKlang.h>

namespace gs2d {

class IrrKlangAudio : public Audio
{
	irrklang::ISoundEngine* m_engine;
	bool CreateAudioDevice(boost::any data);
	
	boost::weak_ptr<IrrKlangAudio> weak_this;

public:
	static boost::shared_ptr<IrrKlangAudio> Create(boost::any data);

	IrrKlangAudio();
	~IrrKlangAudio();
	AudioSamplePtr LoadSampleFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type = GSST_UNKNOWN);
	AudioSamplePtr LoadSampleFromFileInMemory(void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type = GSST_UNKNOWN);
	boost::any GetAudioContext();
	void SetGlobalVolume(const float volume);
	float GetGlobalVolume() const;
};

typedef boost::shared_ptr<IrrKlangAudio> IrrKlangAudioPtr;

}

#endif
