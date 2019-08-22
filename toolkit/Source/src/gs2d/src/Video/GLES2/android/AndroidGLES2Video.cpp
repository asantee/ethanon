#include "AndroidGLES2Video.h"

namespace gs2d {

using namespace math;

VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub)
{
	return AndroidGLES2Video::Create(width, height, "GS2D", fileIOHub);
}

AndroidGLES2Video::AndroidGLES2Video(
	const unsigned int width,
	const unsigned int height,
	const std::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	GLES2Video(width, height, winTitle, fileIOHub)
{
}

boost::shared_ptr<GLES2Video> AndroidGLES2Video::Create(
	const unsigned int width,
	const unsigned int height,
	const std::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<GLES2Video> p(new AndroidGLES2Video(width, height, winTitle, fileIOHub));
	p->weak_this = p;
	return p;
}

} // namespace gs2d
