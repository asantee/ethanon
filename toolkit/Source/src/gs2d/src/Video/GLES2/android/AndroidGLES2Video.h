#ifndef GS2D_ANDROID_GLES2_H_
#define GS2D_ANDROID_GLES2_H_

#include "../GLES2Video.h"

#include <time.h>
#include <sys/time.h>

namespace gs2d {

class AndroidGLES2Video : public GLES2Video
{
	timespec m_startTime;

public:
	AndroidGLES2Video(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	static boost::shared_ptr<GLES2Video> Create(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	unsigned long GetElapsedTime(const TIME_UNITY unity = TU_MILLISECONDS) const;
	float GetElapsedTimeF(const TIME_UNITY unity = TU_MILLISECONDS) const;
	double GetElapsedTimeD(const TIME_UNITY unity = TU_MILLISECONDS) const;
};

} // namespace gs2d

#endif
