#ifndef GS2D_ANDROID_GLES2_H_
#define GS2D_ANDROID_GLES2_H_

#include "../GLES2Video.h"

namespace gs2d {

// TODO remove this class
class AndroidGLES2Video : public GLES2Video
{
public:
	AndroidGLES2Video(
		const unsigned int width,
		const unsigned int height,
		const std::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);

	static boost::shared_ptr<GLES2Video> Create(
		const unsigned int width,
		const unsigned int height,
		const std::string& winTitle,
		const Platform::FileIOHubPtr& fileIOHub);
};

} // namespace gs2d

#endif
