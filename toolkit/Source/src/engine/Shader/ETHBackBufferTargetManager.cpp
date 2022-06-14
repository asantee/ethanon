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
	m_backBuffer = ETHDynamicBackBufferPtr(new ETHNoDynamicBackBuffer(video, video->GetScreenSizeInPixels()));
}

ETHBackBufferTargetManager::ETHBackBufferTargetManager(
	gs2d::VideoPtr video,
	const ETHAppEnmlFile& file,
	const Platform::Logger& logger)
{
	const gs2d::math::Vector2 screenSize(video->GetScreenSizeInPixels());
	std::stringstream ss; ss << ("Actual size is ") << screenSize.x << (", ") << screenSize.y;

	m_backBuffer = ETHDynamicBackBufferPtr(new ETHNoDynamicBackBuffer(video, screenSize));

	logger.Log(ss.str(), Platform::Logger::LT_INFO);
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

void ETHBackBufferTargetManager::ScreenSizeChanged(const math::Vector2& newScreenSize)
{
}
