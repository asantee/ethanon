/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use, copy,
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so, subject to the
	following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "ETHDefaultDynamicBackBuffer.h"

ETHDefaultDynamicBackBuffer::ETHDefaultDynamicBackBuffer(const gs2d::VideoPtr& video, const gs2d::math::Vector2& size) :
	m_video(video)
{
	m_target = video->CreateRenderTarget(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y), gs2d::GSTF_DEFAULT);
}

void ETHDefaultDynamicBackBuffer::BeginRendering()
{
	m_video->SetRenderTarget(m_target);
	m_video->BeginTargetScene(gs2d::GS_ZERO, true);
}

void ETHDefaultDynamicBackBuffer::EndRendering()
{
	m_video->EndTargetScene();
	m_video->SetRenderTarget(gs2d::SpritePtr());
}

void ETHDefaultDynamicBackBuffer::Present()
{
	m_video->BeginSpriteScene();
	m_target->DrawShaped(m_video->GetCameraPos(), m_video->GetScreenSizeF(), 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f);
	m_video->EndSpriteScene();
}

bool ETHDefaultDynamicBackBuffer::MatchesScreenSize() const
{
	return false;
}
