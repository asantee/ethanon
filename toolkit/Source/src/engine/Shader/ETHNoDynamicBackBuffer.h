#ifndef ETH_NO_DYNAMIC_BACK_BUFFER_H_
#define ETH_NO_DYNAMIC_BACK_BUFFER_H_

#include "ETHDynamicBackBuffer.h"
#include <Video.h>

class ETHNoDynamicBackBuffer : public ETHDynamicBackBuffer
{
	gs2d::VideoWeakPtr m_video;

public:
	ETHNoDynamicBackBuffer(const gs2d::VideoPtr& video, const gs2d::math::Vector2& size);
	void BeginRendering();
	void EndRendering();
	void Present();
	bool MatchesScreenSize() const;
};

#endif