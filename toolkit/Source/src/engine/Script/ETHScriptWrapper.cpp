#include "ETHScriptWrapper.h"

#include "../Shader/ETHShaderManager.h"

#include "../Entity/ETHRenderEntity.h"

#include "../../addons/scriptbuilder.h"

asIScriptEngine *ETHScriptWrapper::m_pASEngine = 0;
ETHResourceProviderPtr ETHScriptWrapper::m_provider;
ETHDrawableManager ETHScriptWrapper::m_drawableManager;
ETHScenePtr ETHScriptWrapper::m_pScene;
ETHBackBufferTargetManagerPtr ETHScriptWrapper::m_backBuffer;
bool ETHScriptWrapper::m_abort = false;
bool ETHScriptWrapper::m_highEndDevice = false;
Platform::FileManagerPtr ETHScriptWrapper::m_expansionFileManager;
bool ETHScriptWrapper::m_roundUpPosition = true;
int ETHScriptWrapper::m_argc = 0;
char **ETHScriptWrapper::m_argv = 0;
ETHScriptWrapper::ETH_NEXT_SCENE ETHScriptWrapper::m_nextScene;
std::string ETHScriptWrapper::m_sceneFileName = ("");
ETHInput ETHScriptWrapper::m_ethInput;
asIScriptModule *ETHScriptWrapper::m_pASModule = 0;
Vector2 ETHScriptWrapper::m_v2LastCamPos(0,0);
asIScriptFunction* ETHScriptWrapper::m_onSceneUpdateFunction = 0;
asIScriptFunction* ETHScriptWrapper::m_onResumeFunction = 0;
asIScriptContext *ETHScriptWrapper::m_pScriptContext = 0;
bool ETHScriptWrapper::m_runningMainFunction = false;
bool ETHScriptWrapper::m_persistentResources = false;
ETHScriptWrapper::Math ETHScriptWrapper::m_math;
float ETHScriptWrapper::m_lastFrameElapsedTime = 1.0f;
ETHEntityCache ETHScriptWrapper::m_entityCache;
ETHScriptWrapper::GARBAGE_COLLECT_MODE ETHScriptWrapper::m_gcMode = ETHScriptWrapper::ONE_STEP;

bool ETHScriptWrapper::RunMainFunction(asIScriptFunction* mainFunc)
{
	if (!mainFunc)
		return false;

	m_runningMainFunction = true;
	ETHGlobal::ExecuteContext(m_pScriptContext, mainFunc, true);
	m_runningMainFunction = false;
	return true;
}

bool ETHScriptWrapper::WarnIfRunsInMainFunction(const std::string &functionName)
{
	if (IsRunningMainFunction())
	{
		ETH_STREAM_DECL(ss) << std::endl << ("WARNING: do not load resources or do scene-related operations inside the main() function.") << std::endl
			<< ("Use onSceneLoaded or onSceneUpdate functions instead.") << std::endl
			<< ("Function used: ") << functionName << std::endl;
		m_provider->Log(ss.str(), Platform::Logger::LT_ERROR);
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
	const float scale = m_provider->GetVideo()->GetScaleFactor();
	return m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo()) * scale;
}

Vector2 ETHScriptWrapper::GetCursorAbsolutePos()
{
	return GetCursorPos();
}

bool ETHScriptWrapper::SetCursorPos(const Vector2 &v2Pos)
{
	const float scale = m_provider->GetVideo()->GetScaleFactor();
	return m_provider->GetInput()->SetCursorPositionF(v2Pos) * scale;
}

std::string ETHScriptWrapper::GetLastCharInput()
{
	std::stringstream ss;
	std::string lastChar = m_provider->GetInput()->GetLastCharInput();
	if (!lastChar.empty())
	{
		ss << lastChar;
		return ss.str();
	}
	return ("");
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
	pEntity->AddToPosition(v3Pos, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos)
{
	pEntity->AddToPositionXY(v2Pos, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionX(ETHEntity *pEntity, const float v)
{
	pEntity->AddToPositionX(v, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionY(ETHEntity *pEntity, const float v)
{
	pEntity->AddToPositionY(v, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionZ(ETHEntity *pEntity, const float v)
{
	pEntity->AddToPositionZ(v);
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

Vector2 ETHScriptWrapper::GetBucket(const Vector2 &v2)
{
	return ETHBucketManager::GetBucket(v2, m_pScene->GetBucketSize());
}

Vector2 ETHScriptWrapper::GetWorldSpaceCursorPos2()
{
	return m_provider->GetVideo()->GetCameraPos() + GetCursorPos();
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

bool ETHScriptWrapper::ArePersistentResourcesEnabled()
{
	return m_persistentResources;
}

ETHResourceProviderPtr ETHScriptWrapper::GetProvider()
{
	return m_provider;
}

void ETHScriptWrapper::CreateDynamicBackBuffer(const ETHAppEnmlFile& file)
{
	m_backBuffer = ETHBackBufferTargetManager::Create(m_provider->GetVideo(), file, *m_provider->GetLogger(), m_ethInput);
}

void ETHScriptWrapper::SetArgc(const int argc)
{
	m_argc = argc;
}

void ETHScriptWrapper::SetArgv(char **argv)
{
	m_argv = argv;
}
