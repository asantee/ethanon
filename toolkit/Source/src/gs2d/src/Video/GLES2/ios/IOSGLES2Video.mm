#include "IOSGLES2Video.h"

namespace gs2d {

using namespace math;

VideoPtr CreateVideo(const unsigned int width, const unsigned int height, const Platform::FileIOHubPtr& fileIOHub)
{
	return IOSGLES2Video::Create(width, height, "GS2D", fileIOHub);
}

IOSGLES2Video::IOSGLES2Video(
	const unsigned int width,
	const unsigned int height,
	const std::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub) :
	GLES2Video(width, height, winTitle, fileIOHub)
{
}

boost::shared_ptr<GLES2Video> IOSGLES2Video::Create(
	const unsigned int width,
	const unsigned int height,
	const std::string& winTitle,
	const Platform::FileIOHubPtr& fileIOHub)
{
	boost::shared_ptr<GLES2Video> p(new IOSGLES2Video(width, height, winTitle, fileIOHub));
	p->weak_this = p;
	return p;
}

} // namespace gs2d
