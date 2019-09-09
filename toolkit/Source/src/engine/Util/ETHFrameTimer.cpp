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
	start();
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
		start();
		return m_currentFrame;
	}

	if (elapsed().user > static_cast<double>(stride)/1000.0)
	{
		start();
		m_currentFrame++;
		if (m_currentFrame > lastFrame)
			m_currentFrame = firstFrame;
	}

	m_interpolator = (static_cast<float>(elapsed().user)*1000.0f)/static_cast<float>(stride);
	return m_currentFrame;
}

float ETHFrameTimer::GetInterpolationBias() const
{
	return m_interpolator;
}
