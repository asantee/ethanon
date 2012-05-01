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

#include "ETHTempEntityHandler.h"
#include "../Entity/ETHRenderEntity.h"

ETHTempEntityHandler::ETHTempEntityHandler(ETHResourceProviderPtr provider) :
	m_provider(provider)
{
}

ETHTempEntityHandler::~ETHTempEntityHandler()
{
	for (ETHEntityList::iterator iter = m_dynamicOrTempEntities.begin(); iter != m_dynamicOrTempEntities.end(); iter++)
	{
		(*iter)->Release();
	}
	ClearCallbackEntities();
}

bool ETHTempEntityHandler::IsCallbackListEmpty() const
{
	return m_lastFrameCallbacks.empty();
}

void ETHTempEntityHandler::ClearCallbackEntities()
{
	for (std::list<ETHRenderEntity*>::iterator iter = m_lastFrameCallbacks.begin();
		iter != m_lastFrameCallbacks.end(); iter++)
	{
		(*iter)->Release();
	}
	m_lastFrameCallbacks.clear();
}

bool ETHTempEntityHandler::AddEntityWhenEligible(ETHRenderEntity* entity)
{
	if (IsTempEntityEligible(entity))
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

bool ETHTempEntityHandler::IsTempEntityEligible(ETHRenderEntity* entity) const
{
	return (((entity->HasCallback() || entity->HasSimulatedBody()) && !entity->IsStatic()) || entity->IsTemporary());
}

bool ETHTempEntityHandler::AddCallbackWhenEligible(ETHRenderEntity* entity)
{
	if (IsCallbackEligible(entity))
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

bool ETHTempEntityHandler::IsCallbackEligible(ETHRenderEntity* entity) const
{
	return (!IsTempEntityEligible(entity));
}

void ETHTempEntityHandler::TestEntityLists() const
{
	for (std::list<ETHRenderEntity*>::const_iterator a = m_dynamicOrTempEntities.begin(); a != m_dynamicOrTempEntities.end(); a++)
	{
		for (std::list<ETHRenderEntity*>::const_iterator b = m_lastFrameCallbacks.begin(); b != m_lastFrameCallbacks.end(); b++)
		{
			assert((*a)->GetID() != (*b)->GetID());
		}
	}
}

void ETHTempEntityHandler::CheckTemporaryEntities(const Vector2& zAxisDir, ETHBucketManager& buckets, const unsigned long lastFrameElapsedTime)
{
	#ifdef _DEBUG
	TestEntityLists();
	#endif

	for (std::list<ETHRenderEntity*>::iterator iter = m_dynamicOrTempEntities.begin(); iter != m_dynamicOrTempEntities.end();)
	{
		ETHRenderEntity* pRenderEntity = *iter;

		pRenderEntity->Update(lastFrameElapsedTime, zAxisDir, buckets);

		// if the particle system is finished, erase it
		if ((pRenderEntity->IsTemporary() && pRenderEntity->AreParticlesOver()))
		{
			const Vector2 v2Bucket = ETHGlobal::GetBucket(pRenderEntity->GetPositionXY(), buckets.GetBucketSize());
			ETHBucketMap::iterator bucketIter = buckets.Find(v2Bucket);

			if (bucketIter == buckets.GetLastBucket())
			{
				iter++;
				continue;
			}

			// Remove from main bucket map
			ETHEntityList::const_iterator iEnd = bucketIter->second.end();
			for (ETHEntityList::iterator entityIter = bucketIter->second.begin(); entityIter != iEnd; entityIter++)
			{
				if ((*entityIter)->GetID() != pRenderEntity->GetID())
					continue;

				#ifdef _DEBUG
				ETH_STREAM_DECL(ss) << GS_L("Entity ") << pRenderEntity->GetEntityName() << GS_L(" (ID#") << pRenderEntity->GetID() << GS_L(") removed (particle effects over)");
				m_provider->Log(ss.str(), Platform::Logger::INFO);
				#endif

				pRenderEntity->SetStopSFXWhenDestroyed(false);
				pRenderEntity->Kill();
				pRenderEntity->Release();
				entityIter = bucketIter->second.erase(entityIter);
				break;
			}

			#ifdef _DEBUG
			ETH_STREAM_DECL(ss) << GS_L("Entity ") << (*iter)->GetEntityName() << GS_L(" (ID#") << pRenderEntity->GetID() << GS_L(") removed from dynamic entity list (particle effects over)");
			m_provider->Log(ss.str(), Platform::Logger::INFO);
			#endif
			(*iter)->Release();
			iter = m_dynamicOrTempEntities.erase(iter);
			continue;
		}
		iter++;
	}
}

void ETHTempEntityHandler::RunCallbacksFromList()
{
	for (std::list<ETHRenderEntity*>::iterator iter = m_dynamicOrTempEntities.begin(); iter != m_dynamicOrTempEntities.end();)
	{
		if (!((*iter)->IsAlive()))
		{
			#ifdef _DEBUG
			ETH_STREAM_DECL(ss) << GS_L("Entity removed from dynamic entity list: ") << (*iter)->GetEntityName();
			m_provider->Log(ss.str(), Platform::Logger::INFO);
			#endif
			(*iter)->Release();
			iter = m_dynamicOrTempEntities.erase(iter);
			continue;
		}
		else
		{
			if ((*iter)->HasCallback())
			{
				(*iter)->RunCallbackScript();
			}
			iter++;
		}
	}

	for (std::list<ETHRenderEntity*>::iterator iter = m_lastFrameCallbacks.begin(); iter != m_lastFrameCallbacks.end();)
	{
		if (!((*iter)->IsAlive()))
		{
			#ifdef _DEBUG
			ETH_STREAM_DECL(ss) << GS_L("Entity callback removed: ") << (*iter)->GetEntityName();
			m_provider->Log(ss.str(), Platform::Logger::INFO);
			#endif
			(*iter)->Release();
			iter = m_lastFrameCallbacks.erase(iter);
			continue;
		}
		else
		{
			if ((*iter)->HasCallback())
			{
				(*iter)->RunCallbackScript();
			}
			(*iter)->Release();
			iter++;
		}
	}
	m_lastFrameCallbacks.clear();
}
