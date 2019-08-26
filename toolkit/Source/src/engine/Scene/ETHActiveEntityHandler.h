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

	void UpdateAlwaysActiveEntities(const Vector2& zAxisDir, const float sceneParallaxIntensity, ETHBucketManager& buckets, const float lastFrameElapsedTime);
	void UpdateCurrentFrameEntities(const Vector2& zAxisDir, const float sceneParallaxIntensity, ETHBucketManager& buckets, const float lastFrameElapsedTime);

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
