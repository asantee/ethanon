#include "ETHDrawableManager.h"

void ETHDrawableManager::DrawTopLayer(const unsigned long lastFrameElapsedTimeMS, const VideoPtr& video)
{
	const Vector2 oldCamPos = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));
	video->SetZBuffer(false);
	video->SetSpriteDepth(0.0f);
	for (std::list<boost::shared_ptr<ETHDrawable> >::iterator iter = m_drawableList.begin();
		iter != m_drawableList.end(); ++iter)
	{
		const boost::shared_ptr<ETHDrawable>& drawable = (*iter);
		drawable->Draw(lastFrameElapsedTimeMS);
	}
	video->SetCameraPos(oldCamPos);
}

void ETHDrawableManager::RemoveTheDead()
{
	for (std::list<boost::shared_ptr<ETHDrawable> >::iterator iter = m_drawableList.begin();
		iter != m_drawableList.end();)
	{
		const boost::shared_ptr<ETHDrawable>& drawable = (*iter);
		if (drawable->IsAlive())
		{
			++iter;
		}
		else
		{
			iter = m_drawableList.erase(iter);
		}
	}
}

void ETHDrawableManager::Insert(const boost::shared_ptr<ETHDrawable>& newItem)
{
	// Insert new element before other persistent elements (e.g.: fading texts and particle systems)
	std::list<boost::shared_ptr<ETHDrawable> >::reverse_iterator last = m_drawableList.rbegin();
	if (last != m_drawableList.rend())
	{
		if ((*last)->IsAlive())
		{
			for (std::list<boost::shared_ptr<ETHDrawable> >::iterator iter = m_drawableList.begin();
				iter != m_drawableList.end(); ++iter)
			{
				const boost::shared_ptr<ETHDrawable>& drawable = (*iter);
				if (drawable->IsAlive())
				{
					m_drawableList.insert(iter, newItem);
					return;
				}
			}
		}
	}

	// if nothing has been added up there...
	m_drawableList.push_back(newItem);
}

void ETHDrawableManager::Clear()
{
	m_drawableList.clear();
}
