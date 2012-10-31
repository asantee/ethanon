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

#include "IrrKlangAudio.h"

namespace gs2d {

GS2D_API AudioPtr CreateAudio(boost::any data)
{
	IrrKlangAudioPtr audio = IrrKlangAudio::Create(data);
	if (audio)
	{
		return audio;
	}
	return AudioPtr();
}

boost::shared_ptr<IrrKlangAudio> IrrKlangAudio::Create(boost::any data)
{
	boost::shared_ptr<IrrKlangAudio> p(new IrrKlangAudio());
	p->weak_this = p;
	if (p->CreateAudioDevice(data))
	{
		return p;
	}
	else
	{
		return IrrKlangAudioPtr();
	}
}

IrrKlangAudio::IrrKlangAudio() :
	m_engine(0),
	m_globalVolume(1.0f)
{
}

IrrKlangAudio::~IrrKlangAudio()
{
	if (m_engine)
	{
		m_engine->drop();
		m_engine = 0;
	}
}

bool IrrKlangAudio::CreateAudioDevice(boost::any data)
{
	m_engine = irrklang::createIrrKlangDevice();
	return (m_engine);
}

boost::any IrrKlangAudio::GetAudioContext()
{
	return m_engine;
}

void IrrKlangAudio::SetGlobalVolume(const float volume)
{
	m_globalVolume = volume;
}

float IrrKlangAudio::GetGlobalVolume() const
{
	return m_globalVolume;
}

AudioSamplePtr IrrKlangAudio::LoadSampleFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager, const GS_SAMPLE_TYPE type)
{
	#warning todo
	return AudioSamplePtr();
}

AudioSamplePtr IrrKlangAudio::LoadSampleFromFileInMemory(void *pBuffer, const unsigned int bufferLength, const GS_SAMPLE_TYPE type)
{
	#warning todo
	return AudioSamplePtr();
}

} // namespace gs2d
