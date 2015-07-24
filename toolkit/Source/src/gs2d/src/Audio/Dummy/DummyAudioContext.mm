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

#import "DummyAudioContext.h"

namespace gs2d {

GS2D_API AudioPtr CreateAudio(boost::any data)
{
	AudioPtr audio = DummyAudioContext::Create(data);
	if (audio)
	{
		return audio;
	}
	else
	{
		return AudioPtr();
	}
}

bool DummyAudioContext::CreateAudioDevice(boost::any data)
{
	return true;
}

boost::shared_ptr<DummyAudioContext> DummyAudioContext::Create(boost::any data)
{
	boost::shared_ptr<DummyAudioContext> p(new DummyAudioContext());
	p->weak_this = p;
	if (p->CreateAudioDevice(data))
	{
		return p;
	}
	else
	{
		return DummyAudioContextPtr();
	}
}

DummyAudioContext::DummyAudioContext()
{
	SetGlobalVolume(1.0f);
}

boost::any DummyAudioContext::GetAudioContext()
{
	return 0;
}

void DummyAudioContext::SetGlobalVolume(const float volume)
{
}

float DummyAudioContext::GetGlobalVolume() const
{
	return 0.0f;
}

} // namespace gs2d
