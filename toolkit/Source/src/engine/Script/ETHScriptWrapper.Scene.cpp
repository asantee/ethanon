#include "ETHScriptWrapper.h"

#include "../Shader/ETHShaderManager.h"

#include "../Entity/ETHRenderEntity.h"
#include "../Entity/ETHEntityChooser.h"

#include "../Resource/ETHDirectories.h"

ETHScriptWrapper::ETH_NEXT_SCENE::ETH_NEXT_SCENE()
{
	Reset();
}

void ETHScriptWrapper::ETH_NEXT_SCENE::Reset()
{
	sceneName = GS_L("");
	onSceneUpdateFunc = GS_L("");
	onSceneLoadedFunc = GS_L("");
	onResumeFunc = GS_L("");
	bucketSize = Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE);
}

bool ETHScriptWrapper::ETH_NEXT_SCENE::HasNextScene() const
{
	return (sceneName != GS_L(""));
}

str_type::string ETHScriptWrapper::ETH_NEXT_SCENE::GetSceneName() const
{
	return sceneName;
}

str_type::string ETHScriptWrapper::ETH_NEXT_SCENE::GetOnSceneLoadedFunc() const
{
	return onSceneLoadedFunc;
}

str_type::string ETHScriptWrapper::ETH_NEXT_SCENE::GetOnSceneUpdateFunc() const
{
	return onSceneUpdateFunc;
}

str_type::string ETHScriptWrapper::ETH_NEXT_SCENE::GetOnResumeFunc() const
{
	return onResumeFunc;
}

str_type::string ETHScriptWrapper::ETH_NEXT_SCENE::GetLightmapDirectory() const
{
	return lightmapDirectory;
}

Vector2 ETHScriptWrapper::ETH_NEXT_SCENE::GetBucketSize() const
{
	return bucketSize;
}

void ETHScriptWrapper::ETH_NEXT_SCENE::SetNextScene(
	const str_type::string &sceneName,
	const str_type::string &onSceneLoadedFunc,
	const str_type::string &onSceneUpdateFunc,
	const str_type::string& onResumeFunc,
	const str_type::string& lightmapDirectory,
	const Vector2 &bucketSize)
{
	this->sceneName = sceneName;
	this->onSceneLoadedFunc = onSceneLoadedFunc;
	this->onSceneUpdateFunc = onSceneUpdateFunc;
	this->onResumeFunc = onResumeFunc;
	this->bucketSize = bucketSize;
	this->lightmapDirectory = lightmapDirectory;
}

ETHEntity *ETHScriptWrapper::SeekEntity(const int id)
{
	if (WarnIfRunsInMainFunction(GS_L("SeekEntity")))
		return 0;

	ETHEntity *pEntity = m_pScene->GetBucketManager().SeekEntity(id);
	if (pEntity)
	{
		// don't let it return temporary handles
		if (pEntity->IsTemporary())
		{
			return 0;
		}
		else
		{
			pEntity->AddRef();
			return pEntity;
		}
	}
	return 0;
}

ETHEntity *ETHScriptWrapper::SeekEntity(const str_type::string &name)
{
	if (WarnIfRunsInMainFunction(GS_L("SeekEntity")))
		return 0;

	ETHEntity *pEntity = m_pScene->GetBucketManager().SeekEntity(name);
	if (pEntity)
	{
		// don't let it return temporary handles
		if (pEntity->IsTemporary())
		{
			return 0;
		}
		else
		{
			pEntity->AddRef();
			return pEntity;
		}
	}
	return 0;
}

int ETHScriptWrapper::AddEntity(
	const str_type::string &file,
	const Vector3 &v3Pos,
	const float angle,
	ETHEntity **ppOutEntity,
	const str_type::string &alternativeName,
	const float scale)
{
	if (WarnIfRunsInMainFunction(GS_L("AddEntity")))
		return -1;

	const str_type::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();

	const ETHEntityProperties* props = m_entityCache.Get(
		file,
		resourceDirectory + ETHDirectories::GetEntityDirectory(),
		m_provider->GetFileManager());

	if (!props)
	{
		return -1;
	}

	ETHRenderEntity* entity = new ETHRenderEntity(m_provider, *props, angle, scale);
	entity->SetOrphanPosition(v3Pos);
	entity->SetAngle(angle);

	if (ppOutEntity)
	{
		entity->AddRef();
		*ppOutEntity = entity;
	}

	return m_pScene->AddEntity(entity, alternativeName);
}

int ETHScriptWrapper::AddEntity(const str_type::string &file, const Vector3 &v3Pos, const str_type::string &alternativeName)
{
	return AddEntity(file, v3Pos, 0.0f, 0, alternativeName, 1.0f);
}

int ETHScriptWrapper::AddEntity(const str_type::string &file, const Vector3 &v3Pos, ETHEntity **ppOutEntity)
{
	return AddEntity(file, v3Pos, 0.0f, ppOutEntity, GS_L(""), 1.0f);
}

int ETHScriptWrapper::AddEntity(const str_type::string &file, const Vector3 &v3Pos, const float angle)
{
	return AddEntity(file, v3Pos, angle, 0, GS_L(""), 1.0f);
}

int ETHScriptWrapper::AddScaledEntity(const str_type::string &file, const Vector3 &v3Pos, const float scale)
{
	return AddEntity(file, v3Pos, 0.0f, 0, GS_L(""), scale);
}

int ETHScriptWrapper::AddScaledEntity(const str_type::string &file, const Vector3 &v3Pos, const float scale, ETHEntity **ppOutEntity)
{
	return AddEntity(file, v3Pos, 0.0f, ppOutEntity, GS_L(""), scale);
}

ETHEntity *ETHScriptWrapper::DeleteEntity(ETHEntity *pEntity)
{
	if (WarnIfRunsInMainFunction(GS_L("DeleteEntity")) || !pEntity)
		return 0;

	if (m_pScene->DeleteEntity(pEntity))
	{
		pEntity->Release();
	}
	return 0;
}

void ETHScriptWrapper::LoadLightmaps(const str_type::string& directory)
{
	if (m_usePreLoadedLightmapsFromFile)
	{
		ReadLightmapsFromBitmapFiles(directory);
	}
}

void ETHScriptWrapper::ReadLightmapsFromBitmapFiles(const str_type::string& directory)
{
	if (m_pScene)
	{
		const str_type::string resourceDirectory = (m_expansionFileManager) ? GS_L("") : GetResourceDirectory();
		const str_type::string lightmapDirectory = (directory.empty()) ? GetSceneFileName() : directory;
		
		Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
		Platform::FileManagerPtr currentFileManager     = fileIOHub->GetFileManager();
		const str_type::string currentResourceDirectory = fileIOHub->GetResourceDirectory();
		if (m_expansionFileManager)
		{
			fileIOHub->SetFileManager(m_expansionFileManager, resourceDirectory);
		}

		m_pScene->LoadLightmapsFromBitmapFiles(resourceDirectory + lightmapDirectory);
		
		fileIOHub->SetFileManager(currentFileManager, currentResourceDirectory);
	}
}

void ETHScriptWrapper::SetAmbientLight(const Vector3 &v3Color)
{
	if (WarnIfRunsInMainFunction(GS_L("SetAmbientLight")))
		return;

	m_pScene->SetAmbientLight(v3Color);
}

void ETHScriptWrapper::SetParallaxOrigin(const Vector2 &parallaxOrigin)
{
	// to-do
	//m_provider->GetShaderManager()->SetParallaxNormalizedOrigin(parallaxOrigin);
}

Vector2 ETHScriptWrapper::GetParallaxOrigin()
{
	return Vector2(0.5f); //m_provider->GetShaderManager()->GetParallaxNormalizedOrigin();
}

void ETHScriptWrapper::SetParallaxIntensity(const float intensity)
{
	m_provider->GetShaderManager()->SetParallaxIntensity(intensity);
}

float ETHScriptWrapper::GetParallaxIntensity()
{
	return m_provider->GetShaderManager()->GetParallaxIntensity();
}

void ETHScriptWrapper::SetBucketClearenceFactor(const float factor)
{
	if (WarnIfRunsInMainFunction(GS_L("SetBucketClearenceFactor")))
		return;
	m_pScene->SetBucketClearenceFactor(factor);
}

float ETHScriptWrapper::GetBucketClearenceFactor()
{
	if (WarnIfRunsInMainFunction(GS_L("GetBucketClearenceFactor")))
		return 0.0f;
	return m_pScene->GetBucketClearenceFactor();
}

Vector3 ETHScriptWrapper::GetAmbientLight()
{
	if (WarnIfRunsInMainFunction(GS_L("GetAmbientLight")))
		return Vector3(0,0,0);

	return m_pScene->GetAmbientLight();
}

void ETHScriptWrapper::SetCameraPos(const Vector2 &v2Pos)
{
	if (WarnIfRunsInMainFunction(GS_L("SetCameraPos")))
		return;

	// rounds up camera final position
	VideoPtr video = m_provider->GetVideo();
	video->SetCameraPos(v2Pos);
}

void ETHScriptWrapper::AddToCameraPos(const Vector2 &v2Add)
{
	if (WarnIfRunsInMainFunction(GS_L("AddToCameraPos")))
		return;

	// rounds up camera final position
	VideoPtr video = m_provider->GetVideo();
	video->MoveCamera(v2Add);
}

void ETHScriptWrapper::EnableLightmaps(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("EnableLightmaps")))
		return;

	m_useLightmaps = enable;
	m_pScene->EnableLightmaps(enable);
}

void ETHScriptWrapper::EnablePreLoadedLightmapsFromFile(const bool enable)
{
	m_usePreLoadedLightmapsFromFile = enable;
}

Vector2 ETHScriptWrapper::GetCameraPos()
{
	if (WarnIfRunsInMainFunction(GS_L("GetCameraPos")))
		return Vector2(0,0);

	return m_provider->GetVideo()->GetCameraPos();
}

unsigned int ETHScriptWrapper::GetNumEntities()
{
	if (WarnIfRunsInMainFunction(GS_L("GetNumEntities")))
		return 0;

	return m_pScene->GetNumEntities();
}

bool ETHScriptWrapper::AddFloatData(const str_type::string &entity, const str_type::string &name, const float value)
{
	if (WarnIfRunsInMainFunction(GS_L("AddFloatData")))
		return false;
	return m_pScene->AddFloatData(entity, name, value);
}

bool ETHScriptWrapper::AddIntData(const str_type::string &entity, const str_type::string &name, const int value)
{
	if (WarnIfRunsInMainFunction(GS_L("AddIntData")))
		return false;
	return m_pScene->AddIntData(entity, name, value);
}

bool ETHScriptWrapper::AddUIntData(const str_type::string &entity, const str_type::string &name, const unsigned int value)
{
	if (WarnIfRunsInMainFunction(GS_L("AddUIntData")))
		return false;
	return m_pScene->AddUIntData(entity, name, value);
}

bool ETHScriptWrapper::AddStringData(const str_type::string &entity, const str_type::string &name, const str_type::string &value)
{
	if (WarnIfRunsInMainFunction(GS_L("AddStringData")))
		return false;
	return m_pScene->AddStringData(entity, name, value);
}

bool ETHScriptWrapper::AddVector2Data(const str_type::string &entity, const str_type::string &name, const Vector2 &value)
{
	if (WarnIfRunsInMainFunction(GS_L("AddVector2Data")))
		return false;
	return m_pScene->AddVector2Data(entity, name, value);
}

bool ETHScriptWrapper::AddVector3Data(const str_type::string &entity, const str_type::string &name, const Vector3 &value)
{
	if (WarnIfRunsInMainFunction(GS_L("AddVector3Data")))
		return false;
	return m_pScene->AddVector3Data(entity, name, value);
}

uint32_t ETHScriptWrapper::GetBackgroundColor()
{
	return m_provider->GetVideo()->GetBGColor().To32BitARGB();
}

void ETHScriptWrapper::SetBackgroundColor(const uint32_t color)
{
	m_provider->GetVideo()->SetBGColor(Color(color));
}

void ETHScriptWrapper::GetEntityArrayByName(const str_type::string &name, ETHEntityArray& outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntityArrayByName")))
		return;

	m_pScene->GetBucketManager().GetEntityArrayByName(name, outVector);
}

void ETHScriptWrapper::GetEntityArrayFromBucket(const Vector2 &v2Bucket, ETHEntityArray& outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntityArrayFromBucket")))
		return;

	m_pScene->GetBucketManager().GetEntityArrayFromBucket(v2Bucket, outVector);
}

void ETHScriptWrapper::GetAllEntitiesInScene(ETHEntityArray& outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetAllEntitiesInScene")))
		return;
	m_pScene->GetBucketManager().GetEntityArray(outVector);
}

void ETHScriptWrapper::GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray& outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntitiesAroundBucket")))
		return;
	m_pScene->GetBucketManager().GetEntitiesAroundBucket(bucket, outVector);
}

void ETHScriptWrapper::GetEntitiesAroundEntity(ETHEntity* entity, ETHEntityArray& outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntitiesAroundEntity")))
		return;
	ETHBucketManager& bucketManager = m_pScene->GetBucketManager();
	bucketManager.GetEntitiesAroundBucket(
		entity->GetCurrentBucket(bucketManager),
		outVector,
		ETHEntitySingleExceptionChooser(entity->GetID()));
}

void ETHScriptWrapper::GetWhiteListedEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray& outVector, const str_type::string& semicolonSeparatedNames)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntitiesAroundBucket")))
		return;
	m_pScene->GetBucketManager().GetWhiteListedEntitiesAroundBucket(bucket, outVector, semicolonSeparatedNames);
}

void ETHScriptWrapper::GetEntitiesAroundBucketWithBlackList(const Vector2& bucket, ETHEntityArray& outVector, const str_type::string& semicolonSeparatedNames)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntitiesAroundBucketWithBlackList")))
		return;
	m_pScene->GetBucketManager().GetEntitiesAroundBucketWithBlackList(bucket, outVector, semicolonSeparatedNames);
}

Vector2 ETHScriptWrapper::GetCurrentBucket(ETHEntity *pEntity)
{
	return pEntity->GetCurrentBucket(m_pScene->GetBucketManager());
}

void ETHScriptWrapper::SetBorderBucketsDrawing(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("SetBucketBorderDrawing")))
		return;
	m_pScene->SetBorderBucketsDrawing(enable);
}

bool ETHScriptWrapper::IsDrawingBorderBuckets()
{
	if (WarnIfRunsInMainFunction(GS_L("IsDrawingBorderBuckets")))
		return false;
	return m_pScene->IsDrawingBorderBuckets();
}

void ETHScriptWrapper::SetPositionRoundUp(const bool roundUp)
{
	m_roundUpPosition = roundUp;
}

bool ETHScriptWrapper::GetPositionRoundUp()
{
	return m_roundUpPosition;
}

void ETHScriptWrapper::SetZBuffer(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("SetZBuffer")))
		return;
	m_pScene->SetZBuffer(enable);
}

bool ETHScriptWrapper::GetZBuffer()
{
	if (WarnIfRunsInMainFunction(GS_L("GetZBuffer")))
		return false;
	return m_pScene->GetZBuffer();
}

void ETHScriptWrapper::EnableRealTimeShadows(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("EnableRealTimeShadows")))
		return;
	m_pScene->EnableRealTimeShadows(enable);
}

void ETHScriptWrapper::GetVisibleEntities(ETHEntityArray &entityArray)
{
	if (WarnIfRunsInMainFunction(GS_L("GetVisibleEntities")))
		return;
	m_pScene->GetBucketManager().GetVisibleEntities(entityArray);
}

void ETHScriptWrapper::GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray& outVector, const bool screenSpace)
{
	if (WarnIfRunsInMainFunction(GS_L("GetIntersectingEntities")))
		return;
	m_pScene->GetIntersectingEntities(v2Here, outVector, screenSpace);
}

int ETHScriptWrapper::GetNumProcessedEntities()
{
	if (WarnIfRunsInMainFunction(GS_L("GetNumProcessedEntities")))
		return 0;
	return m_pScene->GetNumProcessedEntities();
}

int ETHScriptWrapper::GetNumRenderedPieces()
{
	if (WarnIfRunsInMainFunction(GS_L("GetNumRenderedPieces")))
		return 0;
	return m_pScene->GetNumRenderedPieces();
}

bool ETHScriptWrapper::SaveScene(const str_type::string &escFile)
{
	if (WarnIfRunsInMainFunction(GS_L("SaveScene")))
		return false;

	// loads a new scene from file
	str_type::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory();
	fileName += escFile;

	return m_pScene->SaveToFile(fileName, m_entityCache);
}

void ETHScriptWrapper::ReleaseResources()
{
	m_provider->GetAudioResourceManager()->ReleaseResources();
	m_provider->GetGraphicResourceManager()->ReleaseResources();
}

void ETHScriptWrapper::ResolveJoints()
{
	if (WarnIfRunsInMainFunction(GS_L("ResolveJoints")))
		return;
	if (m_pScene)
		m_pScene->ResolveJoints();
}

bool ETHScriptWrapper::LoadScene(const str_type::string &escFile, const str_type::string& lightmapDirectory, const Vector2& bucketSize)
{
	if (!ArePersistentResourcesEnabled())
	{
		if (escFile != m_sceneFileName)
		{
			ReleaseResources();
		}
	}

	m_provider->GetGraphicResourceManager()->ReleaseTemporaryResources();

	str_type::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory();
	fileName += escFile;

	// if the name is set to _ETH_EMPTY_SCENE_STRING, don't load anything
	if (escFile != _ETH_EMPTY_SCENE_STRING && escFile.size() > 0)
	{
		m_pScene = ETHScenePtr(
			new ETHScene(
				fileName,
				m_provider,
				ETHSceneProperties(),
				m_pASModule,
				m_pScriptContext,
				m_entityCache,
				bucketSize));
	}
	else
	{
		m_pScene = ETHScenePtr(new ETHScene(m_provider, ETHSceneProperties(), m_pASModule, m_pScriptContext, bucketSize));
	}

	ETHEntityArray entities;
	m_pScene->GetBucketManager().GetEntityArray(entities);
	m_pScene->ResolveJoints();
	m_drawableManager.Clear();
	m_sceneFileName = escFile;
	m_pScene->EnableLightmaps(m_useLightmaps);
	LoadLightmaps(lightmapDirectory);
	m_provider->GetVideo()->SetCameraPos(Vector2(0,0));
	LoadSceneScripts();
	m_timer.CalcLastFrame();
	return true;
}

void ETHScriptWrapper::AddSceneInScript(const str_type::string& escFile, const Vector3& offset, ETHEntityArray &outVector)
{
	if (!m_pScene) return;
	const str_type::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory() + escFile;
	m_pScene->AddSceneFromFile(fileName, m_entityCache, m_pScene->AssembleEntityPath(), false /*readSceneProperties*/, offset, outVector, true);
}

void ETHScriptWrapper::AddSceneFromString(const str_type::string& content, const Vector3& offset, ETHEntityArray &outVector)
{
	if (!m_pScene) return;
	m_pScene->AddSceneFromString("xmlContent", content, m_entityCache, m_pScene->AssembleEntityPath(), false /*readSceneProperties*/, offset, outVector, true);
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile)
{
	LoadSceneInScript(
		escFile,
		GS_L(""),
		GS_L(""),
		GS_L(""),
		Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(
	const str_type::string &escFile,
	const str_type::string &onSceneLoadedFunc,
	const str_type::string &onSceneUpdateFunc,
	const str_type::string &onResumeFunc)
{
	LoadSceneInScript(
		escFile,
		onSceneLoadedFunc,
		onSceneUpdateFunc,
		onResumeFunc,
		Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc, const str_type::string &onSceneUpdateFunc)
{
	LoadSceneInScript(
		escFile,
		onSceneLoadedFunc,
		onSceneUpdateFunc,
		GS_L(""),
		Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(
	const str_type::string &escFile,
	const str_type::string &onSceneLoadedFunc,
	const str_type::string &onSceneUpdateFunc,
	const Vector2 &v2BucketSize)
{
	LoadSceneInScript(
		escFile,
		onSceneLoadedFunc,
		onSceneUpdateFunc,
		GS_L(""),
		v2BucketSize);
}

void ETHScriptWrapper::LoadSceneInScript(
	const str_type::string &escFile,
	const str_type::string &onSceneLoadedFunc,
	const str_type::string &onSceneUpdateFunc,
	const str_type::string &onResumeFunc,
	const Vector2 &v2BucketSize)
{
	LoadSceneInScript(
		escFile,
		onSceneLoadedFunc,
		onSceneUpdateFunc,
		onResumeFunc,
		GS_L(""),
		v2BucketSize);
}

void ETHScriptWrapper::LoadSceneInScript(
	const str_type::string &escFile,
	const str_type::string &onSceneLoadedFunc,
	const str_type::string &onSceneUpdateFunc,
	const str_type::string &onResumeFunc,
	const str_type::string &lightmapDirectory,
	const Vector2 &v2BucketSize)
{
	const str_type::string& sceneName((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile);
	
	m_nextScene.SetNextScene(
		sceneName,
		onSceneLoadedFunc,
		onSceneUpdateFunc,
		onResumeFunc,
		lightmapDirectory,
		v2BucketSize);
}

str_type::string ETHScriptWrapper::GetSceneFileName()
{
	if (WarnIfRunsInMainFunction(GS_L("GetSceneFileName")))
		return GS_L("");

	return m_sceneFileName;
}

Vector2 ETHScriptWrapper::GetLastCameraPos()
{
	return m_v2LastCamPos;
}

void ETHScriptWrapper::LoadSceneScripts()
{
	asIScriptFunction* onSceneLoadedFunc = m_pASModule->GetFunctionByName(m_nextScene.GetOnSceneLoadedFunc().c_str());
	if (onSceneLoadedFunc)
	{
		ETHGlobal::ExecuteContext(m_pScriptContext, onSceneLoadedFunc);
	}

	m_onSceneUpdateFunction = m_pASModule->GetFunctionByName(m_nextScene.GetOnSceneUpdateFunc().c_str());
	m_onResumeFunction      = m_pASModule->GetFunctionByName(m_nextScene.GetOnResumeFunc().c_str());
}

void ETHScriptWrapper::SetGravity(const Vector2& gravity)
{
	if (WarnIfRunsInMainFunction(GS_L("SetGravity")))
		return;
	m_pScene->GetSimulator().SetGravity(gravity);
}

Vector2 ETHScriptWrapper::GetGravity()
{
	if (WarnIfRunsInMainFunction(GS_L("GetGravity")))
		return Vector2(0,0);
	return m_pScene->GetSimulator().GetGravity();
}

void ETHScriptWrapper::SetNumIterations(const int velocityIterations, const int positionIterations)
{
	if (WarnIfRunsInMainFunction(GS_L("SetNumIterations")))
		return;
	m_pScene->GetSimulator().SetNumIterations(velocityIterations, positionIterations);
}

void ETHScriptWrapper::GetNumIterations(int& velocityIterations, int& positionIterations)
{
	if (WarnIfRunsInMainFunction(GS_L("GetNumIterations")))
		return;
	m_pScene->GetSimulator().GetNumIterations(velocityIterations, positionIterations);
}

void ETHScriptWrapper::SetTimeStepScale(const float scale)
{
	if (WarnIfRunsInMainFunction(GS_L("SetTimeStepScale")))
		return;
	m_pScene->GetSimulator().SetTimeStepScale(scale);
}

float ETHScriptWrapper::GetTimeStepScale()
{
	if (WarnIfRunsInMainFunction(GS_L("GetTimeStepScale")))
		return 0.0f;
	return m_pScene->GetSimulator().GetTimeStepScale();
}

ETHEntity* ETHScriptWrapper::GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal)
{
	if (WarnIfRunsInMainFunction(GS_L("GetClosestContact")))
		return 0;
	return m_pScene->GetSimulator().GetClosestContact(a, b, point, normal);
}

ETHEntity* ETHScriptWrapper::GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point,
											   Vector2& normal, const str_type::string& semicolonSeparatedIgnoreList)
{
	if (WarnIfRunsInMainFunction(GS_L("GetClosestContact")))
		return 0;
	return m_pScene->GetSimulator().GetClosestContact(a, b, point, normal, semicolonSeparatedIgnoreList);
}

bool ETHScriptWrapper::GetContactEntities(const Vector2& a, const Vector2& b, ETHEntityArray& entities)
{
	if (WarnIfRunsInMainFunction(GS_L("GetContactEntities")))
		return false;
	return m_pScene->GetSimulator().GetContactEntities(a, b, entities);
}

bool ETHScriptWrapper::IsFixedTimeStep()
{
	if (WarnIfRunsInMainFunction(GS_L("IsFixedTimeStep")))
		return false;
	return m_pScene->GetSimulator().IsFixedTimeStep();
}

float ETHScriptWrapper::GetFixedTimeStepValue()
{
	if (WarnIfRunsInMainFunction(GS_L("GetFixedTimeStepValue")))
		return 0.0f;
	return m_pScene->GetSimulator().GetFixedTimeStepValue();
}

void ETHScriptWrapper::SetFixedTimeStep(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("SetFixedTimeStep")))
		return;
	m_pScene->GetSimulator().SetFixedTimeStep(enable);
}

void ETHScriptWrapper::SetFixedTimeStepValue(const float value)
{
	if (WarnIfRunsInMainFunction(GS_L("SetFixedTimeStepValue")))
		return;
	m_pScene->GetSimulator().SetFixedTimeStepValue(value);
}

float ETHScriptWrapper::GetCurrentPhysicsTimeStepMS()
{
	if (WarnIfRunsInMainFunction(GS_L("GetCurrentPhysicsTimeStepMS")))
		return 0.0f;
	return m_pScene->GetSimulator().GetCurrentDynamicTimeStepMS();
}

void ETHScriptWrapper::DisableContact()
{
	if (WarnIfRunsInMainFunction(GS_L("DisableContact")))
		return;
	ETHPhysicsSimulator& simulator = m_pScene->GetSimulator();
	if (!simulator.IsRunningPreSolveContactCallback())
		m_provider->Log(GS_L("DisableContact function must be called inside " + ETHPhysicsEntityController::PRESOLVE_CONTACT_CALLBACK_PREFIX), Platform::Logger::ERROR);
	simulator.DisableNextContact();	
}

void ETHScriptWrapper::SetFixedHeight(const float height)
{
	m_provider->GetVideo()->SetVirtualScreenHeight(height);
}

float ETHScriptWrapper::GetDummyScale()
{
	return 1.0f;
}

float ETHScriptWrapper::DummyScale(const float v)
{
	return v;
}

Vector2 ETHScriptWrapper::DummyScale(const Vector2& v)
{
	return v;
}

Vector3 ETHScriptWrapper::DummyScale(const Vector3& v)
{
	return v;
}

void ETHScriptWrapper::SetZAxisDirection(const Vector2& dir)
{
	if (WarnIfRunsInMainFunction(GS_L("SetZAxisDirection")) || !m_pScene)
		return;
	m_pScene->SetZAxisDirection(dir);
}

Vector2 ETHScriptWrapper::GetZAxisDirection()
{
	if (WarnIfRunsInMainFunction(GS_L("GetZAxisDirection")) || !m_pScene)
		return gs2d::math::constant::ZERO_VECTOR2;
	return m_pScene->GetZAxisDirection();
}

void ETHScriptWrapper::ForceEntityRendering(ETHEntity* entity)
{
	if (WarnIfRunsInMainFunction(GS_L("ForceEntityRendering")) || !m_pScene)
		return;
	m_pScene->AddEntityToPersistentList(static_cast<ETHRenderEntity*>(entity));
}
