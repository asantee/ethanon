#include "AndroidGLES2Video.h"

namespace gs2d {

using namespace math;

AndroidGLES2Video::AndroidGLES2Video(
	const unsigned int width,
	const unsigned int height,
	const std::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	GLES2Video(width, height, winTitle, fileIOHub)
{
}

} // namespace gs2d
