#include "ETHNoDynamicBackBuffer.h"

ETHNoDynamicBackBuffer::ETHNoDynamicBackBuffer(const gs2d::VideoPtr& video, const gs2d::math::Vector2& size) :
	m_video(video)
{
	GS2D_UNUSED_ARGUMENT(size);
}

void ETHNoDynamicBackBuffer::BeginRendering()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
		video->BeginSpriteScene();
}

void ETHNoDynamicBackBuffer::EndRendering()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
		video->EndSpriteScene();
}

void ETHNoDynamicBackBuffer::Present()
{
}

bool ETHNoDynamicBackBuffer::MatchesScreenSize() const
{
	return true;
}
