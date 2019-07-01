#ifndef GS2D_IOS_GLES2_H_
#define GS2D_IOS_GLES2_H_

#include "../GLES2Video.h"

namespace gs2d {

class IOSGLES2Video : public GLES2Video
{
	double m_startTime;

public:
	IOSGLES2Video(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	static boost::shared_ptr<GLES2Video> Create(
		const unsigned int width,
		const unsigned int height,
		const str_type::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	float GetElapsedTimeF(const TIME_UNITY unity) const;
	unsigned long GetElapsedTime(const TIME_UNITY unity) const;
};

} // namespace gs2d

#endif
