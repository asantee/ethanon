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

#include "ETHFrameTimer.h"

ETHFrameTimer::ETHFrameTimer()
{
	Reset();
}

void ETHFrameTimer::Reset()
{
	m_interpolator = 0.0f;
	m_currentFrame = 0;
	m_currentFirstFrame = m_currentLastFrame = 0;
	restart();
}

unsigned int ETHFrameTimer::Get()
{
	return m_currentFrame;
}

unsigned int ETHFrameTimer::Update(const unsigned int firstFrame, const unsigned int lastFrame, const unsigned int stride)
{
	if (firstFrame != m_currentFirstFrame || lastFrame != m_currentLastFrame)
	{
		m_currentFrame = m_currentFirstFrame = firstFrame;
		m_currentLastFrame = lastFrame;
		restart();
		return m_currentFrame;
	}

	if (elapsed() > static_cast<double>(stride)/1000.0)
	{
		restart();
		m_currentFrame++;
		if (m_currentFrame > lastFrame)
			m_currentFrame = firstFrame;
	}

	m_interpolator = (static_cast<float>(elapsed())*1000.0f)/static_cast<float>(stride);
	return m_currentFrame;
}

float ETHFrameTimer::GetInterpolationBias() const
{
	return m_interpolator;
}
