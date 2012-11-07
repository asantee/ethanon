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

Vector2 ETHScriptWrapper::ETH_NEXT_SCENE::GetBucketSize() const
{
	return bucketSize;
}

void ETHScriptWrapper::ETH_NEXT_SCENE::SetNextScene(const str_type::string &sceneName, const str_type::string &onSceneLoadedFunc,
													const str_type::string &onSceneUpdateFunc, const str_type::string& onResumeFunc,
													const Vector2 &bucketSize)
{
	this->sceneName = sceneName;
	this->onSceneLoadedFunc = onSceneLoadedFunc;
	this->onSceneUpdateFunc = onSceneUpdateFunc;
	this->onResumeFunc = onResumeFunc;
	this->bucketSize = bucketSize;
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

int ETHScriptWrapper::AddEntity(const str_type::string &file, const Vector3 &v3Pos, const float angle, ETHEntity **ppOutEntity,
								const str_type::string &alternativeName, const float scale)
{
	if (WarnIfRunsInMainFunction(GS_L("AddEntity")))
		return -1;

	const str_type::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();
	const ETHEntityProperties* props = m_entityCache.Get(file, resourceDirectory + ETHDirectories::GetEntityDirectory(),
														 m_provider->GetFileManager());

	if (!props)
	{
		return -1;
	}

	const float globalScale = m_provider->GetGlobalScaleManager()->GetScale();

	ETHRenderEntity* entity = new ETHRenderEntity(m_provider, *props, angle, scale * globalScale);
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

bool ETHScriptWrapper::GenerateLightmaps()
{
	if (!m_useLightmaps)
		return false;
	return m_pScene->GenerateLightmaps();
}

void ETHScriptWrapper::SetAmbientLight(const Vector3 &v3Color)
{
	if (WarnIfRunsInMainFunction(GS_L("SetAmbientLight")))
		return;

	m_pScene->SetAmbientLight(v3Color);
}

void ETHScriptWrapper::SetParallaxOrigin(const Vector2 &parallaxOrigin)
{
	if (WarnIfRunsInMainFunction(GS_L("SetParallaxOrigin")))
		return;

	m_provider->GetShaderManager()->SetParallaxNormalizedOrigin(parallaxOrigin);
}

Vector2 ETHScriptWrapper::GetParallaxOrigin()
{
	if (WarnIfRunsInMainFunction(GS_L("GetParallaxOrigin")))
		return gs2d::math::constant::ZERO_VECTOR2;

	return m_provider->GetShaderManager()->GetParallaxNormalizedOrigin();
}

void ETHScriptWrapper::SetParallaxIntensity(const float intensity)
{
	if (WarnIfRunsInMainFunction(GS_L("SetParallaxIntensity")))
		return;

	m_provider->GetShaderManager()->SetParallaxIntensity(intensity);
}

float ETHScriptWrapper::GetParallaxIntensity()
{
	if (WarnIfRunsInMainFunction(GS_L("GetParallaxIntensity")))
		return 0.0f;

	return m_provider->GetShaderManager()->GetParallaxIntensity();
}

void ETHScriptWrapper::SetParallaxVerticalIntensity(const float intensity)
{
	if (WarnIfRunsInMainFunction(GS_L("SetParallaxVerticalIntensity")))
		return;

	m_provider->GetShaderManager()->SetParallaxVerticalIntensity(intensity);
}

float ETHScriptWrapper::GetParallaxVerticalIntensity()
{
	if (WarnIfRunsInMainFunction(GS_L("GetParallaxVerticalIntensity")))
		return 0.0f;

	return m_provider->GetShaderManager()->GetParallaxIntensity();
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
	video->RoundUpPosition(m_roundUpPosition);
	video->SetCameraPos(v2Pos);
	video->RoundUpPosition(false);
}

void ETHScriptWrapper::AddToCameraPos(const Vector2 &v2Add)
{
	if (WarnIfRunsInMainFunction(GS_L("AddToCameraPos")))
		return;

	// rounds up camera final position
	VideoPtr video = m_provider->GetVideo();
	video->RoundUpPosition(m_roundUpPosition);
	video->MoveCamera(m_provider->GetGlobalScaleManager()->Scale(v2Add));
	video->RoundUpPosition(false);
}

void ETHScriptWrapper::EnableLightmaps(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("EnableLightmaps")))
		return;

	m_useLightmaps = enable;
	m_pScene->EnableLightmaps(enable);
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

GS_DWORD ETHScriptWrapper::GetBackgroundColor()
{
	return m_provider->GetVideo()->GetBGColor();
}

void ETHScriptWrapper::SetBackgroundColor(const GS_DWORD color)
{
	m_provider->GetVideo()->SetBGColor(color);
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

void ETHScriptWrapper::SetHaloRotation(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("SetHaloRotation")))
		return;
	m_pScene->SetHaloRotation(enable);
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

int ETHScriptWrapper::GetNumRenderedEntities()
{
	if (WarnIfRunsInMainFunction(GS_L("GetNumRenderedEntities")))
		return 0;
	return m_pScene->GetNumRenderedEntities();
}

bool ETHScriptWrapper::SaveScene(const str_type::string &escFile)
{
	if (WarnIfRunsInMainFunction(GS_L("SaveScene")))
		return false;

	// loads a new scene from file
	str_type::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory();
	fileName += escFile;

	return m_pScene->SaveToFile(fileName);
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

bool ETHScriptWrapper::LoadScene(const str_type::string &escFile, const Vector2& bucketSize)
{
	if (!ArePersistentResourcesEnabled())
	{
		if (escFile != m_sceneFileName)
		{
			ReleaseResources();
		}
	}

	str_type::string fileName = m_provider->GetFileIOHub()->GetResourceDirectory();
	fileName += escFile;

	// if the name is set to _ETH_EMPTY_SCENE_STRING, don't load anything
	if (escFile != _ETH_EMPTY_SCENE_STRING && escFile.size() > 0)
	{
		m_pScene = ETHScenePtr(new ETHScene(fileName, m_provider, m_richLighting, ETHSceneProperties(), m_pASModule,
			m_pScriptContext, false, bucketSize));
	}
	else
	{
		m_pScene = ETHScenePtr(new ETHScene(m_provider, m_richLighting, ETHSceneProperties(), m_pASModule,
			m_pScriptContext, false, bucketSize));
	}

	m_pScene->ScaleEntities(m_provider->GetGlobalScaleManager()->GetScale(), true);
	m_pScene->ResolveJoints();
	m_drawableManager.Clear();
	m_sceneFileName = escFile;
	m_pScene->EnableLightmaps(m_useLightmaps);
	GenerateLightmaps();
	m_provider->GetVideo()->SetCameraPos(Vector2(0,0));
	LoadSceneScripts();
	m_timer.CalcLastFrame();
	return true;
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile)
{
	LoadSceneInScript(escFile, GS_L(""), GS_L(""), GS_L(""), Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
										 const str_type::string &onSceneUpdateFunc, const str_type::string &onResumeFunc)
{
	LoadSceneInScript(escFile, onSceneLoadedFunc, onSceneUpdateFunc, onResumeFunc, Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc, const str_type::string &onSceneUpdateFunc)
{
	LoadSceneInScript(escFile, onSceneLoadedFunc, onSceneUpdateFunc, GS_L(""), Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
										 const str_type::string &onSceneUpdateFunc, const Vector2 &v2BucketSize)
{
	LoadSceneInScript(escFile, onSceneLoadedFunc, onSceneUpdateFunc, GS_L(""), v2BucketSize);
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
										 const str_type::string &onSceneUpdateFunc, const str_type::string &onResumeFunc, const Vector2 &v2BucketSize)
{
	const float globalScale = m_provider->GetGlobalScaleManager()->GetScale();
	const str_type::string& sceneName((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile);
	m_nextScene.SetNextScene(sceneName, onSceneLoadedFunc, onSceneUpdateFunc, onResumeFunc, v2BucketSize * globalScale);
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
	const int onSceneLoadedFuncId = GetFunctionId(m_pASModule, m_nextScene.GetOnSceneLoadedFunc());
	if (onSceneLoadedFuncId >= 0)
	{
		ETHGlobal::ExecuteContext(m_pScriptContext, onSceneLoadedFuncId);
	}

	m_onSceneUpdateFunctionId = GetFunctionId(m_pASModule, m_nextScene.GetOnSceneUpdateFunc());
	m_onResumeFunctionId = GetFunctionId(m_pASModule, m_nextScene.GetOnResumeFunc());
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

void ETHScriptWrapper::UsePixelShaders(const bool enable)
{
	if (!m_provider->GetShaderManager()->IsPixelLightingSupported() && enable)
	{
		ETH_STREAM_DECL(ss) << GS_L("Scripting error: pixel shaders are no supported.");
		m_provider->Log(ss.str(), Platform::FileLogger::WARNING);
		return;
	}
	m_provider->GetShaderManager()->UsePS(enable);
}

bool ETHScriptWrapper::IsPixelShaderSupported()
{
	return m_provider->GetShaderManager()->IsPixelLightingSupported();
}

void ETHScriptWrapper::SetFixedHeight(const float height)
{
	m_provider->GetGlobalScaleManager()->SetFixedHeight(m_backBuffer, height);
}

void ETHScriptWrapper::SetFixedWidth(const float width)
{
	m_provider->GetGlobalScaleManager()->SetFixedWidth(m_backBuffer, width);
}

float ETHScriptWrapper::GetScale()
{
	return m_provider->GetGlobalScaleManager()->GetScale();
}

float ETHScriptWrapper::Scale(const float v)
{
	return m_provider->GetGlobalScaleManager()->Scale(v);
}

Vector2 ETHScriptWrapper::Scale(const Vector2& v)
{
	return m_provider->GetGlobalScaleManager()->Scale(v);
}

Vector3 ETHScriptWrapper::Scale(const Vector3& v)
{
	return m_provider->GetGlobalScaleManager()->Scale(v);
}

void ETHScriptWrapper::SetScaleFactor(const float v)
{
	m_provider->GetGlobalScaleManager()->SetScaleFactor(v);
}

void ETHScriptWrapper::ScaleEntities()
{
	if (WarnIfRunsInMainFunction(GS_L("ScaleEntities")))
		return;
	m_pScene->ScaleEntities(m_provider->GetGlobalScaleManager()->GetScale(), true);
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
