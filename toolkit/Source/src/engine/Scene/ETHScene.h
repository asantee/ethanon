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
		const str_type::string& fileName,
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

	void RenderScene(const bool roundUp, const ETHBackBufferTargetManagerPtr& backBuffer);

	bool SaveToFile(const str_type::string& fileName, ETHEntityCache& entityCache);
	int AddEntity(ETHRenderEntity* pEntity);
	int AddEntity(ETHRenderEntity* pEntity, const str_type::string& alternativeName);
	void SetSceneProperties(const ETHSceneProperties &prop);
	void EnableLightmaps(const bool enable);
	void EnableRealTimeShadows(const bool enable);
	bool AreRealTimeShadowsEnabled() const;

	str_type::string ConvertFileNameToLightmapDirectory(str_type::string filePath);
	bool GenerateLightmaps(const int id = -1);
	void LoadLightmapsFromBitmapFiles(const str_type::string& currentSceneFilePath);
	void SaveLightmapsToFile(const str_type::string& directory);
	bool AreLightmapsEnabled() const;

	void Update(
		const float lastFrameElapsedTime,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		asIScriptFunction* onUpdateCallbackFunction);

	bool DeleteEntity(ETHEntity *pEntity);

	const ETHSceneProperties* GetSceneProperties() const;
	ETHSceneProperties* GetEditableSceneProperties();

	void AddLight(const ETHLight& light);

	bool AddFloatData(const str_type::string &entity, const str_type::string &name, const float value);
	bool AddIntData(const str_type::string &entity, const str_type::string &name, const int value);
	bool AddUIntData(const str_type::string &entity, const str_type::string &name, const unsigned int value);
	bool AddStringData(const str_type::string &entity, const str_type::string &name, const str_type::string &value);
	bool AddVector2Data(const str_type::string &entity, const str_type::string &name, const Vector2 &value);
	bool AddVector3Data(const str_type::string &entity, const str_type::string &name, const Vector3 &value);
	bool AddCustomData(const str_type::string &entity, const str_type::string &name, const ETHCustomDataConstPtr &inData);

	int GetNumLights();
	int CountLights();

	void SetLightIntensity(const float intensity);
	float GetLightIntensity() const;

	void SetBucketClearenceFactor(const float factor);
	float GetBucketClearenceFactor() const;

	void SetAmbientLight(const Vector3 &color);
	Vector3 GetAmbientLight() const;

	void SetBorderBucketsDrawing(const bool enable);
	bool IsDrawingBorderBuckets() const;

	void SetZAxisDirection(const Vector2 &v2);
	Vector2 GetZAxisDirection() const;

	int GetLastID() const;
	float GetMaxHeight() const;
	float GetMinHeight() const;
	Vector2 GetBucketSize() const;
	int GetNumProcessedEntities();
	int GetNumRenderedPieces();
	void ScaleEntities(const float scale, const bool scalePosition);

	ETHBucketManager& GetBucketManager();

	void GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace);

	unsigned int GetNumEntities() const;

	void RecoverResources(const Platform::FileManagerPtr& expansionFileManager);
	void ClearLightmaps();

	void SetZBuffer(const bool enable);
	bool GetZBuffer() const;

	ETHPhysicsSimulator& GetSimulator();

	void ResolveJoints();

	void AddEntityToPersistentList(ETHRenderEntity* entity);

private:
	bool LoadFromFile(
		const str_type::string& fileName,
		ETHEntityCache& entityCache,
		const str_type::string &entityPath);

	void Init(ETHResourceProviderPtr provider, const ETHSceneProperties& props, asIScriptModule *pModule, asIScriptContext *pContext);

	void MapEntitiesToBeRendered(
		float &maxHeight,
		float &minHeight,
		const ETHBackBufferTargetManagerPtr& backBuffer);

	void DrawEntityMultimap(const bool roundUp, const ETHBackBufferTargetManagerPtr& backBuffer);

	bool AssignCallbackScript(ETHSpriteEntity* entity);

	void AssignControllerToEntity(
		ETHEntity* entity,
		asIScriptFunction* callback,
		asIScriptFunction* constructorCallback);

	bool DrawBucketOutlines(const ETHBackBufferTargetManagerPtr& backBuffer);
	bool ReadFromXMLFile(
		const str_type::string& fileName,
		TiXmlElement *pElement,
		ETHEntityCache& entityCache,
		const str_type::string &entityPath);

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
	int m_idCounter;
	unsigned int m_nCurrentLights;
	unsigned int m_nProcessedEntities;
	unsigned int m_nRenderedPieces;
	bool m_enableZBuffer;
	float m_bucketClearenceFactor;
};

typedef boost::shared_ptr<ETHScene> ETHScenePtr;

#endif
