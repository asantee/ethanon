#import <Foundation/Foundation.h>

#include "IOSGLES2Video.h"

#include "../../../Platform/getRealTime.h"

namespace gs2d {

using namespace math;

GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub)
{
	return IOSGLES2Video::Create(width, height, GS_L("GS2D"), fileIOHub);
}

IOSGLES2Video::IOSGLES2Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	GLES2Video(width, height, winTitle, fileIOHub)
{
    m_startTime = getRealTime();
}

boost::shared_ptr<GLES2Video> IOSGLES2Video::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<GLES2Video> p(new IOSGLES2Video(width, height, winTitle, fileIOHub));
	p->weak_this = p;
	return p;
}

// it will be implemented here for the boost timer is presenting strange behaviour
float IOSGLES2Video::GetElapsedTimeF(const TIME_UNITY unity) const
{
	double elapsedTimeS = getRealTime() - m_startTime;

	switch (unity)
	{
	case TU_HOURS:
		elapsedTimeS /= 60.0;
		elapsedTimeS /= 60.0;
		break;
	case TU_MINUTES:
		elapsedTimeS /= 60.0;
		break;
	case TU_MILLISECONDS:
		elapsedTimeS *= 1000.0;
	case TU_SECONDS:
	default:
		break;
	};
	return elapsedTimeS;
}

unsigned long IOSGLES2Video::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeF(unity));
}

} // namespace gs2d
