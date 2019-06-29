#ifndef ETH_DRAWABLE_MANAGER_H_
#define ETH_DRAWABLE_MANAGER_H_

#include "ETHDrawable.h"
#include "../Scene/ETHScene.h"

class ETHDrawableManager
{
	std::list<boost::shared_ptr<ETHDrawable> > m_drawableList;

public:
	void DrawTopLayer(const unsigned long lastFrameElapsedTimeMS, const VideoPtr& video);
	void Insert(const boost::shared_ptr<ETHDrawable>& newItem);
	void Clear();
	void RemoveTheDead();
};

#endif
