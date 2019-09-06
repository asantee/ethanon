#ifndef ETH_FRAME_TIMER_H_
#define ETH_FRAME_TIMER_H_

#include <boost/timer/timer.hpp>

class ETHFrameTimer : private boost::timer::cpu_timer
{
public:
	ETHFrameTimer();
	void Reset();
	unsigned int Get();
	unsigned int Update(const unsigned int firstFrame, const unsigned int lastFrame, const unsigned int stride);
	float GetInterpolationBias() const;

private:
	unsigned int m_currentFirstFrame;
	unsigned int m_currentLastFrame;
	unsigned int m_currentFrame;
	float m_interpolator;
};

#endif
