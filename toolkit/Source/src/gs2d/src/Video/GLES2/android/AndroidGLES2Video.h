#ifndef GS2D_ANDROID_GLES2_H_
#define GS2D_ANDROID_GLES2_H_

#include "../GLES2Video.h"

namespace gs2d {

class AndroidGLES2Video : public GLES2Video
{
public:
	AndroidGLES2Video(
		const unsigned int width,
		const unsigned int height,
		const std::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);
};

} // namespace gs2d

#endif
