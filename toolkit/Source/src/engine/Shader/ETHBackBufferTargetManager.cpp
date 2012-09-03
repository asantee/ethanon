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

#include "ETHBackBufferTargetManager.h"
#include "ETHDefaultDynamicBackBuffer.h"
#include "ETHNoDynamicBackBuffer.h"

ETHBackBufferTargetManager::ETHBackBufferTargetManager(gs2d::VideoPtr video, const ETHAppEnmlFile& file, const Platform::Logger& logger)
{
	const gs2d::str_type::string fixedWidth  = file.GetFixedWidth();
	const gs2d::str_type::string fixedHeight = file.GetFixedHeight();
	if (!ComputeLength(video, fixedWidth, fixedHeight, true))
	{
		ComputeLength(video, fixedHeight, fixedWidth, false);
	}
	const gs2d::math::Vector2 screenSize(video->GetScreenSizeF());
	m_bufferSize.x = gs2d::math::Min(screenSize.x, m_bufferSize.x);
	m_bufferSize.y = gs2d::math::Min(screenSize.y, m_bufferSize.y);

	m_backBuffer = (m_bufferSize == screenSize) ? 
					ETHDynamicBackBufferPtr(new ETHNoDynamicBackBuffer(video, m_bufferSize)) :
					ETHDynamicBackBufferPtr(new ETHDefaultDynamicBackBuffer(video, m_bufferSize));
	m_targetScale = m_bufferSize.x / screenSize.x;

	gs2d::str_type::stringstream ss; ss << GS_L("Backbuffer created as ") << m_bufferSize.x << GS_L(", ") << m_bufferSize.y;
	logger.Log(ss.str(), Platform::Logger::INFO);
}

bool ETHBackBufferTargetManager::ComputeLength(gs2d::VideoPtr video, const gs2d::str_type::string& thisSide, const gs2d::str_type::string& otherSide, const bool isWidth)
{
	if (IsAuto(thisSide))
	{
		const gs2d::math::Vector2 screenSize(video->GetScreenSizeF());
		if (IsAuto(otherSide))
		{
			m_bufferSize = screenSize;
			return true;
		}
		else
		{
			float otherValue = -1.0f;
			ENML_SSCANF(otherSide.c_str(), GS_L("%f"), &otherValue);
			if (isWidth)
				m_bufferSize.x = ceilf(otherValue * (screenSize.x / screenSize.y));
			else
				m_bufferSize.y = ceilf(otherValue * (screenSize.y / screenSize.x));
			return false;
		}
	}
	else
	{
		int value = -1;
		ENML_SSCANF(thisSide.c_str(), GS_L("%i"), &value);
		if (isWidth)
			m_bufferSize.x = static_cast<float>(value);
		else
			m_bufferSize.y = static_cast<float>(value);
		return false;
	}
}

bool ETHBackBufferTargetManager::IsAuto(const gs2d::str_type::string& str)
{
	return (str == GS_L("auto") || str.empty());
}

gs2d::math::Vector2 ETHBackBufferTargetManager::GetBufferSize() const
{
	return m_bufferSize;
}

void ETHBackBufferTargetManager::BeginRendering()
{
	m_backBuffer->BeginRendering();
}

void ETHBackBufferTargetManager::EndRendering()
{
	m_backBuffer->EndRendering();
}

void ETHBackBufferTargetManager::Present()
{
	m_backBuffer->Present();
}

float ETHBackBufferTargetManager::GetTargetScale() const
{
	return m_targetScale;
}
