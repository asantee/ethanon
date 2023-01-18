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
	sceneName = ("");
	onSceneUpdateFunc = ("");
	onSceneCreatedFunc = ("");
	onResumeFunc = ("");
	bucketSize = Vector2(ETH_DEFAULT_BUCKET_SIZE, ETH_DEFAULT_BUCKET_SIZE);
}

bool ETHScriptWrapper::ETH_NEXT_SCENE::HasNextScene() const
{
	return (sceneName != (""));
}

std::string ETHScriptWrapper::ETH_NEXT_SCENE::GetSceneName() const
{
	return sceneName;
}

std::string ETHScriptWrapper::ETH_NEXT_SCENE::GetOnSceneCreatedFunc() const
{
	return onSceneCreatedFunc;
}

std::string ETHScriptWrapper::ETH_NEXT_SCENE::GetOnSceneUpdateFunc() const
{
	return onSceneUpdateFunc;
}

std::string ETHScriptWrapper::ETH_NEXT_SCENE::GetOnResumeFunc() const
{
	return onResumeFunc;
}

std::string ETHScriptWrapper::ETH_NEXT_SCENE::GetLightmapDirectory() const
{
	return lightmapDirectory;
}

Vector2 ETHScriptWrapper::ETH_NEXT_SCENE::GetBucketSize() const
{
	return bucketSize;
}

void ETHScriptWrapper::ETH_NEXT_SCENE::SetNextScene(
	const std::string &sceneName,
	const std::string &onSceneCreatedFunc,
	const std::string &onSceneUpdateFunc,
	const std::string& onResumeFunc,
	const std::string& lightmapDirectory,
	const Vector2 &bucketSize)
{
	this->sceneName = sceneName;
	this->onSceneCreatedFunc = onSceneCreatedFunc;
	this->onSceneUpdateFunc = onSceneUpdateFunc;
	this->onResumeFunc = onResumeFunc;
	this->bucketSize = bucketSize;
	this->lightmapDirectory = lightmapDirectory;
}

ETHEntity *ETHScriptWrapper::SeekEntity(const int id)
{
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

ETHEntity *ETHScriptWrapper::SeekEntity(const std::string &name)
{
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
	const std::string &file,
	const Vector3 &v3Pos,
	const float angle,
	ETHEntity **ppOutEntity,
	const std::string &alternativeName,
	const float scale)
{
	const std::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();

	const ETHEntityProperties* props = m_entityCache.Get(
		file,
		resourceDirectory + ETHDirectories::GetEntityDirectory(),
		m_provider->GetFileManager());

	if (!props)
	{
		return -1;
	}

	ETHRenderEntity* entity = new ETHRenderEntity(m_provider, *props, m_pScene->GetLightmapDirectory(), angle, scale, true /*immediatelyLoadSprites*/);
	entity->SetOrphanPosition(v3Pos);
	entity->SetAngle(angle);

	if (ppOutEntity)
	{
		entity->AddRef();
		*ppOutEntity = entity;
	}

	return m_pScene->AddEntity(entity, alternativeName);
}

int ETHScriptWrapper::AddEntity(const std::string &file, const Vector3 &v3Pos, const std::string &alternativeName)
{
	return AddEntity(file, v3Pos, 0.0f, 0, alternativeName, 1.0f);
}

int ETHScriptWrapper::AddEntity(const std::string &file, const Vector3 &v3Pos, ETHEntity **ppOutEntity)
{
	return AddEntity(file, v3Pos, 0.0f, ppOutEntity, (""), 1.0f);
}

int ETHScriptWrapper::AddEntity(const std::string &file, const Vector3 &v3Pos, const float angle)
{
	return AddEntity(file, v3Pos, angle, 0, (""), 1.0f);
}

int ETHScriptWrapper::AddScaledEntity(const std::string &file, const Vector3 &v3Pos, const float scale)
{
	return AddEntity(file, v3Pos, 0.0f, 0, (""), scale);
}

int ETHScriptWrapper::AddScaledEntity(const std::string &file, const Vector3 &v3Pos, const float scale, ETHEntity **ppOutEntity)
{
	return AddEntity(file, v3Pos, 0.0f, ppOutEntity, (""), scale);
}

void ETHScriptWrapper::DeleteEntity(ETHEntity *pEntity)
{
	m_pScene->DeleteEntity(pEntity);
	pEntity->Release();
}

void ETHScriptWrapper::LoadLightmaps(const std::string& directory)
{
	if (m_pScene)
	{
		const std::string lightmapDirectory = ETHScene::ConvertSceneFileNameToLightmapDirectory((directory.empty()) ? GetSceneFileName() : directory);
		m_pScene->LoadLightmapsFromBitmapFiles(GetResourceDirectory() + lightmapDirectory);
	}
}

void ETHScriptWrapper::SetAmbientLight(const Vector3 &v3Color)
{
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
	if (m_pScene)
	{
		m_pScene->SetParallaxIntensity(intensity);
	}
}

float ETHScriptWrapper::GetParallaxIntensity()
{
	if (m_pScene)
	{
		return m_pScene->GetParallaxIntensity();
	}
	else
	{
		return 0.0f;
	}
}

void ETHScriptWrapper::SetBucketClearenceFactor(const float factor)
{
	WarnIfRunsInMainFunction("SetBucketClearenceFactor");
	m_pScene->SetBucketClearenceFactor(factor);
}

float ETHScriptWrapper::GetBucketClearenceFactor()
{
	WarnIfRunsInMainFunction("GetBucketClearenceFactor");
	return m_pScene->GetBucketClearenceFactor();
}

Vector3 ETHScriptWrapper::GetAmbientLight()
{
	return m_pScene->GetAmbientLight();
}

void ETHScriptWrapper::SetCameraPos(const Vector2 &v2Pos)
{
	// rounds up camera final position
	VideoPtr video = m_provider->GetVideo();
	video->SetCameraPos(v2Pos);
}

void ETHScriptWrapper::AddToCameraPos(const Vector2 &v2Add)
{
	// rounds up camera final position
	VideoPtr video = m_provider->GetVideo();
	video->MoveCamera(v2Add);
}

Vector2 ETHScriptWrapper::GetCameraPos()
{
	return m_provider->GetVideo()->GetCameraPos();
}

unsigned int ETHScriptWrapper::GetNumEntities()
{
	WarnIfRunsInMainFunction(("GetNumEntities"));
	return m_pScene->GetNumEntities();
}

bool ETHScriptWrapper::AddFloatData(const std::string &entity, const std::string &name, const float value)
{
	return m_pScene->AddFloatData(entity, name, value);
}

bool ETHScriptWrapper::AddIntData(const std::string &entity, const std::string &name, const int value)
{
	return m_pScene->AddIntData(entity, name, value);
}

bool ETHScriptWrapper::AddUIntData(const std::string &entity, const std::string &name, const unsigned int value)
{
	return m_pScene->AddUIntData(entity, name, value);
}

bool ETHScriptWrapper::AddStringData(const std::string &entity, const std::string &name, const std::string &value)
{
	return m_pScene->AddStringData(entity, name, value);
}

bool ETHScriptWrapper::AddVector2Data(const std::string &entity, const std::string &name, const Vector2 &value)
{
	return m_pScene->AddVector2Data(entity, name, value);
}

bool ETHScriptWrapper::AddVector3Data(const std::string &entity, const std::string &name, const Vector3 &value)
{
	return m_pScene->AddVector3Data(entity, name, value);
}

uint32_t ETHScriptWrapper::GetBackgroundColor()
{
	return m_provider->GetVideo()->GetBackgroundColor().To32BitARGB();
}

void ETHScriptWrapper::SetBackgroundColor(const uint32_t color)
{
	m_provider->GetVideo()->SetBackgroundColor(Color(color));
}

void ETHScriptWrapper::GetEntityArrayByName(const std::string &name, ETHEntityArray& outVector)
{
	m_pScene->GetBucketManager().GetEntityArrayByName(name, outVector);
}

void ETHScriptWrapper::GetEntityArrayFromBucket(const Vector2 &v2Bucket, ETHEntityArray& outVector)
{
	m_pScene->GetBucketManager().GetEntityArrayFromBucket(v2Bucket, outVector);
}

void ETHScriptWrapper::GetEntitiesFromBucket(const Vector2 &v2Bucket, ETHEntityArray& outVector, const std::string& semicolonSeparatedNames)
{
	m_pScene->GetBucketManager().GetWhiteListedEntityArrayFromBucket(v2Bucket, outVector, semicolonSeparatedNames);
}

void ETHScriptWrapper::GetAllEntitiesInScene(ETHEntityArray& outVector)
{
	m_pScene->GetBucketManager().GetEntityArray(outVector);
}

void ETHScriptWrapper::GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray& outVector)
{
	m_pScene->GetBucketManager().GetEntitiesAroundBucket(bucket, outVector);
}

void ETHScriptWrapper::GetEntitiesAroundEntity(ETHEntity* entity, ETHEntityArray& outVector)
{
	ETHBucketManager& bucketManager = m_pScene->GetBucketManager();
	bucketManager.GetEntitiesAroundBucket(
		entity->GetCurrentBucket(bucketManager),
		outVector,
		ETHEntitySingleExceptionChooser(entity->GetID()));

	entity->Release();
}

void ETHScriptWrapper::GetWhiteListedEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray& outVector, const std::string& semicolonSeparatedNames)
{
	m_pScene->GetBucketManager().GetWhiteListedEntitiesAroundBucket(bucket, outVector, semicolonSeparatedNames);
}

void ETHScriptWrapper::GetEntitiesAroundBucketWithBlackList(const Vector2& bucket, ETHEntityArray& outVector, const std::string& semicolonSeparatedNames)
{
	m_pScene->GetBucketManager().GetEntitiesAroundBucketWithBlackList(bucket, outVector, semicolonSeparatedNames);
}

Vector2 ETHScriptWrapper::GetCurrentBucket(ETHEntity *pEntity)
{
	return pEntity->GetCurrentBucket(m_pScene->GetBucketManager());
}

void ETHScriptWrapper::SetBorderBucketsDrawing(const bool enable)
{
	WarnIfRunsInMainFunction("SetBucketBorderDrawing");
	m_pScene->SetBorderBucketsDrawing(enable);
}

bool ETHScriptWrapper::IsDrawingBorderBuckets()
{
	WarnIfRunsInMainFunction("IsDrawingBorderBuckets");
	return m_pScene->IsDrawingBorderBuckets();
}

void ETHScriptWrapper::GetVisibleEntities(ETHEntityArray &entityArray)
{
	WarnIfRunsInMainFunction("GetVisibleEntities");
	m_pScene->GetBucketManager().GetVisibleEntities(entityArray);
}

void ETHScriptWrapper::GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray& outVector, const bool screenSpace)
{
	m_pScene->GetIntersectingEntities(v2Here, outVector, screenSpace);
}

int ETHScriptWrapper::GetNumProcessedEntities()
{
	WarnIfRunsInMainFunction("GetNumProcessedEntities");
	return m_pScene->GetNumProcessedEntities();
}

int ETHScriptWrapper::GetNumRenderedPieces()
{
	WarnIfRunsInMainFunction("GetNumRenderedPieces");
	return m_pScene->GetNumRenderedPieces();
}

bool ETHScriptWrapper::SaveScene(const std::string &escFile)
{
	WarnIfRunsInMainFunction("SaveScene");

	// loads a new scene from file
	std::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory();
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
	WarnIfRunsInMainFunction("ResolveJoints");

	if (m_pScene)
		m_pScene->ResolveJoints();
}

bool ETHScriptWrapper::LoadScene(const std::string &escFile, const std::string& lightmapDirectory, const Vector2& bucketSize)
{
	if (!ArePersistentResourcesEnabled())
	{
		if (escFile != m_sceneFileName)
		{
			ReleaseResources();
		}
	}

	m_provider->GetGraphicResourceManager()->ReleaseTemporaryResources();

	const std::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();

	std::string lightmapDirectoryFinal;
	if (lightmapDirectory == "")
	{
		lightmapDirectoryFinal = ETHScene::ConvertSceneFileNameToLightmapDirectory(escFile);
	}
	else
	{
		lightmapDirectoryFinal = ETHScene::ConvertSceneFileNameToLightmapDirectory(lightmapDirectory);
	}

	lightmapDirectoryFinal = resourceDirectory + lightmapDirectoryFinal;
	
	std::string fileName = resourceDirectory + escFile;

	// if the name is set to _ETH_EMPTY_SCENE_STRING, don't load anything
	if (escFile != ETH_EMPTY_SCENE_STRING && escFile.size() > 0)
	{
		m_pScene = ETHScenePtr(
			new ETHScene(
				fileName,
				lightmapDirectoryFinal,
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

	m_provider->GetVideo()->SetCameraPos(Vector2(0,0));

	// load scripts
	m_onSceneCreatedFunction = m_pASModule->GetFunctionByName(m_nextScene.GetOnSceneCreatedFunc().c_str());
	m_onSceneUpdateFunction  = m_pASModule->GetFunctionByName(m_nextScene.GetOnSceneUpdateFunc().c_str());
	m_onResumeFunction       = m_pASModule->GetFunctionByName(m_nextScene.GetOnResumeFunc().c_str());

	return true;
}

void ETHScriptWrapper::AddSceneInScript(const std::string& escFile, const Vector3& offset, ETHEntityArray &outVector, const bool immediatelyLoadSprites)
{
	if (!m_pScene) return;
	const std::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory() + escFile;
	m_pScene->AddSceneFromFile(fileName, m_entityCache, m_pScene->AssembleEntityPath(), false /*readSceneProperties*/, offset, outVector, true /*shouldGenerateNewIDs*/, immediatelyLoadSprites);
}

void ETHScriptWrapper::AddSceneFromString(const std::string& content, const Vector3& offset, ETHEntityArray &outVector, const bool immediatelyLoadSprites)
{
	if (!m_pScene) return;
	m_pScene->AddSceneFromString("xmlContent", content, m_entityCache, m_pScene->AssembleEntityPath(), false /*readSceneProperties*/, offset, outVector, true /*shouldGenerateNewIDs*/, immediatelyLoadSprites);
}

void ETHScriptWrapper::LoadSceneInScript(const std::string &escFile)
{
	LoadSceneInScript(
		escFile,
		(""),
		(""),
		(""),
		Vector2(ETH_DEFAULT_BUCKET_SIZE, ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(
	const std::string &escFile,
	const std::string &onSceneCreatedFunc,
	const std::string &onSceneUpdateFunc,
	const std::string &onResumeFunc)
{
	LoadSceneInScript(
		escFile,
		onSceneCreatedFunc,
		onSceneUpdateFunc,
		onResumeFunc,
		Vector2(ETH_DEFAULT_BUCKET_SIZE, ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const std::string &escFile, const std::string &onSceneCreatedFunc, const std::string &onSceneUpdateFunc)
{
	LoadSceneInScript(
		escFile,
		onSceneCreatedFunc,
		onSceneUpdateFunc,
		(""),
		Vector2(ETH_DEFAULT_BUCKET_SIZE, ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(
	const std::string &escFile,
	const std::string &onSceneCreatedFunc,
	const std::string &onSceneUpdateFunc,
	const Vector2 &v2BucketSize)
{
	LoadSceneInScript(
		escFile,
		onSceneCreatedFunc,
		onSceneUpdateFunc,
		(""),
		v2BucketSize);
}

void ETHScriptWrapper::LoadSceneInScript(
	const std::string &escFile,
	const std::string &onSceneCreatedFunc,
	const std::string &onSceneUpdateFunc,
	const std::string &onResumeFunc,
	const Vector2 &v2BucketSize)
{
	LoadSceneInScript(
		escFile,
		onSceneCreatedFunc,
		onSceneUpdateFunc,
		onResumeFunc,
		(""),
		v2BucketSize);
}

void ETHScriptWrapper::LoadSceneInScript(
	const std::string &escFile,
	const std::string &onSceneCreatedFunc,
	const std::string &onSceneUpdateFunc,
	const std::string &onResumeFunc,
	const std::string &lightmapDirectory,
	const Vector2 &v2BucketSize)
{
	const std::string& sceneName((escFile == ("")) ? ETH_EMPTY_SCENE_STRING : escFile);
	
	m_nextScene.SetNextScene(
		sceneName,
		onSceneCreatedFunc,
		onSceneUpdateFunc,
		onResumeFunc,
		lightmapDirectory,
		v2BucketSize);
}

std::string ETHScriptWrapper::GetSceneFileName()
{
	WarnIfRunsInMainFunction("GetSceneFileName");

	return m_sceneFileName;
}

Vector2 ETHScriptWrapper::GetLastCameraPos()
{
	return m_v2LastCamPos;
}

void ETHScriptWrapper::SetGravity(const Vector2& gravity)
{
	m_pScene->GetSimulator().SetGravity(gravity);
}

Vector2 ETHScriptWrapper::GetGravity()
{
	return m_pScene->GetSimulator().GetGravity();
}

void ETHScriptWrapper::SetNumIterations(const int velocityIterations, const int positionIterations)
{
	m_pScene->GetSimulator().SetNumIterations(velocityIterations, positionIterations);
}

void ETHScriptWrapper::GetNumIterations(int& velocityIterations, int& positionIterations)
{
	m_pScene->GetSimulator().GetNumIterations(velocityIterations, positionIterations);
}

void ETHScriptWrapper::SetTimeStepScale(const float scale)
{
	m_pScene->GetSimulator().SetTimeStepScale(scale);
}

float ETHScriptWrapper::GetTimeStepScale()
{
	return m_pScene->GetSimulator().GetTimeStepScale();
}

ETHEntity* ETHScriptWrapper::GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal)
{
	return m_pScene->GetSimulator().GetClosestContact(a, b, point, normal);
}

ETHEntity* ETHScriptWrapper::GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point,
											   Vector2& normal, const std::string& semicolonSeparatedIgnoreList)
{
	return m_pScene->GetSimulator().GetClosestContact(a, b, point, normal, semicolonSeparatedIgnoreList);
}

bool ETHScriptWrapper::GetContactEntities(const Vector2& a, const Vector2& b, ETHEntityArray& entities)
{
	return m_pScene->GetSimulator().GetContactEntities(a, b, entities);
}

bool ETHScriptWrapper::IsFixedTimeStep()
{
	return m_pScene->GetSimulator().IsFixedTimeStep();
}

float ETHScriptWrapper::GetFixedTimeStepValue()
{
	return m_pScene->GetSimulator().GetFixedTimeStepValue();
}

void ETHScriptWrapper::SetFixedTimeStep(const bool enable)
{
	m_pScene->GetSimulator().SetFixedTimeStep(enable);
}

void ETHScriptWrapper::SetFixedTimeStepValue(const float value)
{
	m_pScene->GetSimulator().SetFixedTimeStepValue(value);
}

float ETHScriptWrapper::GetCurrentPhysicsTimeStepMS()
{
	return m_pScene->GetSimulator().GetCurrentDynamicTimeStepMS();
}

void ETHScriptWrapper::DisableContact()
{
	ETHPhysicsSimulator& simulator = m_pScene->GetSimulator();
	if (!simulator.IsRunningPreSolveContactCallback())
		m_provider->Log(("DisableContact function must be called inside " + ETHPhysicsEntityController::PRESOLVE_CONTACT_CALLBACK_PREFIX), Platform::Logger::LT_ERROR);
	simulator.DisableNextContact();	
}

void ETHScriptWrapper::SetFixedHeight(const float height)
{
	m_provider->GetVideo()->SetVirtualScreenHeight(height);
	Sprite::SetVirtualScreenHeight(m_provider->GetVideo()->GetScreenSizeF(), height);
}

void ETHScriptWrapper::UpdateFixedHeight()
{
	Sprite::SetVirtualScreenHeight(
		m_provider->GetVideo()->GetScreenSizeF(),
		m_provider->GetVideo()->GetVirtualScreenHeight());
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
	m_pScene->SetZAxisDirection(dir);
}

Vector2 ETHScriptWrapper::GetZAxisDirection()
{
	return m_pScene->GetZAxisDirection();
}

void ETHScriptWrapper::ForceEntityRendering(ETHEntity* entity)
{
	m_pScene->AddEntityToPersistentList(static_cast<ETHRenderEntity*>(entity));
}
