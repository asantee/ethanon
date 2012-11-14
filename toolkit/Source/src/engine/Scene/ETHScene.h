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

#include "../Entity/ETHEntityArray.h"
#include "ETHBucketManager.h"
#include "ETHSceneProperties.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Util/ETHASUtil.h"
#include "../Scene/ETHTempEntityHandler.h"
#include "../Physics/ETHPhysicsSimulator.h"
#include "../Script/ETHEntityDestructorManager.h"

class ETHScene
{
public:
	ETHScene(
		const str_type::string& fileName,
		ETHResourceProviderPtr provider,
		const bool richLighting,
		const ETHSceneProperties& props,
		asIScriptModule *pModule,
		asIScriptContext *pContext,
		const bool isInEditor,
		const Vector2 &v2BucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));

	ETHScene(
		ETHResourceProviderPtr provider,
		const bool richLighting,
		const ETHSceneProperties& props,
		asIScriptModule *pModule,
		asIScriptContext *pContext,
		const bool isInEditor,
		const Vector2 &v2BucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));

	~ETHScene();

	void ClearResources();

	void RenderScene(
		const bool roundUp,
		const unsigned long lastFrameElapsedTime,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		SpritePtr pOutline = SpritePtr(),
		SpritePtr pInvisibleEntSymbol = SpritePtr());

	bool SaveToFile(const str_type::string& fileName);
	int AddEntity(ETHRenderEntity* pEntity);
	int AddEntity(ETHRenderEntity* pEntity, const str_type::string& alternativeName);
	void SetSceneProperties(const ETHSceneProperties &prop);
	void AddLight(const ETHLight &light);
	void AddLight(const ETHLight &light, const Vector3& v3Pos, const Vector2& scale);
	bool GenerateLightmaps(const int id = -1);
	void EnableLightmaps(const bool enable);
	bool AreLightmapsEnabled() const;
	void EnableRealTimeShadows(const bool enable);
	bool AreRealTimeShadowsEnabled() const;
	void ForceAllSFXStop();
	void Update(const unsigned long lastFrameElapsedTime);
	void UpdateTemporary(const unsigned long lastFrameElapsedTime);
	bool DeleteEntity(ETHEntity *pEntity);

	const ETHSceneProperties* GetSceneProperties() const;
	ETHSceneProperties* GetEditableSceneProperties();

	bool AddFloatData(const str_type::string &entity, const str_type::string &name, const float value);
	bool AddIntData(const str_type::string &entity, const str_type::string &name, const int value);
	bool AddUIntData(const str_type::string &entity, const str_type::string &name, const unsigned int value);
	bool AddStringData(const str_type::string &entity, const str_type::string &name, const str_type::string &value);
	bool AddVector2Data(const str_type::string &entity, const str_type::string &name, const Vector2 &value);
	bool AddVector3Data(const str_type::string &entity, const str_type::string &name, const Vector3 &value);
	bool AddCustomData(const str_type::string &entity, const str_type::string &name, const ETHCustomDataConstPtr &inData);

	int GetNumLights();
	int CountLights();

	void ShowLightmaps(const bool show);
	bool AreLightmapsShown();

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
	void SetHaloRotation(const bool enable);
	bool GetHaloRotation() const;
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
	const bool m_isInEditor;

	bool LoadFromFile(const str_type::string& fileName);

	void Init(ETHResourceProviderPtr provider, const ETHSceneProperties& props, asIScriptModule *pModule, asIScriptContext *pContext);

	void RunCallbacksFromList();
	bool RenderTransparentLayer(std::list<ETHRenderEntity*> &halos);

	void MapEntitiesToBeRendered(
		std::multimap<float, ETHRenderEntity*>& mmEntities,
		float &maxHeight,
		float &minHeight,
		const ETHBackBufferTargetManagerPtr& backBuffer);

	void DrawEntityMultimap(
		std::multimap<float, ETHRenderEntity*>& mmEntities,
		float &minHeight,
		float &maxHeight,
		const unsigned long lastFrameElapsedTime,
		const ETHBackBufferTargetManagerPtr& backBuffer,
		SpritePtr pOutline,
		const bool roundUp,
		SpritePtr pInvisibleEntSymbol,
		std::list<ETHRenderEntity*> &outHalos,
		std::list<ETHRenderEntity*> &outParticles);

	void ReleaseMappedEntities(std::multimap<float, ETHRenderEntity*>& mmEntities);



	bool RenderParticleList(std::list<ETHRenderEntity*> &particles);
	bool AssignCallbackScript(ETHSpriteEntity* entity);
	void AssignControllerToEntity(ETHEntity* entity, const int callbackId, const int constructorCallbackId, const int destructorCallbackId);
	bool DrawBucketOutlines();
	bool ReadFromXMLFile(TiXmlElement *pElement);

	void FillMultimapAndClearPersistenList(std::multimap<float, ETHRenderEntity*>& mm, const std::list<Vector2>& currentBucketList);

	float ComputeDrawHash(const float entityDepth, const ETHEntityProperties::ENTITY_TYPE& type) const;
	ETHBucketManager m_buckets;
	ETHTempEntityHandler m_tempEntities;

	std::list<ETHLight> m_lights;
	std::list<ETHRenderEntity*> m_persistentEntities;

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
	bool m_enableLightmaps;
	bool m_showingLightmaps;
	bool m_rotatingHalos;
	bool m_usingRTShadows;
	bool m_richLighting;
	bool m_enableZBuffer;
};

typedef boost::shared_ptr<ETHScene> ETHScenePtr;

#endif