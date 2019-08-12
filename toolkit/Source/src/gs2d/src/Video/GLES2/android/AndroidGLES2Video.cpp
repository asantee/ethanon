#include "AndroidGLES2Video.h"

namespace gs2d {

using namespace math;

GS2D_API VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub)
{
	return AndroidGLES2Video::Create(width, height, GS_L("GS2D"), fileIOHub);
}

AndroidGLES2Video::AndroidGLES2Video(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	GLES2Video(width, height, winTitle, fileIOHub)
{
	clock_gettime(CLOCK_MONOTONIC_RAW, &m_startTime);
}

boost::shared_ptr<GLES2Video> AndroidGLES2Video::Create(
	const unsigned int width,
	const unsigned int height,
	const str_type::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<GLES2Video> p(new AndroidGLES2Video(width, height, winTitle, fileIOHub));
	p->weak_this = p;
	return p;
}

// the boost timer is behaving very strange on iOS, so this method will be implemented outside
float AndroidGLES2Video::GetElapsedTimeF(const TIME_UNITY unity) const
{
	return static_cast<float>(GetElapsedTimeD(unity));
}

unsigned long AndroidGLES2Video::GetElapsedTime(const TIME_UNITY unity) const
{
	return static_cast<unsigned long>(GetElapsedTimeD(unity));
}

double AndroidGLES2Video::GetElapsedTimeD(const TIME_UNITY unity) const
{
	struct timespec current;
	clock_gettime(CLOCK_MONOTONIC_RAW, &current);

	const double sec  = static_cast<double>(current.tv_sec - m_startTime.tv_sec);
	const double nsec = static_cast<double>(current.tv_nsec - m_startTime.tv_nsec) / 1000000000.0;

	double elapsedTimeS = sec + nsec;

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

} // namespace gs2d
