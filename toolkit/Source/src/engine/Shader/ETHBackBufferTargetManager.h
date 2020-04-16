#ifndef ETH_BACK_BUFFER_TARGET_MANAGER_H_
#define ETH_BACK_BUFFER_TARGET_MANAGER_H_

#include <Enml/Enml.h>
#include <Platform/Logger.h>
#include "ETHDynamicBackBuffer.h"
#include "../Platform/ETHAppEnmlFile.h"

#include <Math/OrientedBoundingBox.h>

class ETHInput;

class ETHBackBufferTargetManager : public gs2d::Application::ScreenSizeChangeListener
{
	ETHDynamicBackBufferPtr m_backBuffer;

	ETHBackBufferTargetManager(gs2d::VideoPtr video);
	ETHBackBufferTargetManager(gs2d::VideoPtr video, const ETHAppEnmlFile& file, const Platform::Logger& logger);

public:
	static boost::shared_ptr<ETHBackBufferTargetManager> Create(gs2d::VideoPtr video, const ETHAppEnmlFile& file, const Platform::Logger& logger, ETHInput& ethInput);
	static boost::shared_ptr<ETHBackBufferTargetManager> Create(gs2d::VideoPtr video);

	void ScreenSizeChanged(const gs2d::math::Vector2& newScreenSize) override;

	void BeginRendering();
	void EndRendering();
	void Present();
};

typedef boost::shared_ptr<ETHBackBufferTargetManager> ETHBackBufferTargetManagerPtr;
typedef boost::weak_ptr<ETHBackBufferTargetManager> ETHBackBufferTargetManagerWeakPtr;

#endif
