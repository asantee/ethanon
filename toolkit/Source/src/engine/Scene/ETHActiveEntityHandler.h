/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#ifndef ETH_TEMP_ENTITY_HANDLER_H_
#define ETH_TEMP_ENTITY_HANDLER_H_

#include "ETHBucketManager.h"

class ETHActiveEntityHandler
{
public:
	ETHActiveEntityHandler(ETHResourceProviderPtr provider);
	~ETHActiveEntityHandler();

	bool AddEntityWhenEligible(ETHRenderEntity* entity);
	bool ShouldEntityBeAlwaysActive(ETHRenderEntity* entity) const;

	void UpdateAlwaysActiveEntities(const Vector2& zAxisDir, ETHBucketManager& buckets, const unsigned long lastFrameElapsedTime);
	void UpdateCurrentFrameEntities(const Vector2& zAxisDir, ETHBucketManager& buckets, const unsigned long lastFrameElapsedTime);

	bool AddStaticCallbackWhenEligible(ETHRenderEntity* entity);
	bool IsStaticCallbackEligible(ETHRenderEntity* entity) const;

	bool IsCallbackListEmpty() const;

private:
	bool RemoveFinishedTemporaryEntity(ETHRenderEntity* entity, ETHBucketManager& buckets);

	void ClearCallbackEntities();
	void TestEntityLists() const;

	ETHResourceProviderPtr m_provider;

	/*
	 * This list will hold every temporary or dynamic entity with callbacks or physics.
	 * It must be kept separetely because these entities will be updated at every,
	 * frame even if they're culled out by the spatial hashing system.
	 */
	std::list<ETHRenderEntity*> m_dynamicOrTempEntities;

	/*
	 * This list holds every callback script function that must be called per
	 * frame. So we don't have to call them during the rendering iteration
	 */
	std::list<ETHRenderEntity*> m_lastFrameCallbacks;
};

#endif