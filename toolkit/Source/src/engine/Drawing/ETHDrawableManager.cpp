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

#include "ETHDrawableManager.h"

void ETHDrawableManager::DrawTopLayer(const unsigned int lastFrameElapsedTimeMS, const ETHScenePtr& scene, const VideoPtr& video)
{
	const Vector2 oldCamPos = video->GetCameraPos();
	video->SetCameraPos(Vector2(0,0));
	video->SetZBuffer(scene->GetZBuffer());
	video->SetZWrite(scene->GetZBuffer());
	video->SetSpriteDepth(0.0f);
	for (std::list<boost::shared_ptr<ETHDrawable> >::iterator iter = m_drawableList.begin();
		iter != m_drawableList.end();)
	{
		const boost::shared_ptr<ETHDrawable>& drawable = (*iter);
		drawable->Draw(lastFrameElapsedTimeMS);
		if (drawable->IsAlive())
		{
			++iter;
		}
		else
		{
			iter = m_drawableList.erase(iter);
		}
	}
	video->SetCameraPos(oldCamPos);
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
					if (iter != m_drawableList.begin())
						m_drawableList.insert(--iter, newItem);
					else
						m_drawableList.push_front(newItem);
					return;
				}
			}
		}
	}

	// if nothing happened up there...
	m_drawableList.push_back(newItem);
}

void ETHDrawableManager::Clear()
{
	m_drawableList.clear();
}
