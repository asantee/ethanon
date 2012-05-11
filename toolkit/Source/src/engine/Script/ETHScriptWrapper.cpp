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

void ETHScriptWrapper::ShowMessage(str_type::string sMsg, const ETH_MESSAGE type, const bool abort)
{
	str_type::stringstream ss;
	Platform::Logger::TYPE logType;
	switch (type)
	{
	case ETH_ERROR:
		ss << GS_L("ERROR - ");
		logType = Platform::Logger::ERROR;
		if (abort)
		{
			Abort();
		}
		break;
	case ETH_WARNING:
		ss << GS_L("Warning - ");
		logType = Platform::Logger::WARNING;
		break;
	default:
		logType = Platform::Logger::INFO;
	};
	ss << sMsg;
	m_provider->Log(ss.str(), logType);
}

void ETHScriptWrapper::ForwardCommand(const str_type::string& cmd)
{
	m_provider->GetVideo()->ForwardCommand(cmd);
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

int ETHScriptWrapper::AddEntity(const str_type::string &file, const Vector3 &v3Pos, const float angle, ETHEntity **ppOutEntity, const str_type::string &alternativeName, const float scale)
{
	if (WarnIfRunsInMainFunction(GS_L("AddEntity")))
		return -1;

	const ETHEntityProperties* props = m_entityCache.Get(file, m_provider->GetResourcePath() + ETHDirectories::GetEntityPath(), m_provider->GetVideo()->GetFileManager());
	if (!props)
	{
		return -1;
	}

	ETHRenderEntity* entity = new ETHRenderEntity(m_provider, *props, angle);
	entity->SetOrphanPosition(v3Pos);
	entity->SetAngle(angle);

	if (scale != 1.0f)
	{
		entity->Scale(scale);
	}

	if (entity->IsStatic() && entity->IsApplyLight())
	{
		str_type::stringstream ss;
		ss << GS_L("AddEntity - This is a static entity and its lightmap has not been rendered yet. ") << 
			GS_L("It might be incorrectly lit: ") << m_provider->GetResourcePath() << ETHDirectories::GetEntityPath() << file;
		ShowMessage(ss.str(), ETH_WARNING);
	}

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

	if (m_pScene->GetBucketManager().DeleteEntity(pEntity->GetID(), ETHGlobal::GetBucket(pEntity->GetPositionXY(), m_pScene->GetBucketSize())))
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

void ETHScriptWrapper::PrintFloat(const float f)
{
	ETH_STREAM_DECL(ss) << f;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
}

void ETHScriptWrapper::PrintInt(const int n)
{
	ETH_STREAM_DECL(ss) << n;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
}

void ETHScriptWrapper::PrintUInt(const unsigned int n)
{
	ETH_STREAM_DECL(ss) << n;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
}

void ETHScriptWrapper::Print(const str_type::string &str)
{
	ETH_STREAM_DECL(ss) << str;
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
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

float ETHScriptWrapper::GetTimeF()
{
	return m_provider->GetVideo()->GetElapsedTimeF();
}

unsigned long ETHScriptWrapper::GetTime()
{
	return m_provider->GetVideo()->GetElapsedTime();
}

float ETHScriptWrapper::UnitsPerSecond(const float speed)
{
	return static_cast<float>(static_cast<double>(m_lastFrameElapsedTime) / 1000.0 * static_cast<double>(speed));
}

void ETHScriptWrapper::SetLastFrameElapsedTime(const unsigned long lastFrameElapsedTime)
{
	m_lastFrameElapsedTime = lastFrameElapsedTime;
}

unsigned long ETHScriptWrapper::GetLastFrameElapsedTime()
{
	return m_lastFrameElapsedTime;
}

void ETHScriptWrapper::Exit()
{
	m_provider->GetVideo()->Quit();
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

void ETHScriptWrapper::SetParallaxIntensity(const float intensity)
{
	if (WarnIfRunsInMainFunction(GS_L("SetParallaxIntensity")))
		return;

	m_provider->GetShaderManager()->SetParallaxIntensity(intensity);
}

Vector3 ETHScriptWrapper::GetAmbientLight()
{
	if (WarnIfRunsInMainFunction(GS_L("GetAmbientLight")))
		return Vector3(0,0,0);

	return m_pScene->GetAmbientLight();
}

unsigned int ETHScriptWrapper::GetScreenWidth()
{
	return static_cast<unsigned int>(GetScreenSize().x);
}

unsigned int ETHScriptWrapper::GetScreenHeight()
{
	return static_cast<unsigned int>(GetScreenSize().y);
}

Vector2 ETHScriptWrapper::GetScreenSize()
{
	return m_backBuffer->GetBufferSize();
}

void ETHScriptWrapper::SetCameraPos(const Vector2 &v2Pos)
{
	if (WarnIfRunsInMainFunction(GS_L("SetCameraPos")))
		return;

	// rounds up camera final position
	m_provider->GetVideo()->RoundUpPosition(m_roundUpPosition);
	m_provider->GetVideo()->SetCameraPos(v2Pos);
	m_provider->GetVideo()->RoundUpPosition(false);
}

void ETHScriptWrapper::AddToCameraPos(const Vector2 &v2Add)
{
	if (WarnIfRunsInMainFunction(GS_L("AddToCameraPos")))
		return;

	// rounds up camera final position
	m_provider->GetVideo()->RoundUpPosition(m_roundUpPosition);
	m_provider->GetVideo()->MoveCamera(v2Add);
	m_provider->GetVideo()->RoundUpPosition(false);
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

float ETHScriptWrapper::GetFPSRate()
{
	return m_provider->GetVideo()->GetFPSRate();
}

Vector2 ETHScriptWrapper::ComputeCarretPosition(const str_type::string &font, const str_type::string &text, const unsigned int pos)
{
	return m_provider->GetVideo()->ComputeCarretPosition(font, text, pos);
}

Vector2 ETHScriptWrapper::ComputeTextBoxSize(const str_type::string &font, const str_type::string &text)
{
	return m_provider->GetVideo()->ComputeTextBoxSize(font, text);
}

void ETHScriptWrapper::DrawText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const GS_DWORD color, const float scale)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(
				new ETHTextDrawer(m_provider, v2Pos, text, font, color, 0x0, 0x0, scale)));
}

void ETHScriptWrapper::LoadSprite(const str_type::string &name)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadSprite")))
		return;
	LoadAndGetSprite(name);
}

SpritePtr ETHScriptWrapper::LoadAndGetSprite(const str_type::string &name)
{
	str_type::string path = m_provider->GetResourcePath();
	path += name;
	return m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetVideo(), path, false);
}

Vector2 ETHScriptWrapper::GetSpriteSize(const str_type::string &name)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		return pSprite->GetBitmapSizeF();
	}
	else
	{
		return Vector2(0,0);
	}
}

Vector2 ETHScriptWrapper::GetSpriteFrameSize(const str_type::string& name)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		return pSprite->GetFrameSize();
	}
	else
	{
		return Vector2(0,0);
	}
}

void ETHScriptWrapper::SetupSpriteRects(const str_type::string& name, const unsigned int columns, const unsigned int rows)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetupSpriteRects(columns, rows);
	}
}

void ETHScriptWrapper::SetSpriteRect(const str_type::string& name, const unsigned int frame)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetRect(frame);
	}
}

void ETHScriptWrapper::SetSpriteOrigin(const str_type::string& name, const Vector2& origin)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetOrigin(origin);
	}
}

void ETHScriptWrapper::DrawSprite(const str_type::string &name, const Vector2 &v2Pos, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(), m_provider->GetResourcePath(), name, v2Pos,
							Vector2(0,0), color, 0.0f, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawShaped(const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(), m_provider->GetResourcePath(), name, v2Pos,
							v2Size, color, 0.0f, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawSpriteZ(const str_type::string &name, const float depth, const Vector2 &v2Pos, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(), m_provider->GetResourcePath(), name, v2Pos,
							Vector2(0,0), color, depth, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawShapedZ(const str_type::string &name, const float depth, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(), m_provider->GetResourcePath(), name, v2Pos,
							v2Size, color, depth, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawFadingText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const GS_DWORD color, unsigned long time, const float scale)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(
		new ETHTextDrawer(m_provider, v2Pos, text, font, color, time, m_provider->GetVideo()->GetElapsedTime(), scale)));
}

void ETHScriptWrapper::AddLight(const Vector3 &v3Pos, const Vector3 &v3Color, const float range, const bool castShadows)
{
	if (WarnIfRunsInMainFunction(GS_L("AddLight")))
		return;

	ETHLight light(true);
	light.castShadows = castShadows;
	light.color = v3Color;
	light.pos = v3Pos;
	light.range = range;
	light.staticLight = false;
	m_pScene->AddLight(light);
}

void ETHScriptWrapper::HideCursor(const bool hide)
{
	m_provider->GetVideo()->HideCursor(hide);
}

bool ETHScriptWrapper::LoadMusic(const str_type::string &file)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadMusic")))
		return false;

	str_type::stringstream ss;
	ss << m_provider->GetResourcePath() << file;
	if (!m_provider->GetAudioResourceManager()->AddFile(m_provider->GetAudio(), ss.str(), GSST_MUSIC))
	{
		ShowMessage(GS_L("Could not load the file: ") + file, ETH_ERROR, false);
		return false;
	}
	return true;
}

bool ETHScriptWrapper::LoadSoundEffect(const str_type::string &file)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadSoundEffect")))
		return false;

	str_type::stringstream ss;
	ss << m_provider->GetResourcePath() << file;
	if (!m_provider->GetAudioResourceManager()->AddFile(m_provider->GetAudio(), ss.str().c_str(), GSST_SOUND_EFFECT))
	{
		ShowMessage(GS_L("Could not load the file: ") + file, ETH_ERROR, false);
		return false;
	}
	return true;
}

bool ETHScriptWrapper::PlaySample(const str_type::string &file)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Play();
	return true;
}

bool ETHScriptWrapper::LoopSample(const str_type::string &file, const bool loop)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetLoop(loop);
	return true;
}

bool ETHScriptWrapper::StopSample(const str_type::string &file)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Stop();
	return true;
}

bool ETHScriptWrapper::PauseSample(const str_type::string &file)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->Pause();
	return true;
}

bool ETHScriptWrapper::SetSampleVolume(const str_type::string &file, const float volume)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	const float volumeFix = Max(Min(volume, 1.0f), 0.0f);
	pSample->SetVolume(volumeFix);
	return true;
}

bool ETHScriptWrapper::SetSamplePan(const str_type::string &file, const float pan)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetPan(pan);
	return true;
}

bool ETHScriptWrapper::SetSampleSpeed(const str_type::string &file, const float speed)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	pSample->SetSpeed(speed);
	return true;
}

bool ETHScriptWrapper::SampleExists(const str_type::string &file)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
		return false;
	return true;
}

bool ETHScriptWrapper::IsSamplePlaying(const str_type::string &file)
{
	AudioSamplePtr pSample = m_provider->GetAudioResourceManager()->GetPointer(m_provider->GetAudio(), file, m_provider->GetResourcePath(), GS_L(""), GSST_UNKNOWN);
	if (!pSample)
	{
		ShowMessage(GS_L("File not found: ") + file, ETH_ERROR, false);
		return false;
	}
	return pSample->IsPlaying();
}

void ETHScriptWrapper::SetGlobalVolume(const float volume)
{
	m_provider->GetAudio()->SetGlobalVolume(volume);
}

float ETHScriptWrapper::GetGlobalVolume()
{
	return m_provider->GetAudio()->GetGlobalVolume();
}

unsigned int ETHScriptWrapper::GetNumEntities()
{
	if (WarnIfRunsInMainFunction(GS_L("GetNumEntities")))
		return 0;

	return m_pScene->GetNumEntities();
}

int ETHScriptWrapper::GetLastID()
{
	if (WarnIfRunsInMainFunction(GS_L("GetLastID")))
		return -1;

	return m_pScene->GetLastID();
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

float ETHScriptWrapper::GetMaxHeight()
{
	if (WarnIfRunsInMainFunction(GS_L("GetMaxHeight")))
		return 0.0f;
	return m_pScene->GetMaxHeight();
}

float ETHScriptWrapper::GetMinHeight()
{
	if (WarnIfRunsInMainFunction(GS_L("GetMinHeight")))
		return 0.0f;
	return m_pScene->GetMinHeight();
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

void ETHScriptWrapper::DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
							  const GS_DWORD color0, const GS_DWORD color1,
							  const GS_DWORD color2, const GS_DWORD color3)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(new ETHRectangleDrawer(m_provider, v2Pos, v2Size, color0, color1, color2, color3)));
}

void ETHScriptWrapper::DrawLine(const Vector2 &v2A, const Vector2 &v2B, const GS_COLOR a, const GS_COLOR b, const float width)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHPrimitiveDrawer>(new ETHLineDrawer(m_provider, v2A, v2B, a, b, width)));
}

/*void ETHScriptWrapper::PlayCutscene(const str_type::string &fileName)
{
	str_type::string path = m_provider->GetResourcePath();
	path += fileName;
	GS_PLAYER_INFO info = ::PlayCutscene(m_provider->GetVideo(), path, m_provider->GetInput(), GSK_ESC);
    if (info == GSPI_CLOSE_WINDOW)
		m_provider->GetVideo()->Quit();
}*/

bool ETHScriptWrapper::Windowed()
{
	return m_provider->GetVideo()->IsWindowed();
}

ETH_VIDEO_MODE ETHScriptWrapper::GetVideoMode(const unsigned int nMode)
{
	const Video::VIDEO_MODE gsMode = m_provider->GetVideo()->GetVideoMode(nMode);
	ETH_VIDEO_MODE mode;
	if (gsMode.height > 0)
	{
		mode.height = gsMode.height;
		mode.width  = gsMode.width;
		mode.pf = gsMode.pf;
	}
	return mode;
}

unsigned int ETHScriptWrapper::GetVideoModeCount()
{
	return m_provider->GetVideo()->GetVideoModeCount();
}

GS_DWORD ETHScriptWrapper::GetBackgroundColor()
{
	return m_provider->GetVideo()->GetBGColor();
}

void ETHScriptWrapper::SetBackgroundColor(const GS_DWORD color)
{
	m_provider->GetVideo()->SetBGColor(color);
}

Vector2 ETHScriptWrapper::GetSystemScreenSize()
{
	const Vector2i v2 = m_provider->GetVideo()->GetClientScreenSize();
	return Vector2((float)v2.x, (float)v2.y);
}

bool ETHScriptWrapper::GetEntityArrayByName(const str_type::string &name, ETHEntityArray &outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntityArrayByName")))
		return false;

	return m_pScene->GetBucketManager().GetEntityArrayByName(name, outVector);
}

bool ETHScriptWrapper::GetEntityArrayFromBucket(const Vector2 &v2Bucket, ETHEntityArray &outVector)
{
	if (WarnIfRunsInMainFunction(GS_L("GetEntityArrayFromBucket")))
		return false;

	return m_pScene->GetBucketManager().GetEntityArrayFromBucket(v2Bucket, outVector);
}

bool ETHScriptWrapper::GetAllEntitiesInScene(ETHEntityArray &outVector)
{
	return m_pScene->GetBucketManager().GetEntityArray(outVector);
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

void ETHScriptWrapper::AddToPosition(ETHEntity *pEntity, const Vector3 &v3Pos)
{
	pEntity->AddToPosition(v3Pos, m_pScene->GetBucketManager());
}

void ETHScriptWrapper::AddToPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos)
{
	pEntity->AddToPositionXY(v2Pos, m_pScene->GetBucketManager());
}

Vector2 ETHScriptWrapper::GetCurrentBucket(ETHEntity *pEntity)
{
	return pEntity->GetCurrentBucket(m_pScene->GetBucketManager());
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

int ETHScriptWrapper::GetArgc()
{
	return m_argc;
}

str_type::string ETHScriptWrapper::GetStringFromFileInPackage(const str_type::string& fileName)
{
	str_type::string out;
	m_provider->GetVideo()->GetFileManager()->GetAnsiFileString(fileName, out);
	return out;
}

bool ETHScriptWrapper::FileInPackageExists(const str_type::string& fileName)
{
	return m_provider->GetVideo()->GetFileManager()->FileExists(fileName);
}

bool ETHScriptWrapper::FileExists(const str_type::string& fileName)
{
	str_type::ifstream ifs(fileName.c_str());
	if (ifs.is_open())
	{
		ifs.close();
		return true;
	}
	else
	{
		return false;
	}
}

str_type::string ETHScriptWrapper::GetArgv(const int n)
{
	if (n >= m_argc || n < 0)
	{
		return GS_L("");
	}
	return m_argv[n];
}

str_type::string ETHScriptWrapper::GetProgramPath()
{
	return m_provider->GetResourcePath();
}

str_type::string ETHScriptWrapper::GetAbsolutePath(const str_type::string &fileName)
{
	return GetProgramPath() + fileName;
}

str_type::string ETHScriptWrapper::GetExternalStoragePath()
{
	return m_provider->GetVideo()->GetExternalStoragePath();
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

Vector2 ETHScriptWrapper::GetWorldSpaceCursorPos2()
{
	return Vector2(m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo())+m_provider->GetVideo()->GetCameraPos()) * m_backBuffer->GetTargetScale();
}

Vector3 ETHScriptWrapper::GetWorldSpaceCursorPos3()
{
	return Vector3(m_provider->GetInput()->GetCursorPositionF(m_provider->GetVideo())+m_provider->GetVideo()->GetCameraPos(), 0) * m_backBuffer->GetTargetScale();
}

void ETHScriptWrapper::SetHaloRotation(const bool enable)
{
	if (WarnIfRunsInMainFunction(GS_L("SetHaloRotation")))
		return;
	m_pScene->SetHaloRotation(enable);
}

void ETHScriptWrapper::EnableQuitKeys(const bool enable)
{
	m_provider->GetVideo()->EnableQuitShortcuts(enable);
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

void ETHScriptWrapper::GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace)
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
	str_type::string fileName = m_provider->GetResourcePath();
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

bool ETHScriptWrapper::LoadScene(const str_type::string &escFile, const Vector2 &v2BucketSize)
{
	if (!IsPersistentResources())
	{
		if (escFile != m_sceneFileName)
		{
			ReleaseResources();
		}
	}

	// if the name is set to _ETH_EMPTY_SCENE_STRING, don't load anything
	str_type::string fileName = m_provider->GetResourcePath();
	fileName += escFile;
	if (escFile != _ETH_EMPTY_SCENE_STRING && escFile.size() > 0)
	{
		m_pScene = ETHScenePtr(new ETHScene(fileName, m_provider, m_richLighting, ETHSceneProperties(), m_pASModule,
							   m_pScriptContext, false, v2BucketSize));
		if (!m_pScene->GetNumEntities())
		{
			ShowMessage(GS_L("Couldn't load the scene"), ETH_ERROR);
			return false;
		}
	}
	else
	{
		m_pScene = ETHScenePtr(new ETHScene(m_provider, m_richLighting, ETHSceneProperties(), m_pASModule,
							   m_pScriptContext, false, v2BucketSize));
	}

	m_pScene->ResolveJoints();
	m_primitiveList.clear();
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
	m_nextScene.SetNextScene((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile, GS_L(""), GS_L(""), GS_L(""),
							 Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
										 const str_type::string &onSceneUpdateFunc, const str_type::string &onResumeFunc)
{
	m_nextScene.SetNextScene((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile, onSceneLoadedFunc, onSceneUpdateFunc, onResumeFunc,
							 Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc, const str_type::string &onSceneUpdateFunc)
{
	m_nextScene.SetNextScene((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile, onSceneLoadedFunc, onSceneUpdateFunc, GS_L(""),
		Vector2(_ETH_DEFAULT_BUCKET_SIZE,_ETH_DEFAULT_BUCKET_SIZE));
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
										 const str_type::string &onSceneUpdateFunc, const Vector2 &v2BucketSize)
{
	m_nextScene.SetNextScene((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile, onSceneLoadedFunc, onSceneUpdateFunc, GS_L(""), v2BucketSize);
}

void ETHScriptWrapper::LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
										 const str_type::string &onSceneUpdateFunc, const str_type::string &onResumeFunc, const Vector2 &v2BucketSize)
{
	m_nextScene.SetNextScene((escFile == GS_L("")) ? _ETH_EMPTY_SCENE_STRING : escFile, onSceneLoadedFunc, onSceneUpdateFunc, onResumeFunc, v2BucketSize);
}

str_type::string ETHScriptWrapper::GetSceneFileName()
{
	if (WarnIfRunsInMainFunction(GS_L("GetSceneFileName")))
		return GS_L("");

	return m_sceneFileName;
}

ETHInput *ETHScriptWrapper::GetInputHandle()
{
	return &m_ethInput;
}

void ETHScriptWrapper::SetWindowProperties(const str_type::string &winTitle, const unsigned int width, const unsigned int height,
									const bool windowed, const bool sync, const GS_PIXEL_FORMAT gsPF)
{
	GS2D_UNUSED_ARGUMENT(sync);
	if (m_provider->GetVideo())
	{
		const bool toggle = (m_provider->GetVideo()->IsWindowed() != windowed);
		m_provider->GetVideo()->ResetVideoMode(width, height, gsPF, toggle);
		m_provider->GetVideo()->SetWindowTitle(winTitle);
		if (windowed)
		{
			Vector2i v2Backbuffer(static_cast<int>(width), static_cast<int>(height));
			Vector2i v2Screen = m_provider->GetVideo()->GetClientScreenSize();
			m_provider->GetVideo()->SetWindowPosition(v2Screen/2-v2Backbuffer/2);
		}
	}
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

void ETHScriptWrapper::SetPersistentResources(const bool enable)
{
	m_persistentResources = enable;
}

bool ETHScriptWrapper::IsPersistentResources()
{
	return m_persistentResources;
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
