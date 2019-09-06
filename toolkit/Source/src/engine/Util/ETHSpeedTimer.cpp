#include "ETHSpeedTimer.h"

ETHSpeedTimer::ETHSpeedTimer()
{
	m_elapsed = 1;
}

double ETHSpeedTimer::CalcLastFrame()
{
	m_elapsed = elapsed().user;
	start();
	return m_elapsed;
}

float ETHSpeedTimer::UnitsPerSecond(float speed) const
{
	return static_cast<float>(static_cast<double>(speed)*(m_elapsed));
}

float ETHSpeedTimer::UnitsPerSecond() const
{
	return static_cast<float>(m_elapsed);
}

double ETHSpeedTimer::GetElapsedTime() const
{
	return m_elapsed;
}
