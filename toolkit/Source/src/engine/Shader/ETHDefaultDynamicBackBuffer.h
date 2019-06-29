#ifndef ETH_DEFAULT_DYNAMIC_BACK_BUFFER_H_
#define ETH_DEFAULT_DYNAMIC_BACK_BUFFER_H_

#include "ETHDynamicBackBuffer.h"
#include <Video.h>

class ETHDefaultDynamicBackBuffer : public ETHDynamicBackBuffer
{
	gs2d::SpritePtr m_target;
	gs2d::VideoWeakPtr m_video;

public:
	ETHDefaultDynamicBackBuffer(const gs2d::VideoPtr& video, const gs2d::math::Vector2& size);
	void BeginRendering();
	void EndRendering();
	void Present();
	bool MatchesScreenSize() const;
};

#endif