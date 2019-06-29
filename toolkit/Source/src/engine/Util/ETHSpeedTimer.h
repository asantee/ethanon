#ifndef ETH_SPEED_TIMER_H_
#define ETH_SPEED_TIMER_H_

#include <boost/timer.hpp>

class ETHSpeedTimer : public boost::timer
{
public:
	ETHSpeedTimer();
	double CalcLastFrame();
	float UnitsPerSecond(float speed) const;
	float UnitsPerSecond() const ;
	double GetElapsedTime() const;
private:
	double m_elapsed;
};

#endif
