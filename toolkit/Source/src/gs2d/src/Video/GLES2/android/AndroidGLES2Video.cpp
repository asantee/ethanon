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
