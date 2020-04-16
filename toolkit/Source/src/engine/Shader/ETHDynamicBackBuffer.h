#ifndef ETH_DYNAMIC_BACK_BUFFER_H_
#define ETH_DYNAMIC_BACK_BUFFER_H_

#include <Math/GameMath.h>
#include <boost/shared_ptr.hpp>

class ETHDynamicBackBuffer
{
public:
	virtual void BeginRendering() = 0;
	virtual void EndRendering() = 0;
	virtual void Present() = 0;
	virtual bool MatchesScreenSize() const = 0;
};

typedef boost::shared_ptr<ETHDynamicBackBuffer> ETHDynamicBackBufferPtr;

#endif
