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

#ifndef ETH_BACK_BUFFER_TARGET_MANAGER_H_
#define ETH_BACK_BUFFER_TARGET_MANAGER_H_

#include <enml/enml.h>
#include <gs2d.h>
#include <Platform/Logger.h>
#include "ETHDynamicBackBuffer.h"
#include "../Platform/ETHAppEnmlFile.h"

class ETHInput;

class ETHBackBufferTargetManager : public gs2d::Application::ScreenSizeChangeListener
{
	gs2d::math::Vector2 m_bufferSize;
	float m_targetScale;

	static bool IsAuto(const gs2d::str_type::string& str);
	bool ComputeLength(gs2d::VideoPtr video, const gs2d::str_type::string& thisSide, const gs2d::str_type::string& otherSide, const bool isWidth);

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
