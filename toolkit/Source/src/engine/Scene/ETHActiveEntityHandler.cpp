#include "ETHActiveEntityHandler.h"

#include "../Entity/ETHRenderEntity.h"

ETHActiveEntityHandler::ETHActiveEntityHandler(ETHResourceProviderPtr provider) :
	m_provider(provider)
{
}

ETHActiveEntityHandler::~ETHActiveEntityHandler()
{
	for (ETHEntityList::iterator iter = m_dynamicOrTempEntities.begin(); iter != m_dynamicOrTempEntities.end(); ++iter)
	{
		(*iter)->Release();
	}
	ClearCallbackEntities();
}

bool ETHActiveEntityHandler::IsCallbackListEmpty() const
{
	return m_lastFrameCallbacks.empty();
}

void ETHActiveEntityHandler::ClearCallbackEntities()
{
	for (std::list<ETHRenderEntity*>::iterator iter = m_lastFrameCallbacks.begin();
		iter != m_lastFrameCallbacks.end(); ++iter)
	{
		(*iter)->Release();
	}
	m_lastFrameCallbacks.clear();
}

bool ETHActiveEntityHandler::AddEntityWhenEligible(ETHRenderEntity* entity)
{
	if (ShouldEntityBeAlwaysActive(entity))
	{
		entity->AddRef();
		m_dynamicOrTempEntities.push_back(entity);
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHActiveEntityHandler::ShouldEntityBeAlwaysActive(ETHRenderEntity* entity) const
{
	return (((entity->HasAnyCallbackFunction() || entity->HasSimulatedBody()) && !entity->IsStatic())
			|| entity->IsTemporary());
}

bool ETHActiveEntityHandler::AddStaticCallbackWhenEligible(ETHRenderEntity* entity)
{
	if (IsStaticCallbackEligible(entity))
	{
		m_lastFrameCallbacks.push_back(entity);
		entity->AddRef();
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHActiveEntityHandler::IsStaticCallbackEligible(ETHRenderEntity* entity) const
{
	return (!ShouldEntityBeAlwaysActive(entity));
}

void ETHActiveEntityHandler::TestEntityLists() const
{
	for (std::list<ETHRenderEntity*>::const_iterator a = m_dynamicOrTempEntities.begin(); a != m_dynamicOrTempEntities.end(); ++a)
	{
		for (std::list<ETHRenderEntity*>::const_iterator b = m_lastFrameCallbacks.begin(); b != m_lastFrameCallbacks.end(); ++b)
		{
			ETHRenderEntity* entityA = (*a);
			ETHRenderEntity* entityB = (*b);
			if (entityA == entityB)
			{
				ETH_STREAM_DECL(ss) << ("Equal entities found on both lists: ") << entityA->GetEntityName();
				m_provider->Log(ss.str(), Platform::Logger::WARNING);
			}
			assert(entityA != entityB);
		}
	}
}

void ETHActiveEntityHandler::UpdateAlwaysActiveEntities(const Vector2& zAxisDir, const float sceneParallaxIntensity, ETHBucketManager& buckets, const float lastFrameElapsedTime)
{
	#if defined(_DEBUG) || defined(DEBUG)
	TestEntityLists();
	#endif

	for (std::list<ETHRenderEntity*>::iterator iter = m_dynamicOrTempEntities.begin(); iter != m_dynamicOrTempEntities.end();)
	{
		ETHRenderEntity* entity = (*iter);

		if (!(entity->IsAlive()) || RemoveFinishedTemporaryEntity(entity, buckets))
		{
			#if defined(_DEBUG) || defined(DEBUG)
			 ETH_STREAM_DECL(ss) << ("Entity removed from dynamic entity list: ") << entity->GetEntityName();
			 m_provider->Log(ss.str(), Platform::Logger::INFO);
			#endif
			entity->Release();
			iter = m_dynamicOrTempEntities.erase(iter);
			continue;
		}

		entity->Update(lastFrameElapsedTime, zAxisDir, sceneParallaxIntensity, buckets);

		if (entity->HasAnyCallbackFunction())
		{
			entity->RunCallbackScript();
		}

		++iter;
	}
}

void ETHActiveEntityHandler::UpdateCurrentFrameEntities(const Vector2& zAxisDir, const float sceneParallaxIntensity, ETHBucketManager& buckets, const float lastFrameElapsedTime)
{
	#if defined(_DEBUG) || defined(DEBUG)
	TestEntityLists();
	#endif

	for (std::list<ETHRenderEntity*>::iterator iter = m_lastFrameCallbacks.begin(); iter != m_lastFrameCallbacks.end();)
	{
		ETHRenderEntity* entity = (*iter);

		if (entity->IsAlive())
		{
			entity->Update(lastFrameElapsedTime, zAxisDir, sceneParallaxIntensity, buckets);

			if (entity->HasAnyCallbackFunction())
			{
				entity->RunCallbackScript();
			}
		}
		entity->Release();
		iter = m_lastFrameCallbacks.erase(iter);
	}
}

bool ETHActiveEntityHandler::RemoveFinishedTemporaryEntity(ETHRenderEntity* entity, ETHBucketManager& buckets)
{
	if ((entity->IsTemporary() && entity->AreParticlesOver()))
	{
		const Vector2 v2Bucket = ETHBucketManager::GetBucket(entity->GetPositionXY(), buckets.GetBucketSize());
		ETHBucketMap::iterator bucketIter = buckets.Find(v2Bucket);

		assert(bucketIter != buckets.GetLastBucket());

		// Remove from main bucket map
		buckets.DeleteEntity(entity->GetID(), v2Bucket);

		#if defined(_DEBUG) || defined(DEBUG)
		 ETH_STREAM_DECL(ss) << ("Entity ") << entity->GetEntityName() << (" (ID#") << entity->GetID() << (") removed from dynamic entity list (particle effects over)");
		 m_provider->Log(ss.str(), Platform::Logger::INFO);
		#endif
		return true;
	}
	return false;
}
