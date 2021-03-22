#ifndef ETH_BUCKET_MANAGER_H_
#define ETH_BUCKET_MANAGER_H_

class ETHRenderEntity;
class ETHSpriteEntity;
class ETHEntity;
class ETHEntityArray;

#include "ETHSceneProperties.h"

#include "../Resource/ETHResourceProvider.h"

#include <hopscotch_map.h>

#include <cstdint>

#include <list>
#include <map>

typedef std::list<ETHRenderEntity*> ETHEntityList;
typedef tsl::hopscotch_map<uint64_t, ETHEntityList> ETHBucketMap;
typedef std::map<uint64_t, ETHEntityList> ETHOrderedBucketMap;

class ETHEntityChooser;

class ETHBucketManager
{
public:
	static uint64_t Hash(const math::Vector2& v);

	static Vector2 GetBucket(const Vector2 &v2, const Vector2 &v2BucketSize);
	static void GetIntersectingBuckets(
		std::list<Vector2> &outList,
		const Vector2 &pos,
		const Vector2 &size,
		const Vector2 &bucketSize,
		bool includeUpperSeams = false,
		bool includeLowerSeams = false);

	static Vector2 ComputeBucketRelativePosition(const Vector2& p, const Vector2 &bucketSize);

	ETHBucketManager(const ETHResourceProviderPtr& provider, const Vector2& bucketSize, const bool drawingBorderBuckets);
	~ETHBucketManager();

	enum SIDE
	{
		FRONT = 0,
		BACK = 1
	};

	ETHBucketMap::iterator GetFirstBucket();
	ETHBucketMap::iterator GetLastBucket();
	ETHBucketMap::iterator Find(const Vector2& key);
	ETHBucketMap::const_iterator Find(const Vector2& key) const;
	ETHBucketMap::const_iterator GetFirstBucket() const;
	ETHBucketMap::const_iterator GetLastBucket() const;

	bool IsEmpty() const;
	bool IsDrawingBorderBuckets() const;
	std::size_t GetNumEntities(const Vector2& key) const;
	void SetBorderBucketsDrawing(const bool enable);

	void Add(ETHRenderEntity* entity, const SIDE side);
	const Vector2& GetBucketSize() const;

	unsigned int GetNumEntities() const;

	/// Get the list of visible buckets
	void GetIntersectingBuckets(std::list<Vector2>& bucketList, const Vector2& pos, const Vector2& size, const bool upperSeams, const bool lowerSeams);

	/// Search for an entity whose location collides with the 'at' point
	/// It returns the entity ID # and it's further data. If nAfterThisID is greater than
	/// -1 the entity returned will be another one (if the point collides with more than
	/// one entities)
	int SeekEntity(const Vector2& pointAbsPos, ETHEntity** pOutData, const ETHSceneProperties& props, ETHSpriteEntity* pAfterThisOne = 0);

	/// Seek the entity by ID #
	ETHSpriteEntity *SeekEntity(const int id);

	/// Seek the entity by it's original file name file name
	ETHSpriteEntity *SeekEntity(const std::string& fileName);

	/// Delete the entity by ID #
	bool DeleteEntity(ETHEntity* pEntity, const Vector2 &searchBucket);

	/// Delete the entity by ID #
	bool DeleteEntity(const int id);

	/// get an array of pointers with all entities named 'name' in scene
	void GetEntityArrayByName(const std::string& name, ETHEntityArray &outVector);

	/// get an array of pointers with all entities in the bucket
	void GetEntityArrayFromBucket(const Vector2 &bucket, ETHEntityArray &outVector);
	void GetEntityArrayFromBucket(const Vector2 &bucket, ETHEntityArray &outVector, const ETHEntityChooser& chooser);
	void GetWhiteListedEntityArrayFromBucket(const Vector2 &bucket, ETHEntityArray &outVector, const std::string& semicolonSeparatedNames);

	/// entities around the bucket
	void GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector);
	void GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector, const ETHEntityChooser& chooser);
	void GetWhiteListedEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector, const std::string& semicolonSeparatedNames);
	void GetEntitiesAroundBucketWithBlackList(const Vector2& bucket, ETHEntityArray &outVector, const std::string& semicolonSeparatedNames);

	/// get an array of visible entities
	void GetVisibleEntities(ETHEntityArray &outVector);

	/// get an array containing all entities that intersect with the point
	void GetIntersectingEntities(const Vector2 &point, ETHEntityArray &outVector, const bool screenSpace, const ETHSceneProperties& props);

	/// get an array containing all entities in scene
	void GetEntityArray(ETHEntityArray &outVector);

	void RequestBucketMove(ETHEntity* target, const Vector2& oldPos, const Vector2& newPos);

	void ResolveMoveRequests();

private:

	class ETHBucketMoveRequest
	{
		Vector2 oldBucket, newBucket;
		ETHEntity* entity;
	public:
		ETHBucketMoveRequest(
			ETHEntity* target,
			const Vector2& oldPos,
			const Vector2& newPos,
			const Vector2& bucketSize);
		~ETHBucketMoveRequest();
		bool IsABucketMove() const;
		ETHEntity* GetEntity() const;
		int GetID() const;
		const Vector2& GetOldBucket() const;
		const Vector2& GetNewBucket() const;
		bool IsAlive() const;
	};

	typedef boost::shared_ptr<ETHBucketMoveRequest> ETHBucketMoveRequestPtr;

	bool MoveEntity(ETHEntity* entity, const Vector2 &currentBucket, const Vector2 &destBucket);

	std::list<ETHBucketMoveRequestPtr> m_moveRequests;

	ETHResourceProviderPtr m_provider;
	ETHBucketManager& operator=(const ETHBucketManager& p);
	ETHBucketMap m_entities;
	const Vector2 m_bucketSize;
	bool m_drawingBorderBuckets;
};

#endif
