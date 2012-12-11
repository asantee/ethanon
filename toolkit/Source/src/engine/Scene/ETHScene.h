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

#ifndef ETH_SCENE_H_
#define ETH_SCENE_H_

#include "ETHBucketManager.h"

#include "../Entity/ETHEntityArray.h"

#include "../Util/ETHASUtil.h"

#include "../Scene/ETHActiveEntityHandler.h"

#include "../Physics/ETHPhysicsSimulator.h"

#include "../Script/ETHEntityDestructorManager.h"

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
		const Vector2 &v2BucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));

	ETHScene(
		ETHResourceProviderPtr provider,
		const ETHSceneProperties& props,
		asIScriptModule *pModule,
		asIScriptContext *pContext,
		const Vector2 &v2BucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));

	~ETHScene();

	void ClearResources();

	void RenderScene(const bool roundUp);

	bool SaveToFile(const str_type::string& fileName);
	int AddEntity(ETHRenderEntity* pEntity);
	int AddEntity(ETHRenderEntity* pEntity, const str_type::string& alternativeName);
	void SetSceneProperties(const ETHSceneProperties &prop);
	bool GenerateLightmaps(const int id = -1);
	void EnableLightmaps(const bool enable);
	bool AreLightmapsEnabled() const;
	void EnableRealTimeShadows(const bool enable);
	bool AreRealTimeShadowsEnabled() const;
	void ForceAllSFXStop();

	void Update(
		const unsigned long lastFrameElapsedTime,
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
	int GetNumRenderedEntities();
	void ScaleEntities(const float scale, const bool scalePosition);

	ETHBucketManager& GetBucketManager();

	void GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace);

	unsigned int GetNumEntities() const;

	void RecoverResources();
	void ClearLightmaps();

	void SetZBuffer(const bool enable);
	bool GetZBuffer() const;

	ETHPhysicsSimulator& GetSimulator();

	void ResolveJoints();

	void AddEntityToPersistentList(ETHRenderEntity* entity);

private:
	bool LoadFromFile(const str_type::string& fileName);

	void Init(ETHResourceProviderPtr provider, const ETHSceneProperties& props, asIScriptModule *pModule, asIScriptContext *pContext);

	void MapEntitiesToBeRendered(
		float &maxHeight,
		float &minHeight,
		const ETHBackBufferTargetManagerPtr& backBuffer);

	void DrawEntityMultimap(const bool roundUp);

	bool AssignCallbackScript(ETHSpriteEntity* entity);

	void AssignControllerToEntity(
		ETHEntity* entity,
		asIScriptFunction* callback,
		asIScriptFunction* constructorCallback,
		asIScriptFunction* destructorCallback);

	bool DrawBucketOutlines();
	bool ReadFromXMLFile(TiXmlElement *pElement);

	void FillMultimapAndClearPersistenList(
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
	ETHEntityDestructorManagerPtr m_destructorManager;
	asIScriptModule *m_pModule;
	asIScriptContext *m_pContext;
	float m_maxSceneHeight, m_minSceneHeight;
	int m_idCounter;
	int m_nCurrentLights;
	int m_nRenderedEntities;
	bool m_enableZBuffer;
};

typedef boost::shared_ptr<ETHScene> ETHScenePtr;

#endif
