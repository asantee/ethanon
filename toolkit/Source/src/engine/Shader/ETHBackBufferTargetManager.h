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
	gs2d::math::Vector2 m_bufferSize;
	float m_targetScale;

	static bool IsAuto(const std::string& str);
	bool ComputeLength(gs2d::VideoPtr video, const std::string& thisSide, const std::string& otherSide, const bool isWidth);

	ETHDynamicBackBufferPtr m_backBuffer;
	gs2d::math::OrientedBoundingBoxPtr m_obb;

	ETHBackBufferTargetManager(gs2d::VideoPtr video);
	ETHBackBufferTargetManager(gs2d::VideoPtr video, const ETHAppEnmlFile& file, const Platform::Logger& logger);

	void ScreenSizeChanged(const gs2d::math::Vector2& newScreenSize);
	void CreateOBB();

public:
	static boost::shared_ptr<ETHBackBufferTargetManager> Create(gs2d::VideoPtr video, const ETHAppEnmlFile& file, const Platform::Logger& logger, ETHInput& ethInput);
	static boost::shared_ptr<ETHBackBufferTargetManager> Create(gs2d::VideoPtr video);

	gs2d::math::Vector2 GetBufferSize() const;
	void BeginRendering();
	void EndRendering();
	void Present();
	float GetTargetScale() const;
	gs2d::math::OrientedBoundingBoxPtr GetOBB() const;
};

typedef boost::shared_ptr<ETHBackBufferTargetManager> ETHBackBufferTargetManagerPtr;
typedef boost::weak_ptr<ETHBackBufferTargetManager> ETHBackBufferTargetManagerWeakPtr;

#endif
