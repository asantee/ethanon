/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#ifndef GS2D_CD_AUDIO_CONTEXT_H_
#define GS2D_CD_AUDIO_CONTEXT_H_

#include "../../Audio.h"
#include "../../Platform/FileLogger.h"

namespace gs2d {

class CDAudioContext : public Audio
{
	bool CreateAudioDevice(boost::any data);
	Platform::FileLogger m_logger;

	boost::weak_ptr<CDAudioContext> weak_this;

	CDAudioContext();

public:
	static boost::shared_ptr<CDAudioContext> Create(boost::any data);

	static bool IsStreamable(const Audio::SAMPLE_TYPE type);

	AudioSamplePtr LoadSampleFromFile(
		const str_type::string& fileName,
		const Platform::FileManagerPtr& fileManager,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE);

	AudioSamplePtr LoadSampleFromFileInMemory(
		void *pBuffer,
		const unsigned int bufferLength,
		const Audio::SAMPLE_TYPE type = Audio::UNKNOWN_TYPE);

	boost::any GetAudioContext();

	void SetGlobalVolume(const float volume);
	float GetGlobalVolume() const;
    
    void Update();
};

typedef boost::shared_ptr<CDAudioContext> CDAudioContextPtr;
typedef boost::shared_ptr<CDAudioContext> CDAudioContextWeakPtr;

} // namespace gs2d

#endif
