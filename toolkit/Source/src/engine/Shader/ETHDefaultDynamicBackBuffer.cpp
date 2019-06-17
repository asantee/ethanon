#include "ETHDefaultDynamicBackBuffer.h"

ETHDefaultDynamicBackBuffer::ETHDefaultDynamicBackBuffer(const gs2d::VideoPtr& video, const gs2d::math::Vector2& size) :
	m_video(video)
{
	m_target = video->CreateRenderTarget(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y), gs2d::Texture::TF_DEFAULT);
}

void ETHDefaultDynamicBackBuffer::BeginRendering()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
	{
		video->SetRenderTarget(m_target);
		video->SetAlphaMode(gs2d::Video::AM_PIXEL);
		video->BeginTargetScene(gs2d::constant::ZERO, true);
		video->SetCurrentShader(gs2d::ShaderPtr());
	}
}

void ETHDefaultDynamicBackBuffer::EndRendering()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
	{
		video->EndTargetScene();
		video->SetRenderTarget(gs2d::SpritePtr());
	}
}

void ETHDefaultDynamicBackBuffer::Present()
{
	gs2d::VideoPtr video = m_video.lock();
	if (video)
	{
		video->SetCurrentShader(gs2d::ShaderPtr());
		video->BeginSpriteScene();

		const gs2d::Video::ALPHA_MODE alpha = video->GetAlphaMode();
		video->SetAlphaMode(gs2d::Video::AM_NONE);
		
		m_target->DrawShaped(
			video->GetCameraPos(),
			video->GetScreenSizeF(),
			gs2d::math::constant::ONE_VECTOR4,
			gs2d::math::constant::ONE_VECTOR4,
			gs2d::math::constant::ONE_VECTOR4,
			gs2d::math::constant::ONE_VECTOR4,
			0.0f);

		video->SetAlphaMode(alpha);

		video->EndSpriteScene();
	}
}

bool ETHDefaultDynamicBackBuffer::MatchesScreenSize() const
{
	return false;
}
