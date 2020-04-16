#include "ETHNoDynamicBackBuffer.h"

#define UNUSED_ARGUMENT(argument) ((void)(argument))

ETHNoDynamicBackBuffer::ETHNoDynamicBackBuffer(const gs2d::VideoPtr& video, const gs2d::math::Vector2& size) :
	m_video(video)
{
	UNUSED_ARGUMENT(size);
}

void ETHNoDynamicBackBuffer::BeginRendering()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
		video->BeginRendering();
}

void ETHNoDynamicBackBuffer::EndRendering()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
		video->EndRendering();
}

void ETHNoDynamicBackBuffer::Present()
{
}

bool ETHNoDynamicBackBuffer::MatchesScreenSize() const
{
	return true;
}
