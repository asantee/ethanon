#include "ETHEngine.h"

#include "Script/ETHScriptObjRegister.h"
#include "Script/ETHBinaryStream.h"

#include "ETHTypes.h"

#include "Platform/ETHAppEnmlFile.h"

#include "Resource/ETHResourceLoader.h"
#include "Resource/ETHDirectories.h"

#include "../addons/scriptbuilder.h"

#include <Math/Randomizer.h>

#if defined(APPLE_IOS) || defined(ANDROID)
 #define MOBILE
#endif

using namespace gs2d;
using namespace gs2d::math;

const std::string ETHEngine::ETH_SCRIPT_MODULE(("EthanonModule"));
const std::string ETHEngine::SCRIPT_EXCEPTION_LOG_SHARED_DATA_KEY(("com.ethanonengine.scriptExceptions"));
const std::string ETHEngine::SD_CURRENT_TIME_MILLIS("com.ethanonengine.data.currentTimeMillis");

gs2d::BaseApplicationPtr gs2d::CreateBaseApplication(const bool autoStartScriptEngine)
{
	return BaseApplicationPtr(new ETHEngine(false, true, autoStartScriptEngine));
}

#define UNUSED_ARGUMENT(argument) ((void)(argument))

#if defined(__aarch64__) || defined(__x86_64__) || defined(_WIN64) || defined (__LP64__)
 #define ETH_BYTECODE_FILE_NAME ("game_64.bin")
#else
 #define ETH_BYTECODE_FILE_NAME ("game_32.bin")
#endif

ETHEngine::ETHEngine(const bool testing, const bool compileAndRun, const bool autoStartScriptEngine) :
	ETH_DEFAULT_MAIN_SCRIPT_FILE(_ETH_DEFAULT_MAIN_SCRIPT_FILE),
	ETH_DEFAULT_MAIN_BYTECODE_FILE(ETH_BYTECODE_FILE_NAME),
	ETH_MAIN_FUNCTION(("main")),
	m_testing(testing),
	m_compileAndRun(compileAndRun),
	m_lastBGColor(0x0),
	m_hasBeenResumed(false),
	m_scriptEngineReady(false),
	m_mainFunctionExecuted(false),
	m_autoStartScriptEngine(autoStartScriptEngine)
{
	Application::SharedData.Create(SCRIPT_EXCEPTION_LOG_SHARED_DATA_KEY, (""), false);
	Application::SharedData.Create(SD_CURRENT_TIME_MILLIS, "0", false);
}

ETHEngine::~ETHEngine()
{
	m_pScene.reset(); // destroy the scene first, so the script engine is free to run garbage collection
	if (m_pScriptContext)
	{
		m_pScriptContext->Release();
		m_pScriptContext = 0;
	}
	m_pASEngine->Release();
	m_pASEngine = 0;
}

boost::shared_ptr<ETHEngine> ETHEngine::Cast(gs2d::BaseApplicationPtr application)
{
	return boost::static_pointer_cast<ETHEngine>(application);
}

ETHResourceProviderPtr ETHEngine::GetProvider()
{
	return m_provider;
}

bool ETHEngine::StartScriptEngine()
{
	if (m_pASEngine)
		return true;

	VideoPtr video = m_provider->GetVideo();
	video->SetBackgroundColor(gs2d::constant::BLACK);

	std::cout << ("AngelScript v") << asGetLibraryVersion() << (" options: ") << asGetLibraryOptions() << std::endl;
	if (!PrepareScriptingEngine(m_definedWords))
	{
		Abort();
		return false;
	}

	m_scriptEngineReady = true;
	return true;
}

bool ETHEngine::IsScriptEngineLoaded() const
{
	return m_scriptEngineReady;
}

void ETHEngine::Start(VideoPtr video, InputPtr input, AudioPtr audio)
{
	Platform::FileIOHubPtr fileIOHub = video->GetFileIOHub();

	const bool lowRamDevice = Application::SharedData.Get("ethanon.system.isLowRamDevice") == "true";

	ETHAppEnmlFile file(
		fileIOHub->GetResourceDirectory() + ETH_APP_PROPERTIES_FILE,
		fileIOHub->GetFileManager(),
		video->GetPlatformName(),
		fileIOHub->GetExternalStorageDirectory(),
		lowRamDevice);

	m_definedWords = file.GetDefinedWords();

	m_provider = ETHResourceProviderPtr(new ETHResourceProvider(
		ETHGraphicResourceManagerPtr(new ETHGraphicResourceManager(file.GetDensityManager())),
		ETHAudioResourceManagerPtr(new ETHAudioResourceManager()),
		ETHShaderManagerPtr(new ETHShaderManager(video)),
		video,
		audio,
		input,
		fileIOHub));

	if (lowRamDevice)
	{
		m_provider->Log(("Ethanon is running low ram device mode."), Platform::Logger::LT_INFO);
	}
	
	m_ethInput.SetProvider(m_provider);

	CreateDynamicBackBuffer(file);

	m_drawableManager.Clear();

	if (!m_pASEngine)
	{
		if (m_autoStartScriptEngine)
		{
			if (!StartScriptEngine())
			{
				return;
			}
		}
	}
	else
	{
		video->SetBackgroundColor(m_lastBGColor);
		if (IsScriptEngineLoaded())
		{
			if (m_pScene)
			{
				m_pScene->RecoverResources();
			}
		}
	}
	
	Randomizer::Seed(static_cast<unsigned int>(m_provider->GetVideo()->GetElapsedTime()));
}

Application::APP_STATUS ETHEngine::Update(const float lastFrameDeltaTimeMS)
{
	if (ETHResourceLoader::HasContainerEnqueued())
		return Application::APP_OK;
	
	if (!m_mainFunctionExecuted)
	{
		m_provider->Log(("Starting main function"), Platform::Logger::LT_INFO);
		RunMainFunction(GetMainFunction());
		m_provider->GetVideo()->EnableQuitShortcuts(true);
		m_v2LastCamPos = m_provider->GetVideo()->GetCameraPos();
		m_mainFunctionExecuted = true;
		m_provider->Log(("Ended main function"), Platform::Logger::LT_INFO);
	}

	// updates system clock shared data
	{
		time_t seconds = time(NULL);
		const uint64_t timeMS = static_cast<uint64_t>(seconds) * 1000;
		std::ostringstream o; o << timeMS;
		Application::SharedData.Set(SD_CURRENT_TIME_MILLIS, o.str());
	}

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
		LoadScene(m_nextScene.GetSceneName(), m_nextScene.GetLightmapDirectory(), m_nextScene.GetBucketSize());
		m_nextScene.Reset();
		GarbageCollect(DESTROY_ALL_GARBAGE, m_pASEngine);
	}
	else
	{
		if (!m_pScene)
		{
			ShowMessage(("ETHEngine::LoadNextSceneIfRequested: no scene has been loaded."), ETH_ERROR);
			return false;
		}
	}
	return true;
}

void ETHEngine::RenderFrame()
{
	m_backBuffer->BeginRendering();

	if (!ETHResourceLoader::DoResourceRecoverStep(m_provider))
	{
		// draw scene (if there's any)
		if (m_pScene)
		{
			m_pScene->RenderScene(m_backBuffer);
		}

		m_v2LastCamPos = GetCameraPos();

		// draw sprites, rects, lines and texts
		DrawTopLayer(GetLastFrameElapsedTime());
	}
	else
	{
		if (m_pScene)
		{
			m_pScene->EmptyRenderingQueue();
		}
	}
	
	m_backBuffer->EndRendering();

	m_backBuffer->Present();
}

bool ETHEngine::RunOnResumeFunction() const
{
	if (m_onResumeFunction)
	{
		ETHGlobal::ExecuteContext(m_pScriptContext, m_onResumeFunction);
		return true;
	}
	else
	{
		return false;
	}
}

void ETHEngine::Destroy()
{
	m_lastBGColor = m_provider->GetVideo()->GetBackgroundColor();
	m_provider->GetGraphicResourceManager()->ReleaseResources();
	m_provider->GetAudioResourceManager()->ReleaseResources();
}

void ETHEngine::ReleaseSpriteResources()
{
	m_provider->GetGraphicResourceManager()->ReleaseResources();
}

bool ETHEngine::PrepareScriptingEngine(const std::vector<std::string>& definedWords)
{
	m_pASEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if (!m_pASEngine)
	{
		ShowMessage(("Failed to create AngelScript engine."), ETH_ERROR);
		return false;
	}

	// Set UTF-8 encoding
	int r = m_pASEngine->SetEngineProperty(asEP_SCRIPT_SCANNER, 1);
	if (!CheckAngelScriptError((r < 0), ("Failed setting up script scanner.")))
		return false;

	if (!CheckAngelScriptError((r < 0), ("Failed while setting up string encoding")))
		return false;

	// Message callback
	r = m_pASEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	if (!CheckAngelScriptError((r < 0), ("Failed while setting message callback.")))
		return false;

	ETHGlobal::RegisterEnumTypes(m_pASEngine);
	ETHGlobal::RegisterGlobalProperties(m_pASEngine);
	ETHGlobal::RegisterAllObjects(m_pASEngine);
	RegisterGlobalFunctions(m_pASEngine);

	m_pScriptContext = m_pASEngine->CreateContext();

	// Exception callback
	r = m_pScriptContext->SetExceptionCallback(asFUNCTION(ExceptionCallback), 0, asCALL_CDECL);
	if (!CheckAngelScriptError((r < 0), ("Failed while setting exception callback.")))
		return false;

	if (!BuildModule(definedWords))
		return false;

	// Always collect all garbage by default
	SetFastGarbageCollector(false);
	
	return true;
}

static void RegisterDefinedWords(const std::vector<std::string>& definedWords, CScriptBuilder& builder, const bool isTesting)
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
		builder.DefineWord("MACOS");
	#endif
	#ifdef WIN32
		builder.DefineWord("WINDOWS");
	#endif
	#ifdef LINUX
		builder.DefineWord("LINUX");
	#endif
	#ifdef MOBILE
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

bool ETHEngine::BuildModule(const std::vector<std::string>& definedWords)
{
	const std::string resourcePath = m_provider->GetFileIOHub()->GetResourceDirectory();
	const std::string mainScript = resourcePath + ETH_DEFAULT_MAIN_SCRIPT_FILE;
	const std::string byteCodeWriteFile = m_provider->GetByteCodeSaveDirectory() + ETH_DEFAULT_MAIN_BYTECODE_FILE;
	const std::string byteCodeReadFile  = resourcePath + ETH_DEFAULT_MAIN_BYTECODE_FILE;

	// line separator to ease script output reading
	m_provider->Log(("____________________________\n"), Platform::Logger::LT_INFO);

	// if there's a main script file, load the source from text code and compile it
	if (ETHGlobal::FileExists(mainScript, m_provider->GetFileManager()))
	{
		ETH_STREAM_DECL(ssi) << ("Loading game script from source-code: ") << ETH_DEFAULT_MAIN_SCRIPT_FILE << std::endl;
		m_provider->Log(ssi.str(), Platform::Logger::LT_INFO);

		// Load the main script
		CScriptBuilder builder(m_provider);

		RegisterDefinedWords(definedWords, builder, m_testing);

		int r;
		r = builder.StartNewModule(m_pASEngine, ETH_SCRIPT_MODULE.c_str());
		if (!CheckAngelScriptError(r < 0, ("Failed while starting the new module.")))
			return false;

		r = builder.AddSectionFromFile(mainScript.c_str(), m_provider->GetFileIOHub()->GetResourceDirectory().c_str());
		std::stringstream ss;
		ss << ("Failed while loading the main script. Verify the ") << mainScript << (" file");
		if (!CheckAngelScriptError(r < 0, ss.str()))
			return false;

		// builds the module
		const VideoPtr& video = m_provider->GetVideo();
		const unsigned long buildTime = video->GetElapsedTime();
		r = builder.BuildModule();
		std::stringstream timeStringStream; timeStringStream << ("\nCompile time: ") << video->GetElapsedTime() - buildTime << (" milliseconds");
		m_provider->Log(timeStringStream.str(), Platform::Logger::LT_INFO);
		if (!CheckAngelScriptError(r < 0, ("Failed while building module.")))
			return false;

		// Gets the recently built module
		m_pASModule = builder.GetModule();

		// Writes the compiled byte code to file
		{
			ETHBinaryStream stream(m_provider->GetFileManager());
			if (stream.OpenW(byteCodeWriteFile))
			{
				m_pASModule->SaveByteCode(&stream);
				stream.CloseW();
				ETH_STREAM_DECL(ss) << ("ByteCode saved: ") << byteCodeWriteFile;
				m_provider->Log(ss.str(), Platform::Logger::LT_INFO);
			}
			else
			{
				ETH_STREAM_DECL(ss) << ("Failed while writing the byte code file ") << byteCodeWriteFile;
				m_provider->Log(ss.str(), Platform::Logger::LT_ERROR);
			}
		}
	}
	else // otherwise, try to load the bytecode
	{
		ETH_STREAM_DECL(ss) << ("Loading game script from pre-compiled byte code: ") << ETH_DEFAULT_MAIN_BYTECODE_FILE << std::endl;
		m_provider->Log(ss.str(), Platform::Logger::LT_INFO);
	
		m_pASModule = m_pASEngine->GetModule(ETH_SCRIPT_MODULE.c_str(), asGM_ALWAYS_CREATE);
		ETHBinaryStream stream(m_provider->GetFileManager());
		if (stream.OpenR(byteCodeReadFile))
		{
			if (m_pASModule->LoadByteCode(&stream) < 0)
			{
				ETH_STREAM_DECL(ss) << ("Couldn't load game script from pre-compiled byte code: ") << ETH_DEFAULT_MAIN_BYTECODE_FILE;
				m_provider->Log(ss.str(), Platform::Logger::LT_ERROR);
				stream.CloseR();
				return false;
			}
			stream.CloseR();
		}
		else
		{
			ETH_STREAM_DECL(ss) << ("Failed while reading the byte code file ") << byteCodeReadFile;
			m_provider->Log(ss.str(), Platform::Logger::LT_ERROR);
			Abort();
			return false;
		}
		m_provider->Log(("Finished loading bytecode"), Platform::Logger::LT_INFO);
	}

	return true;
}

asIScriptFunction* ETHEngine::GetMainFunction() const
{
	// finds the main function
	if (!m_pASModule)
	{
		CheckAngelScriptError(true /*error*/, ("Couldn't build scripting module."));
		return 0;
	}

	asIScriptFunction* mainFunc = m_pASModule->GetFunctionByName(ETH_MAIN_FUNCTION.c_str());
	ETH_STREAM_DECL(ss) << ("Function not found: ") << ETH_MAIN_FUNCTION;
	CheckAngelScriptError((!mainFunc), ss.str());
	return mainFunc;
}

bool ETHEngine::CheckAngelScriptError(const bool error, const std::string &description)
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

void ETHEngine::DrawTopLayer(const unsigned long lastFrameElapsedTimeMS)
{
	m_drawableManager.DrawTopLayer(lastFrameElapsedTimeMS, m_provider->GetVideo());
}

std::string ETHEngine::RemoveResourceDirectoryFromSectionString(const std::string& section)
{
	const std::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();
	if (section.find(resourceDirectory) == 0)
	{
		return section.substr(resourceDirectory.length(), std::string::npos);
	}
	else
	{
		return section;
	}
}

void ETHEngine::MessageCallback(const asSMessageInfo *msg)
{
	static int lastRow = msg->row;

	const bool differentRow = (msg->row != lastRow);
	lastRow = msg->row;

	std::string typeStr = ("");
	Platform::Logger::TYPE type;
	switch (msg->type)
	{
	case asMSGTYPE_WARNING:
		type = Platform::Logger::LT_WARNING;
		typeStr = ("WARN ");
		break;
	case asMSGTYPE_ERROR:
		type = Platform::Logger::LT_ERROR;
		typeStr = ("ERROR");
		break;
	default:
		typeStr = ("INFO ");
		type = Platform::Logger::LT_INFO;
	}

	const std::string section = RemoveResourceDirectoryFromSectionString(msg->section);

	std::stringstream ss;

	if (differentRow)
		ss << std::endl;

	ss << ("[") << typeStr << ("] ");
	ss << section << (", line ") << msg->row << (": ");
	ss << msg->message;

	m_provider->Log(ss.str(), type);
}

void ETHEngine::ExceptionCallback(asIScriptContext *ctx, void *param)
{
	UNUSED_ARGUMENT(param);

	asIScriptFunction* function = ctx->GetExceptionFunction();

	const std::string section = RemoveResourceDirectoryFromSectionString(function->GetScriptSectionName());
	std::stringstream ss;
	ss << ("Exception: ") << ctx->GetExceptionString() << std::endl << ("  Callstack:") << std::endl;

	for (std::size_t n = 0; n < ctx->GetCallstackSize(); n++)
	{
		asIScriptFunction* stackedFunction = ctx->GetFunction(static_cast<asUINT>(n));
		if (stackedFunction != NULL)
		{
			const std::string section = RemoveResourceDirectoryFromSectionString(stackedFunction->GetScriptSectionName());
			ss << ("    ") << stackedFunction->GetDeclaration()
			   << (" (") << section << (", ") << ctx->GetLineNumber(static_cast<asUINT>(n)) << (")") << std::endl;
		}
	}
	ss << std::endl;
	
	Application::SharedData.Set(
		SCRIPT_EXCEPTION_LOG_SHARED_DATA_KEY,
		Application::SharedData.Get(SCRIPT_EXCEPTION_LOG_SHARED_DATA_KEY) + ss.str());

	m_provider->Log(ss.str(), Platform::FileLogger::LT_ERROR);
}

void ETHEngine::Restore()
{
	Sprite::Finish();
	Sprite::Initialize(m_provider->GetVideo().get());
}

void ETHEngine::Resume()
{
	m_hasBeenResumed = true;
}
