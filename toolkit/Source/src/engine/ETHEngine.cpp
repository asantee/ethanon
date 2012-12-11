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

#include "ETHEngine.h"

#include "Script/ETHScriptObjRegister.h"
#include "Script/ETHBinaryStream.h"

#include "ETHTypes.h"

#include "Platform/ETHAppEnmlFile.h"

#include "Resource/ETHDirectories.h"

#include <Unicode/UTF8Converter.h>

#ifdef GS2D_STR_TYPE_WCHAR
 #include "../addons/utf16/scriptbuilder.h"
#else
 #include "../addons/ansi/scriptbuilder.h"
#endif

using namespace gs2d;
using namespace gs2d::math;

const str_type::string ETHEngine::ETH_SCRIPT_MODULE(GS_L("EthanonModule"));

gs2d::BaseApplicationPtr gs2d::CreateBaseApplication()
{
	return BaseApplicationPtr(new ETHEngine(false, true));
}

#ifdef ANDROID
#	define ETH_BYTECODE_FILE_NAME GS_L("android_game.bin")
#elif APPLE_IOS
#	define ETH_BYTECODE_FILE_NAME GS_L("ios_game.bin")
#else
#	define ETH_BYTECODE_FILE_NAME GS_L("game.bin")
#endif

ETHEngine::ETHEngine(const bool testing, const bool compileAndRun) :
	ETH_DEFAULT_MAIN_SCRIPT_FILE(_ETH_DEFAULT_MAIN_SCRIPT_FILE),
	ETH_DEFAULT_MAIN_BYTECODE_FILE(ETH_BYTECODE_FILE_NAME),
	ETH_MAIN_FUNCTION(GS_L("main")),
	m_testing(testing),
	m_compileAndRun(compileAndRun),
	m_lastBGColor(0x0),
	m_hasBeenResumed(false)
{
}

ETHEngine::~ETHEngine()
{
	m_pScene.reset(); // destroy the scene first, so the script engine is free to run garbage collection
	if (m_pScriptContext)
	{
		m_pScriptContext->Release();
		m_pScriptContext = 0;
	}
	if (m_pConstructorContext)
	{
		m_pConstructorContext->Release();
		m_pConstructorContext = 0;
	}
	m_pASEngine->Release();
	m_pASEngine = 0;
}

ETHResourceProviderPtr ETHEngine::GetProvider()
{
	return m_provider;
}

void ETHEngine::Start(VideoPtr video, InputPtr input, AudioPtr audio)
{
	Platform::FileIOHubPtr fileIOHub = video->GetFileIOHub();

	ETHAppEnmlFile file(fileIOHub->GetResourceDirectory() + ETH_APP_PROPERTIES_FILE, fileIOHub->GetFileManager(), video->GetPlatformName());
	m_richLighting = file.IsRichLightingEnabled();

	m_provider = ETHResourceProviderPtr(new ETHResourceProvider(
		ETHGraphicResourceManagerPtr(new ETHGraphicResourceManager(file.GetDensityManager())),
		ETHAudioResourceManagerPtr(new ETHAudioResourceManager()),
		ETHShaderManagerPtr(new ETHShaderManager(video, fileIOHub->GetStartResourceDirectory() + ETHDirectories::GetShaderDirectory(), m_richLighting)),
		video, audio, input, fileIOHub, false));

	m_ethInput.SetProvider(m_provider);

	CreateDynamicBackBuffer(file);

	if (!m_pASEngine)
	{
		video->SetBGColor(gs2d::constant::BLACK);

		GS2D_COUT << GS_L("AngelScript v") << asGetLibraryVersion() << GS_L(" options: ") << asGetLibraryOptions() << std::endl;
		if (!PrepareScriptingEngine(file.GetDefinedWords()))
		{
			Abort();
			return;
		}

		if (m_compileAndRun)
		{
			if (!RunMainFunction(GetMainFunction()))
			{
				Abort();
				return;
			}
			video->EnableQuitShortcuts(true);
			m_v2LastCamPos = video->GetCameraPos();
		}
	}
	else
	{
		video->SetBGColor(m_lastBGColor);
		m_pScene->RecoverResources();
	}
}

Application::APP_STATUS ETHEngine::Update(
	unsigned long lastFrameDeltaTimeMS)
{
	// removes dead elements on top layer to fill the list once again
	m_drawableManager.RemoveTheDead();

	SetLastFrameElapsedTime(lastFrameDeltaTimeMS);

	// run garbage collector
	GarbageCollect(m_gcMode, m_pASEngine);

	// process scene load request
	if (!LoadNextSceneIfRequested())
		Abort();

	if (m_hasBeenResumed) // I know I know...
	{
		RunOnResumeFunction();
		m_hasBeenResumed = false;
	}

	//update timer
	m_timer.CalcLastFrame();

	if (m_pScene)
		m_pScene->Update(lastFrameDeltaTimeMS, m_backBuffer, m_onSceneUpdateFunction);

	if (Aborted())
		return Application::APP_QUIT;
	else
		return Application::APP_OK;
}

bool ETHEngine::LoadNextSceneIfRequested()
{
	if (m_nextScene.HasNextScene())
	{
		LoadScene(m_nextScene.GetSceneName(), m_nextScene.GetBucketSize());
		m_nextScene.Reset();
		GarbageCollect(DESTROY_ALL_GARBAGE, m_pASEngine);
	}
	else
	{
		if (!m_pScene)
		{
			ShowMessage(GS_L("ETHEngine::StartEngine: the script has set no scene to load."), ETH_ERROR);
			return false;
		}
	}
	return true;
}

void ETHEngine::RenderFrame()
{
	m_backBuffer->BeginRendering();

	// draw scene (if there's any)
	
	if (m_pScene)
		m_pScene->RenderScene(IsRoundingUpPosition());

	m_v2LastCamPos = GetCameraPos();

	// draw sprites, rects, lines and texts
	DrawTopLayer(GetLastFrameElapsedTime());

	m_backBuffer->EndRendering();

	m_backBuffer->Present();
}

bool ETHEngine::RunOnResumeFunction() const
{
	return RunFunction(m_onResumeFunction);
}

bool ETHEngine::RunFunction(asIScriptFunction* func) const
{
	if (func)
	{
		ETHGlobal::ExecuteContext(m_pScriptContext, func);
		return true;
	}
	else
	{
		return false;
	}
}

void ETHEngine::Destroy()
{
	m_lastBGColor = m_provider->GetVideo()->GetBGColor();
	m_provider->GetGraphicResourceManager()->ReleaseResources();
	m_provider->GetAudioResourceManager()->ReleaseResources();
	m_backBuffer.reset();
}

bool ETHEngine::PrepareScriptingEngine(const std::vector<gs2d::str_type::string>& definedWords)
{
	m_pASEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if (!m_pASEngine)
	{
		ShowMessage(GS_L("Failed to create AngelScript engine."), ETH_ERROR);
		return false;
	}

	// Set UTF-8 encoding
	int r = m_pASEngine->SetEngineProperty(asEP_SCRIPT_SCANNER, 1);
	if (!CheckAngelScriptError((r < 0), GS_L("Failed setting up script scanner.")))
		return false;

	#ifdef GS2D_STR_TYPE_WCHAR
	r = m_pASEngine->SetEngineProperty(asEP_STRING_ENCODING, 1);
	// #else
	// r = m_pASEngine->SetEngineProperty(asEP_STRING_ENCODING, 0);
	#endif

	if (!CheckAngelScriptError((r < 0), GS_L("Failed while setting up string encoding")))
		return false;

	// Message callback
	r = m_pASEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	if (!CheckAngelScriptError((r < 0), GS_L("Failed while setting message callback.")))
		return false;

	ETHGlobal::RegisterEnumTypes(m_pASEngine);
	ETHGlobal::RegisterGlobalProperties(m_pASEngine);
	ETHGlobal::RegisterAllObjects(m_pASEngine);
	RegisterGlobalFunctions(m_pASEngine);

	m_pScriptContext = m_pASEngine->CreateContext();
	m_pConstructorContext = m_pASEngine->CreateContext();

	if (!BuildModule(definedWords))
		return false;

	// Always collect all garbage by default
	SetFastGarbageCollector(false);

	return true;
}

static void RegisterDefinedWords(const std::vector<gs2d::str_type::string>& definedWords, CScriptBuilder& builder, const bool isTesting)
{
	#if defined(_DEBUG) || defined(DEBUG)
		builder.DefineWord("DEBUG");
	#endif
	#ifdef ANDROID
		builder.DefineWord("ANDROID");
	#endif
	#ifdef APPLE_IOS
		builder.DefineWord("APPLE_IOS");
	#endif
	#ifdef MACOSX
		builder.DefineWord("MACOSX");
	#endif
	#ifdef WIN32
		builder.DefineWord("WINDOWS");
	#endif
	#ifdef LINUX
		builder.DefineWord("LINUX");
	#endif
	#if defined(APPLE_IOS) || defined(ANDROID)
		builder.DefineWord("MOBILE");
		builder.DefineWord("MOBILE_DEVICE");
		builder.DefineWord("HANDHELD");
		builder.DefineWord("HANDHELD_DEVICE");
	#endif
	#if defined(WIN32) || defined(MACOSX) || defined(LINUX)
		builder.DefineWord("DESKTOP");
	#endif

	if (isTesting)
	{
		builder.DefineWord("TESTING");
	}

	// register custom words (defined in the app.enml)
	for (std::size_t t = 0; t < definedWords.size(); t++)
	{
		builder.DefineWord(definedWords[t].c_str());
	}
}

bool ETHEngine::BuildModule(const std::vector<gs2d::str_type::string>& definedWords)
{
	const str_type::string resourcePath = m_provider->GetFileIOHub()->GetResourceDirectory();
	const str_type::string mainScript = resourcePath + ETH_DEFAULT_MAIN_SCRIPT_FILE;
	const str_type::string byteCodeWriteFile = m_provider->GetByteCodeSaveDirectory() + ETH_DEFAULT_MAIN_BYTECODE_FILE;
	const str_type::string byteCodeReadFile  = resourcePath + ETH_DEFAULT_MAIN_BYTECODE_FILE;

	// if there's a main script file, load the source from text code and compile it
	if (ETHGlobal::FileExists(mainScript, m_provider->GetFileManager()))
	{
		ETH_STREAM_DECL(ssi) << GS_L("Loading game script from source-code: ") << ETH_DEFAULT_MAIN_SCRIPT_FILE;
		m_provider->Log(ssi.str(), Platform::Logger::INFO);

		// Load the main script
		CScriptBuilder builder(m_provider);

		RegisterDefinedWords(definedWords, builder, m_testing);

		int r;
		r = builder.StartNewModule(m_pASEngine, ETH_SCRIPT_MODULE.c_str());
		if (!CheckAngelScriptError(r < 0, GS_L("Failed while starting the new module.")))
			return false;

		r = builder.AddSectionFromFile(mainScript.c_str());
		str_type::stringstream ss;
		ss << GS_L("Failed while loading the main script. Verify the ") << mainScript << GS_L(" file");
		if (!CheckAngelScriptError(r < 0, ss.str()))
			return false;

		// builds the module
		const VideoPtr& video = m_provider->GetVideo();
		const unsigned long buildTime = video->GetElapsedTime();
		r = builder.BuildModule();
		str_type::stringstream timeStringStream; timeStringStream << GS_L("Compile time: ") << video->GetElapsedTime() - buildTime << GS_L(" milliseconds");
		m_provider->Log(timeStringStream.str(), Platform::Logger::INFO);
		if (!CheckAngelScriptError(r < 0, GS_L("Failed while building module.")))
			return false;

		// Gets the recently built module
		m_pASModule = CScriptBuilder::GetModule(m_pASEngine, ETH_SCRIPT_MODULE);

		// Writes the compiled byte code to file
		ETHBinaryStream stream(m_provider->GetFileManager());
		if (stream.OpenW(byteCodeWriteFile))
		{
			m_pASModule->SaveByteCode(&stream);
			stream.CloseW();
		}
		else
		{
			ETH_STREAM_DECL(ss) << GS_L("Failed while writing the byte code file ") << byteCodeWriteFile;
			m_provider->Log(ss.str(), Platform::Logger::ERROR);
			// return false;
		}
	}
	else // otherwiser, try to load the bytecode
	{
		ETH_STREAM_DECL(ss) << GS_L("Loading game script from pre-compiled byte code: ") << ETH_DEFAULT_MAIN_BYTECODE_FILE;
		m_provider->Log(ss.str(), Platform::Logger::INFO);
	
		m_pASModule = CScriptBuilder::GetModule(m_pASEngine, ETH_SCRIPT_MODULE, asGM_ALWAYS_CREATE);
		ETHBinaryStream stream(m_provider->GetFileManager());
		if (stream.OpenR(byteCodeReadFile))
		{
			if (m_pASModule->LoadByteCode(&stream) < 0)
			{
				ETH_STREAM_DECL(ss) << GS_L("Couldn't load game script from pre-compiled byte code: ") << ETH_DEFAULT_MAIN_BYTECODE_FILE;
				m_provider->Log(ss.str(), Platform::Logger::ERROR);
				stream.CloseR();
				return false;
			}
			stream.CloseR();
		}
		else
		{
			ETH_STREAM_DECL(ss) << GS_L("Failed while reading the byte code file ") << byteCodeReadFile;
			m_provider->Log(ss.str(), Platform::Logger::ERROR);
			Abort();
			return false;
		}
	}

	return true;
}

asIScriptFunction* ETHEngine::GetMainFunction() const
{
	// finds the main function
	asIScriptFunction* mainFunc = m_pASModule->GetFunctionByName(utf8::c(ETH_MAIN_FUNCTION).c_str());
	ETH_STREAM_DECL(ss) << GS_L("Function not found: ") << ETH_MAIN_FUNCTION;
	CheckAngelScriptError((!mainFunc), ss.str());
	return mainFunc;
}

bool ETHEngine::CheckAngelScriptError(const bool error, const str_type::string &description)
{
	if (error)
	{
		ShowMessage(description, ETH_ERROR);
		return false;
	}
	else
	{
		return true;
	}
}

void ETHEngine::DrawTopLayer(const unsigned int lastFrameElapsedTimeMS)
{
	m_drawableManager.DrawTopLayer(lastFrameElapsedTimeMS, m_provider->GetVideo());
}

void ETHEngine::MessageCallback(const asSMessageInfo *msg)
{
	str_type::string typeStr = GS_L("");
	Platform::Logger::TYPE type;
	if (msg->type == asMSGTYPE_WARNING)
	{
		type = Platform::Logger::WARNING;
		typeStr = GS_L("WARNING");
	}
	else if (msg->type == asMSGTYPE_INFORMATION) 
	{
		type = Platform::Logger::INFO;
		typeStr = GS_L("INFO");
	}
	else
	{
		type = Platform::Logger::ERROR;
		typeStr = GS_L("ERROR");
	}

	ETH_STREAM_DECL(ss) << std::endl << GS_L("AngelScript ") << typeStr << GS_L(" (line ") << msg->row << GS_L("): ") << std::endl << msg->message;
	m_provider->Log(ss.str(), type);
}

void ETHEngine::Restore()
{
	if (m_richLighting && m_useLightmaps && m_pScene)
	{
		m_pScene->GenerateLightmaps();
	}
	m_hasBeenResumed = true;
}
