#include "ETHBackBufferTargetManager.h"
#include "ETHNoDynamicBackBuffer.h"

#include "../Util/ETHInput.h"

#include <Video.h>

#ifdef _MSC_VER
  #define GS2D_SSCANF sscanf_s
#else
  #define GS2D_SSCANF sscanf
#endif

ETHBackBufferTargetManagerPtr ETHBackBufferTargetManager::Create(
	gs2d::VideoPtr video,
	const ETHAppEnmlFile& file,
	const Platform::Logger& logger,
	ETHInput& ethInput)
{
	ETHBackBufferTargetManagerPtr r(new ETHBackBufferTargetManager(video, file, logger));
	video->SetScreenSizeChangeListener(r);
	ethInput.SetTargetManager(r);
	return r;
}

ETHBackBufferTargetManagerPtr ETHBackBufferTargetManager::Create(gs2d::VideoPtr video)
{
	ETHBackBufferTargetManagerPtr r(new ETHBackBufferTargetManager(video));
	video->SetScreenSizeChangeListener(r);
	return r;
}

ETHBackBufferTargetManager::ETHBackBufferTargetManager(gs2d::VideoPtr video)
{
	m_bufferSize = video->GetScreenSizeF();
	m_backBuffer = ETHDynamicBackBufferPtr(new ETHNoDynamicBackBuffer(video, m_bufferSize));
	m_targetScale = 1.0f;
	CreateOBB();
}

ETHBackBufferTargetManager::ETHBackBufferTargetManager(
	gs2d::VideoPtr video,
	const ETHAppEnmlFile& file,
	const Platform::Logger& logger)
{
	const std::string fixedWidth  = file.GetFixedWidth();
	const std::string fixedHeight = file.GetFixedHeight();
	if (!ComputeLength(video, fixedWidth, fixedHeight, true))
	{
		ComputeLength(video, fixedHeight, fixedWidth, false);
	}
	const gs2d::math::Vector2 screenSize(video->GetScreenSizeF());
	m_bufferSize.x = gs2d::math::Min(screenSize.x, m_bufferSize.x);
	m_bufferSize.y = gs2d::math::Min(screenSize.y, m_bufferSize.y);

	std::stringstream ss; ss << ("Backbuffer created as ") << m_bufferSize.x << (", ") << m_bufferSize.y
										<< (" on ");

	m_backBuffer = ETHDynamicBackBufferPtr(new ETHNoDynamicBackBuffer(video, m_bufferSize));

	m_targetScale = m_bufferSize.x / screenSize.x;

	CreateOBB();

	logger.Log(ss.str(), Platform::Logger::INFO);
}

void ETHBackBufferTargetManager::CreateOBB()
{
	m_obb = gs2d::math::OrientedBoundingBoxPtr(new gs2d::math::OrientedBoundingBox(m_bufferSize * 0.5f, m_bufferSize, 0.0f));
}

void ETHBackBufferTargetManager::ScreenSizeChanged(const gs2d::math::Vector2& newScreenSize)
{
	if (m_backBuffer->MatchesScreenSize())
	{
		m_bufferSize = newScreenSize;
		CreateOBB();
	}
}

bool ETHBackBufferTargetManager::ComputeLength(gs2d::VideoPtr video, const std::string& thisSide, const std::string& otherSide, const bool isWidth)
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
			GS2D_SSCANF(otherSide.c_str(), ("%f"), &otherValue);
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
		GS2D_SSCANF(thisSide.c_str(), ("%i"), &value);
		if (isWidth)
			m_bufferSize.x = static_cast<float>(value);
		else
			m_bufferSize.y = static_cast<float>(value);
		return false;
	}
}

bool ETHBackBufferTargetManager::IsAuto(const std::string& str)
{
	return (str == ("auto") || str.empty());
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

gs2d::math::OrientedBoundingBoxPtr ETHBackBufferTargetManager::GetOBB() const
{
	return m_obb;
}
