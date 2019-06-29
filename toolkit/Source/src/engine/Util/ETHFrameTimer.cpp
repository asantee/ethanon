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
