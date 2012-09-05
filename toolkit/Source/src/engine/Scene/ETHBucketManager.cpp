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

#include "ETHBucketManager.h"
#include "../Entity/ETHEntityArray.h"
#include "../Entity/ETHRenderEntity.h"
#include "../Entity/ETHEntityChooser.h"
#include <iostream>

// Vector2 hash function
namespace boost {
inline std::size_t hash_value(Vector2 const& p)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, p.x);
	boost::hash_combine(seed, p.y);
	return seed;
}
} // namepace boost

ETHBucketManager::ETHBucketManager(const ETHResourceProviderPtr& provider, const Vector2& bucketSize, const bool drawingBorderBuckets) :
	m_bucketSize(bucketSize),
	m_provider(provider),
	m_drawingBorderBuckets(drawingBorderBuckets)
{
}

ETHBucketManager::~ETHBucketManager()
{
	for (ETHBucketMap::iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList::const_iterator iEnd = bucketIter->second.end();
		for (ETHEntityList::iterator iter = bucketIter->second.begin(); iter != iEnd; ++iter)
		{
			(*iter)->Kill();
			(*iter)->Release();
		}
	}
}

ETHBucketMap::iterator ETHBucketManager::GetFirstBucket()
{
	return m_entities.begin();
}

ETHBucketMap::iterator ETHBucketManager::GetLastBucket()
{
	return m_entities.end();
}

ETHBucketMap::const_iterator ETHBucketManager::GetFirstBucket() const
{
	return m_entities.begin();
}

ETHBucketMap::const_iterator ETHBucketManager::GetLastBucket() const
{
	return m_entities.end();
}

bool ETHBucketManager::IsEmpty() const
{
	return m_entities.empty();
}

ETHBucketMap::const_iterator ETHBucketManager::Find(const Vector2& key) const
{
	return m_entities.find(key);
}

ETHBucketMap::iterator ETHBucketManager::Find(const Vector2& key)
{
	return m_entities.find(key);
}

const Vector2& ETHBucketManager::GetBucketSize() const
{
	return m_bucketSize;
}

bool ETHBucketManager::IsDrawingBorderBuckets() const
{
	return m_drawingBorderBuckets;
}

void ETHBucketManager::SetBorderBucketsDrawing(const bool enable)
{
	m_drawingBorderBuckets = enable;
}

std::size_t ETHBucketManager::GetNumEntities(const Vector2& key) const
{
	ETHBucketMap::const_iterator iter = Find(key);
	if (iter != GetLastBucket())
	{
		return iter->second.size();
	}
	else
	{
		return 0;
	}
}

void ETHBucketManager::Add(ETHRenderEntity* entity, const SIDE side)
{
	const Vector2 bucket = ETHGlobal::GetBucket(entity->GetPositionXY(), GetBucketSize());
	if (side == FRONT)
	{
		m_entities[bucket].push_front(entity);
	}
	else
	{
		m_entities[bucket].push_back(entity);
	}

	#ifdef _DEBUG
	ETH_STREAM_DECL(ss) << GS_L("Entity ") << entity->GetEntityName() << GS_L(" (ID#") << entity->GetID()
						<< GS_L(") added to bucket ") << GS_L("(") << bucket.x << GS_L(", ") << bucket.y << GS_L(")");
	m_provider->Log(ss.str(), Platform::Logger::INFO);
	#endif
}

// moves an entity from a bucket to another
bool ETHBucketManager::MoveEntity(const int id, const Vector2 &currentBucket, const Vector2 &destBucket)
{
	// if the destiny bucket is the current bucket, don't need to do anything
	if (currentBucket == destBucket)
		return true;

	// remove the entity from its current bucket
	ETHBucketMap::iterator bucketIter = Find(currentBucket);

	if (bucketIter == GetLastBucket())
	{
		ETH_STREAM_DECL(ss) << GS_L("The current bucket doesn't exist: (") << currentBucket.x << GS_L(",") << currentBucket.y << GS_L(")");
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return false;
	}

	ETHRenderEntity* entity = 0;
	ETHEntityList& entityList = bucketIter->second;
	for (ETHEntityList::iterator iter = entityList.begin(); iter != entityList.end(); ++iter)
	{
		if ((*iter)->GetID() == id)
		{
			entity = *iter;
			entityList.erase(iter);
			break;
		}
	}

	// if the entity hasn't been found
	if (!entity)
	{
		ETH_STREAM_DECL(ss) << GS_L("Couldn't find entity ID ") << id << GS_L(" to move");
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return false;
	}

	// adds the entity to the destiny bucket
	if (entity->GetType() == ETH_HORIZONTAL)
	{
		m_entities[destBucket].push_front(entity);
	}
	else
	{
		m_entities[destBucket].push_back(entity);
	}

	#ifdef _DEBUG
	ETH_STREAM_DECL(ss) << GS_L("Entity moved from bucket (") << currentBucket.x << GS_L(",") << currentBucket.y << GS_L(") to bucket (")
		 << destBucket.x << GS_L(",") << destBucket.y << GS_L(")");
	m_provider->Log(ss.str(), Platform::Logger::INFO);
	#endif
	return true;
}

unsigned int ETHBucketManager::GetNumEntities() const
{
	unsigned int nEntities = 0;
	for (ETHBucketMap::const_iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		nEntities += bucketIter->second.size();
	}
	return nEntities;
}

// TODO-TO-DO: this method is too large...
int ETHBucketManager::SeekEntity(const Vector2 &at, ETHEntity **pOutData, const ETHSceneProperties& props, ETHEntity *pAfterThisOne)
{
	int closestFromBehindID =-1;
	ETHSpriteEntity *pClosestFromBehind = 0;

	//int currentID =-1;
	//ETHSpriteEntity *pCurrent = 0;

	int closestFromTheFrontID =-1;
	ETHSpriteEntity *pClosestFromTheFront = 0;

	if (pAfterThisOne)
	{
		ETH_VIEW_RECT rect = pAfterThisOne->GetScreenRect(props);
		if (at.x < rect.v2Min.x || at.x > rect.v2Max.x ||
			at.y < rect.v2Min.y || at.y > rect.v2Max.y)
		{
			pAfterThisOne = 0;
		}
	}

	if (IsEmpty())
	{
		return -1;
	}

	std::list<Vector2> buckets;
	ETHGlobal::GetIntersectingBuckets(buckets, at + m_provider->GetVideo()->GetCameraPos(), Vector2(1,1), GetBucketSize(), true, true);

	// seeks the closest intersecting entity from behind
	for (std::list<Vector2>::const_reverse_iterator sceneBucketIter = buckets.rbegin();
		sceneBucketIter != buckets.rend(); ++sceneBucketIter)
	{
		ETHBucketMap::iterator bucketIter = Find(*sceneBucketIter);

		if (bucketIter == GetLastBucket())
			continue;

		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_reverse_iterator iEnd = entityList.rend();
		bool escape = false;
		for (ETHEntityList::reverse_iterator iter = entityList.rbegin(); iter != iEnd; ++iter)
		{
			ETHSpriteEntity *pRenderEntity = (*iter);
			const ETH_VIEW_RECT box = pRenderEntity->GetScreenRect(props);

			if (at.x > box.v2Min.x && at.x < box.v2Max.x &&
				at.y > box.v2Min.y && at.y < box.v2Max.y)
			{
				if (!pAfterThisOne)
				{
					if (pOutData)
						*pOutData = (*iter);
					return (*iter)->GetID();
				}
				else
				{
					if (pRenderEntity->GetID() == pAfterThisOne->GetID())
					{
						//pCurrent = const_cast<ETHRenderEntity*>(*iter);
						//currentID = (*iter)->GetID();
						escape = true;
						break;
					}
				}
				pClosestFromBehind = const_cast<ETHRenderEntity*>(*iter);
				closestFromBehindID = pClosestFromBehind->GetID();
			}
		}
		if (escape)
			break;
	}

	// seeks the first intersecting entity from the front
	for (std::list<Vector2>::const_iterator sceneBucketIter = buckets.begin(); sceneBucketIter != buckets.end(); ++sceneBucketIter)
	{
		ETHBucketMap::iterator bucketIter = Find(*sceneBucketIter);

		if (bucketIter == GetLastBucket())
			continue;

		ETHEntityList::iterator iter;
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		bool escape = false;
		for (iter = entityList.begin(); iter != iEnd; ++iter)
		{
			ETHSpriteEntity *pRenderEntity = (*iter);
			const ETH_VIEW_RECT box = pRenderEntity->GetScreenRect(props);

			if (at.x > box.v2Min.x && at.x < box.v2Max.x &&
				at.y > box.v2Min.y && at.y < box.v2Max.y)
			{
				pClosestFromTheFront = const_cast<ETHRenderEntity*>(*iter);
				closestFromTheFrontID = pClosestFromTheFront->GetID();
				escape = true;
				break;
			}
		}
		if (escape)
			break;
	}
	if (closestFromBehindID >= 0)
	{
		if (pOutData && pClosestFromBehind)
			*pOutData = pClosestFromBehind;
		return closestFromBehindID;
	}
	if (pOutData && pClosestFromTheFront)
		*pOutData = pClosestFromTheFront;
	return closestFromTheFrontID;
}

ETHSpriteEntity* ETHBucketManager::SeekEntity(const int id)
{
	for (ETHBucketMap::iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			if ((*iter)->GetID() == id)
				return (*iter);
		}
	}
	return 0;
}

ETHSpriteEntity* ETHBucketManager::SeekEntity(const str_type::string& fileName)
{
	for (ETHBucketMap::iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			if ((*iter)->GetEntityName() == fileName)
				return (*iter);
		}
	}
	return 0;
}

bool ETHBucketManager::DeleteEntity(const int id)
{
	for (ETHBucketMap::iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			if ((*iter)->GetID() == id)
			{
#				ifdef _DEBUG
					ETH_STREAM_DECL(ss) << GS_L("Entity ") << (*iter)->GetEntityName() << GS_L(" (ID#") << (*iter)->GetID() << GS_L(") removed (DeleteEntity method)");
					m_provider->Log(ss.str(), Platform::Logger::INFO);
#				endif

				if (m_entityKillListener)
					m_entityKillListener->EntityKilled((*iter));

				(*iter)->Kill();
				(*iter)->Release();
				entityList.erase(iter);
				return true;
			}
		}
	}
	return false;
}

bool ETHBucketManager::DeleteEntity(const int id, const Vector2 &searchBucket, const bool stopSfx)
{
	ETHBucketMap::iterator bucketIter = Find(searchBucket);

	// try getting it from bucket (faster)
	if (bucketIter != GetLastBucket())
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_reverse_iterator iEnd = entityList.rend();
		for (ETHEntityList::reverse_iterator iter = entityList.rbegin(); iter != iEnd; ++iter)
		{
			if ((*iter)->GetID() == id)
			{
				#ifdef _DEBUG
				ETH_STREAM_DECL(ss) << GS_L("Entity ") << (*iter)->GetEntityName() << GS_L(" (ID#") << (*iter)->GetID() << GS_L(") removed (DeleteEntity method)");
				m_provider->Log(ss.str(), Platform::Logger::INFO);
				#endif

				if (m_entityKillListener)
					m_entityKillListener->EntityKilled((*iter));

				if(!stopSfx)
					(*iter)->SetStopSFXWhenDestroyed(false);
				(*iter)->Kill();
				(*iter)->Release();
				ETHEntityList::iterator i = iter.base();
				--i;
				entityList.erase(i);
				return true;
			}
		}
	}

	// If it can't find it, it probably means the entity is lost and its move
	// request has been sent. Let's just kill it then
	for (bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			if ((*iter)->GetID() == id)
			{
#				ifdef _DEBUG
					ETH_STREAM_DECL(ss) << GS_L("Entity ") << (*iter)->GetEntityName() << GS_L(" (ID#") << (*iter)->GetID() << GS_L(") removed (DeleteEntity method)");
					m_provider->Log(ss.str(), Platform::Logger::INFO);
#				endif

				if (m_entityKillListener)
					m_entityKillListener->EntityKilled((*iter));

				(*iter)->Kill();
				return true;
			}
		}
	}

	ETH_STREAM_DECL(ss) << GS_L("Couldn't find the entity to delete: ID") << id;
	m_provider->Log(ss.str(), Platform::Logger::ERROR);
	return false;
}

void ETHBucketManager::GetEntityArrayByName(const str_type::string& name, ETHEntityArray &outVector)
{
	for (ETHBucketMap::iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			ETHSpriteEntity *pEntity = *iter;
			if (pEntity->GetEntityName() == name)
			{
				outVector.push_back(pEntity);
			}
		}
	}
}

void ETHBucketManager::GetEntityArrayFromBucket(const Vector2 &bucket, ETHEntityArray &outVector, const ETHEntityChooser& chooser)
{
	ETHBucketMap::iterator bucketIter = Find(bucket);
	if (bucketIter == GetLastBucket())
		return;

	ETHEntityList& entityList = bucketIter->second;
	ETHEntityList::const_iterator iEnd = entityList.end();
	for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
	{
		if (chooser.Choose(*iter))
			outVector.push_back(*iter);
	}
}

void ETHBucketManager::GetEntityArrayFromBucket(const Vector2 &bucket, ETHEntityArray &outVector)
{
	GetEntityArrayFromBucket(bucket, outVector, ETHEntityDefaultChooser());
}

void ETHBucketManager::GetWhiteListedEntityArrayFromBucket(const Vector2 &bucket, ETHEntityArray &outVector,
														   const str_type::string& semicolonSeparatedNames)
{
	GetEntityArrayFromBucket(bucket, outVector, ETHEntityNameArrayChooser(semicolonSeparatedNames, false));
}

void ETHBucketManager::GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector, const ETHEntityChooser& chooser)
{
	GetEntityArrayFromBucket(bucket, outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2( 1, 0), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2( 1, 1), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2( 0, 1), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2(-1, 1), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2(-1, 0), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2(-1,-1), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2( 0,-1), outVector, chooser);
	GetEntityArrayFromBucket(bucket + Vector2( 1,-1), outVector, chooser);
}

void ETHBucketManager::GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector)
{
	GetEntitiesAroundBucket(bucket, outVector, ETHEntityDefaultChooser());
}

void ETHBucketManager::GetWhiteListedEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector, const str_type::string& semicolonSeparatedNames)
{
	GetEntitiesAroundBucket(bucket, outVector, ETHEntityNameArrayChooser(semicolonSeparatedNames, false));
}

void ETHBucketManager::GetEntitiesAroundBucketWithBlackList(const Vector2& bucket, ETHEntityArray &outVector, const str_type::string& semicolonSeparatedNames)
{
	GetEntitiesAroundBucket(bucket, outVector, ETHEntityNameArrayChooser(semicolonSeparatedNames, true));
}

void ETHBucketManager::GetIntersectingBuckets(std::list<Vector2>& bucketList,
	const Vector2& pos, const Vector2& size, const bool upperSeams, const bool lowerSeams)
{
	ETHGlobal::GetIntersectingBuckets(bucketList, pos, size, GetBucketSize(), upperSeams, lowerSeams);
}

void ETHBucketManager::GetVisibleEntities(ETHEntityArray &outVector)
{
	std::list<Vector2> bucketList;
	GetIntersectingBuckets(bucketList, m_provider->GetVideo()->GetCameraPos(),
						   m_provider->GetVideo()->GetScreenSizeF(), IsDrawingBorderBuckets(), IsDrawingBorderBuckets());

	// Loop through all visible Buckets
	for (std::list<Vector2>::iterator bucketPositionIter = bucketList.begin();
		bucketPositionIter != bucketList.end(); ++bucketPositionIter)
	{
		ETHBucketMap::const_iterator bucketIter = Find(*bucketPositionIter);

		if (bucketIter == GetLastBucket())
			continue;

		const ETHEntityList& entityList = bucketIter->second;
		if (entityList.empty())
			continue;

		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::const_iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			outVector.push_back(*iter);
		}
	}
}

void ETHBucketManager::GetIntersectingEntities(const Vector2 &point, ETHEntityArray &outVector, const bool screenSpace, const ETHSceneProperties& props)
{
	ETHEntityArray temp;
	const Vector2 v2Bucket(ETHGlobal::GetBucket(point, GetBucketSize()));
	GetEntitiesAroundBucket(v2Bucket, temp);

	for (unsigned int t=0; t<temp.size(); t++)
	{
		ETH_VIEW_RECT rect = temp[t]->GetScreenRect(props);

		if (!screenSpace)
		{
			const Vector2 cameraPos = m_provider->GetVideo()->GetCameraPos();
			rect.v2Max += cameraPos;
			rect.v2Min += cameraPos;
		}

		if (point.x < rect.v2Min.x)
			continue;
		if (point.y < rect.v2Min.y)
			continue;
		if (point.x > rect.v2Max.x)
			continue;
		if (point.y > rect.v2Max.y)
			continue;
		outVector.push_back(temp[t]);
	}
}

void ETHBucketManager::GetEntityArray(ETHEntityArray &outVector)
{
	for (ETHBucketMap::iterator bucketIter = GetFirstBucket(); bucketIter != GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			outVector.push_back(*iter);
		}
	}
}

void ETHBucketManager::RequestBucketMove(ETHEntity* target, const Vector2& oldPos, const Vector2& newPos)
{
	ETHBucketMoveRequest request(target, oldPos, newPos, GetBucketSize());
	if (request.IsABucketMove())
	{
		m_moveRequests.push_back(request);
	}
}

void ETHBucketManager::ResolveMoveRequests()
{
	for (std::list<ETHBucketMoveRequest>::iterator iter = m_moveRequests.begin();
		iter != m_moveRequests.end(); ++iter)
	{
		// if it's dead, no use in moving it. Let's just discard
		if (!iter->IsAlive())
		{
			DeleteEntity(iter->GetID());
			continue;
		}
		MoveEntity(iter->GetID(), iter->GetOldBucket(), iter->GetNewBucket());
	}
	m_moveRequests.clear();
}

void ETHBucketManager::SetDestructionListener(const ETHEntityKillListenerPtr& listener)
{
	m_entityKillListener = listener;
}

ETHBucketManager::ETHBucketMoveRequest::ETHBucketMoveRequest(ETHEntity* target, const Vector2& oldPos, const Vector2& newPos, const Vector2& bucketSize) :
	entity(target)
{
	oldBucket = ETHGlobal::GetBucket(oldPos, bucketSize);
	newBucket = ETHGlobal::GetBucket(newPos, bucketSize);
}


bool ETHBucketManager::ETHBucketMoveRequest::IsAlive() const
{
	return entity->IsAlive();
}

bool ETHBucketManager::ETHBucketMoveRequest::IsABucketMove() const
{
	return (oldBucket != newBucket);
}

int ETHBucketManager::ETHBucketMoveRequest::GetID() const
{
	return entity->GetID();
}

const Vector2& ETHBucketManager::ETHBucketMoveRequest::GetOldBucket() const
{
	return oldBucket;
}

const Vector2& ETHBucketManager::ETHBucketMoveRequest::GetNewBucket() const
{
	return newBucket;
}
