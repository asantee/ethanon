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

#ifdef GS2D_STR_TYPE_WCHAR
#include "../../addons/utf16/scriptbuilder.h"
#else
#include "../../addons/ansi/scriptbuilder.h"
#endif

asIScriptEngine *ETHScriptWrapper::m_pASEngine = 0;
ETHResourceProviderPtr ETHScriptWrapper::m_provider;
std::list<boost::shared_ptr<ETHPrimitiveDrawer> > ETHScriptWrapper::m_primitiveList;
ETHScenePtr ETHScriptWrapper::m_pScene;
ETHBackBufferTargetManagerPtr ETHScriptWrapper::m_backBuffer;
bool ETHScriptWrapper::m_abort = false;
bool ETHScriptWrapper::m_highEndDevice = false;
bool ETHScriptWrapper::m_useLightmaps = true;
ETHSpeedTimer ETHScriptWrapper::m_timer;
bool ETHScriptWrapper::m_roundUpPosition = true;
int ETHScriptWrapper::m_argc = 0;
str_type::char_t **ETHScriptWrapper::m_argv = 0;
ETHScriptWrapper::ETH_NEXT_SCENE ETHScriptWrapper::m_nextScene;
str_type::string ETHScriptWrapper::m_sceneFileName = GS_L("");
ETHInput ETHScriptWrapper::m_ethInput;
asIScriptModule *ETHScriptWrapper::m_pASModule = 0;
Vector2 ETHScriptWrapper::m_v2LastCamPos(0,0);
int ETHScriptWrapper::m_onSceneUpdateFunctionId =-1;
int ETHScriptWrapper::m_onResumeFunctionId =-1;
asIScriptContext *ETHScriptWrapper::m_pScriptContext = 0;
asIScriptContext *ETHScriptWrapper::m_pConstructorContext = 0;
bool ETHScriptWrapper::m_runningMainFunction = false;
bool ETHScriptWrapper::m_persistentResources = false;
ETHScriptWrapper::Math ETHScriptWrapper::m_math;
unsigned long ETHScriptWrapper::m_lastFrameElapsedTime = 1;
ETHEntityCache ETHScriptWrapper::m_entityCache;
ETHScriptWrapper::GARBAGE_COLLECT_MODE ETHScriptWrapper::m_gcMode = ETHScriptWrapper::ONE_STEP;
bool ETHScriptWrapper::m_richLighting = false;

bool ETHScriptWrapper::RunMainFunction(const int mainFuncId)
{
	if (mainFuncId < 0)
		return false;

	m_runningMainFunction = true;
	ETHGlobal::ExecuteContext(m_pScriptContext, mainFuncId, true);
	m_runningMainFunction = false;
	return true;
}

bool ETHScriptWrapper::WarnIfRunsInMainFunction(const str_type::string &functionName)
{
	if (IsRunningMainFunction())
	{
		ETH_STREAM_DECL(ss) << std::endl << GS_L("WARNING: do not load resources or do scene-related operations inside the main() function.") << std::endl
			<< GS_L("Use onSceneLoaded or onSceneUpdate functions instead.") << std::endl
			<< GS_L("Function used: ") << functionName << std::endl;
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return true;
	}
	return false;
}

void ETHScriptWrapper::Abort()
{
	m_abort = true;
}

bool ETHScriptWrapper::Aborted()
{
	return m_abort;
}

Vector2 ETHScriptWrapper::GetCursorPos()
{
	return m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo()) * m_backBuffer->GetTargetScale();
}

Vector2 ETHScriptWrapper::GetCursorAbsolutePos()
{
	return m_provider->GetInput()->GetCursorPositionF(VideoPtr()) * m_backBuffer->GetTargetScale();
}

bool ETHScriptWrapper::SetCursorPos(const Vector2 &v2Pos)
{
	return m_provider->GetInput()->SetCursorPositionF(v2Pos / m_backBuffer->GetTargetScale());
}

str_type::string ETHScriptWrapper::GetLastCharInput()
{
	str_type::stringstream ss;
	str_type::char_t lastChar = m_provider->GetInput()->GetLastCharInput();
	if (lastChar)
	{
		ss << lastChar;
		return ss.str();
	}
	return GS_L("");
}

void ETHScriptWrapper::PlayParticleSystem(ETHEntity *pEntity, const unsigned int n)
{
	pEntity->PlayParticleSystem(n, m_pScene->GetZAxisDirection());
}

void ETHScriptWrapper::SetPosition(ETHEntity *pEntity, const Vector3 &v3Pos)
{
	pEntity->SetPosition(v3Pos, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::SetPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos)
{
	pEntity->SetPositionXY(v2Pos, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::SetPositionX(ETHEntity *pEntity, const float v)
{
	pEntity->SetPositionX(v, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::SetPositionY(ETHEntity *pEntity, const float v)
{
	pEntity->SetPositionY(v, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::SetPositionZ(ETHEntity *pEntity, const float v)
{
	pEntity->SetPositionZ(v);
}

void ETHScriptWrapper::AddToPosition(ETHEntity *pEntity, const Vector3 &v3Pos)
{
	pEntity->AddToPosition(v3Pos * m_provider->GetGlobalScaleManager()->GetScale(), m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos)
{
	pEntity->AddToPositionXY(v2Pos * m_provider->GetGlobalScaleManager()->GetScale(), m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionX(ETHEntity *pEntity, const float v)
{
	pEntity->AddToPositionX(v * m_provider->GetGlobalScaleManager()->GetScale(), m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionY(ETHEntity *pEntity, const float v)
{
	pEntity->AddToPositionY(v * m_provider->GetGlobalScaleManager()->GetScale(), m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionZ(ETHEntity *pEntity, const float v)
{
	pEntity->AddToPositionZ(v * m_provider->GetGlobalScaleManager()->GetScale());
}

void ETHScriptWrapper::ResolveEntityJoints(ETHEntity* pEntity)
{
	ETHPhysicsEntityControllerPtr controller = boost::dynamic_pointer_cast<ETHPhysicsEntityController>(pEntity->GetController());
	if (controller)
	{
		ETHBucketManager& bucketManager = m_pScene->GetBucketManager();
		const Vector2& currentBucket = pEntity->GetCurrentBucket(bucketManager);
		ETHEntityArray entitiesAround;
		bucketManager.GetEntitiesAroundBucket(currentBucket, entitiesAround);
		controller->ResolveJoints(entitiesAround, *pEntity->GetProperties(), m_pScene->GetSimulator());
	}
}

Vector2 ETHScriptWrapper::GetScreenRectMin(ETHEntity *pEntity)
{
	return pEntity->GetScreenRectMin(*m_pScene->GetSceneProperties());
}

Vector2 ETHScriptWrapper::GetScreenRectMax(ETHEntity *pEntity)
{
	return pEntity->GetScreenRectMax(*m_pScene->GetSceneProperties());
}

Vector2 ETHScriptWrapper::GetBucket(const Vector2 &v2)
{
	return ETHGlobal::GetBucket(v2, m_pScene->GetBucketSize());
}

Vector2 ETHScriptWrapper::GetWorldSpaceCursorPos2()
{
	return Vector2(m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo())+m_provider->GetVideo()->GetCameraPos()) * m_backBuffer->GetTargetScale();
}

Vector3 ETHScriptWrapper::GetWorldSpaceCursorPos3()
{
	return Vector3(m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo())+m_provider->GetVideo()->GetCameraPos(), 0) * m_backBuffer->GetTargetScale();
}

void ETHScriptWrapper::GarbageCollect(const GARBAGE_COLLECT_MODE mode, asIScriptEngine* engine)
{
	switch (mode)
	{
	case ONE_STEP:
		engine->GarbageCollect(asGC_ONE_STEP);
		break;
	case FULL_CYCLE:
		engine->GarbageCollect(asGC_FULL_CYCLE | asGC_DESTROY_GARBAGE);
		break;
	case DESTROY_ALL_GARBAGE:
		engine->GarbageCollect(asGC_FULL_CYCLE);
		break;
	}
}

void ETHScriptWrapper::SetFastGarbageCollector(const bool enable)
{
	m_gcMode = enable ? ONE_STEP : FULL_CYCLE;
}

void ETHScriptWrapper::SetHighEndDevice(const bool highEnd)
{
	m_highEndDevice = highEnd;
}

bool ETHScriptWrapper::IsHighEndDevice()
{
	return m_highEndDevice;
}

ETHInput *ETHScriptWrapper::GetInputHandle()
{
	return &m_ethInput;
}

void ETHScriptWrapper::SetPersistentResources(const bool enable)
{
	m_persistentResources = enable;
}

bool ETHScriptWrapper::IsPersistentResources()
{
	return m_persistentResources;
}

int ETHScriptWrapper::GetFunctionId(asIScriptModule* pModule, const str_type::string& name)
{
	int id =-1;
	if (name != GS_L(""))
	{
		id = CScriptBuilder::GetFunctionIdByName(pModule, name);
		if (id < 0)
		{
			ETHGlobal::CheckFunctionSeekError(id, name);
		}
	}
	return id;
}

ETHResourceProviderPtr ETHScriptWrapper::GetProvider()
{
	return m_provider;
}

void ETHScriptWrapper::CreateDynamicBackBuffer(const ETHAppEnmlFile& file)
{
	m_backBuffer = ETHBackBufferTargetManagerPtr(
		new ETHBackBufferTargetManager(m_provider->GetVideo(), file, *m_provider->GetLogger()));
	m_ethInput.SetTargetManager(m_backBuffer);
}
