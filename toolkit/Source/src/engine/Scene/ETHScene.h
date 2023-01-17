#ifndef ETH_SCENE_H_
#define ETH_SCENE_H_

#include "ETHBucketManager.h"

#include "../Entity/ETHEntityArray.h"

#include "../Util/ETHASUtil.h"

#include "../Scene/ETHActiveEntityHandler.h"

#include "../Physics/ETHPhysicsSimulator.h"

#include "../Renderer/ETHEntityRenderingManager.h"

class ETHScene
{
public:
	ETHScene(
		const std::string& fileName,
		const std::string& lightmapDirectory,
		ETHResourceProviderPtr provider,
		const ETHSceneProperties& props,
		asIScriptModule *pModule,
		asIScriptContext *pContext,
		ETHEntityCache& entityCache,
		const Vector2 &v2BucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));

	ETHScene(
		ETHResourceProviderPtr provider,
		const ETHSceneProperties& props,
		asIScriptModule *pModule,
		asIScriptContext *pContext,
		const Vector2 &v2BucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));

	~ETHScene();

	void ClearResources();

	void RenderScene(const ETHBackBufferTargetManagerPtr& backBuffer);

	bool SaveToFile(const std::string& fileName, ETHEntityCache& entityCache);
	int AddEntity(ETHRenderEntity* pEntity);
	int AddEntity(ETHRenderEntity* pEntity, const std::string& alternativeName);
	void SetSceneProperties(const ETHSceneProperties &prop);
	
	static int UpdateIDCounter(ETHEntity* pEntity);

	void LoadLightmapsFromBitmapFiles(const std::string& lightmapDirectory);
	bool AreLightmapsEnabled() const;

	void Update(
		const float lastFrameElapsedTime,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		asIScriptFunction* onCreateCallbackFunction,
		asIScriptFunction* onUpdateCallbackFunction);

	bool DeleteEntity(ETHEntity *pEntity);

	const ETHSceneProperties* GetSceneProperties() const;
	ETHSceneProperties* GetEditableSceneProperties();

	bool AddFloatData(const std::string &entity, const std::string &name, const float value);
	bool AddIntData(const std::string &entity, const std::string &name, const int value);
	bool AddUIntData(const std::string &entity, const std::string &name, const unsigned int value);
	bool AddStringData(const std::string &entity, const std::string &name, const std::string &value);
	bool AddVector2Data(const std::string &entity, const std::string &name, const Vector2 &value);
	bool AddVector3Data(const std::string &entity, const std::string &name, const Vector3 &value);
	bool AddCustomData(const std::string &entity, const std::string &name, const ETHCustomDataConstPtr &inData);

	void SetBucketClearenceFactor(const float factor);
	float GetBucketClearenceFactor() const;

	void SetAmbientLight(const Vector3 &color);
	Vector3 GetAmbientLight() const;

	void SetBorderBucketsDrawing(const bool enable);
	bool IsDrawingBorderBuckets() const;

	void SetZAxisDirection(const Vector2 &v2);
	Vector2 GetZAxisDirection() const;

	void SetParallaxIntensity(const float intensity);
	float GetParallaxIntensity() const;

	int GetLastID() const;
	float GetMaxHeight() const;
	float GetMinHeight() const;
	Vector2 GetBucketSize() const;
	int GetNumProcessedEntities();
	int GetNumRenderedPieces();
	static void ScaleEntities(const ETHEntityArray& entities, ETHBucketManager& buckets, const float scale, const bool scalePosition);

	ETHBucketManager& GetBucketManager();

	void GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace);

	unsigned int GetNumEntities() const;

	void RecoverResources();

	ETHPhysicsSimulator& GetSimulator();

	void ResolveJoints();

	void AddEntityToPersistentList(ETHRenderEntity* entity);

	bool AddSceneFromFile(
		const std::string& fileName,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		const bool readSceneProperties,
		const Vector3& offset,
		ETHEntityArray &outVector,
		const bool shouldGenerateNewID,
		const bool immediatelyLoadSprites);

	bool AddSceneFromString(
		const std::string& fileName,
		const std::string& xmlContent,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		const bool readSceneProperties,
		const Vector3& offset,
		ETHEntityArray &outVector,
		const bool shouldGenerateNewID,
		const bool immediatelyLoadSprites);

	std::string AssembleEntityPath() const;
	
	std::string GetLightmapDirectory() const;
	
	void EmptyRenderingQueue();
	
	static std::string ConvertSceneFileNameToLightmapDirectory(std::string filePath);

private:
	void Init(ETHResourceProviderPtr provider, const ETHSceneProperties& props, asIScriptModule *pModule, asIScriptContext *pContext);

	void MapEntitiesToBeRendered(
		float &maxHeight,
		float &minHeight,
		const ETHBackBufferTargetManagerPtr& backBuffer);

	void DrawEntityMultimap(const ETHBackBufferTargetManagerPtr& backBuffer);

	bool AssignCallbackScript(ETHSpriteEntity* entity);

	void AssignControllerToEntity(
		ETHEntity* entity,
		asIScriptFunction* callback,
		asIScriptFunction* constructorCallback);

	bool AddEntitiesFromXMLFile(
		const std::string& fileName,
		TiXmlElement *pElement,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		const bool readSceneProperties,
		const Vector3& offset,
		ETHEntityArray &outVector,
		const bool shouldGenerateNewIDs,
		const bool immediatelyLoadSprites);

	void FillCurrentlyVisibleBucketList(std::list<Vector2>& bucketList, const ETHBackBufferTargetManagerPtr& backBuffer);

	void FillMultimapAndClearPersistentList(
		const std::list<Vector2>& currentBucketList,
		const ETHBackBufferTargetManagerPtr& backBuffer);

	float ComputeDrawHash(const float entityDepth, const ETHSpriteEntity* entity) const;
	ETHBucketManager m_buckets;
	ETHActiveEntityHandler m_activeEntityHandler;

	std::list<ETHRenderEntity*> m_persistentEntities;

	ETHEntityRenderingManager m_renderingManager;
	ETHResourceProviderPtr m_provider;
	ETHSceneProperties m_sceneProps;
	ETHPhysicsSimulator m_physicsSimulator;
	asIScriptModule *m_pModule;
	asIScriptContext *m_pContext;
	float m_maxSceneHeight, m_minSceneHeight;
	static int m_idCounter;
	unsigned int m_nProcessedEntities;
	unsigned int m_nRenderedPieces;
	float m_bucketClearenceFactor;
	std::string m_lightmapDirectory;
	bool m_onCreatedFunctionExecuted;
};

typedef boost::shared_ptr<ETHScene> ETHScenePtr;

#endif
