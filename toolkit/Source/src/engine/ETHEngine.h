#ifndef ETH_ENGINE_H_
#define ETH_ENGINE_H_

#include <BaseApplication.h>
#include "Resource/ETHResourceProvider.h"
#include "Script/ETHScriptWrapper.h"
#include "Shader/ETHShaderManager.h"

#define _ETH_DEFAULT_MAIN_SCRIPT_FILE ("main.angelscript")

namespace gs2d {

class ETHEngine : public gs2d::BaseApplication, public ETHScriptWrapper
{
	Color m_lastBGColor;

	const std::string ETH_DEFAULT_MAIN_SCRIPT_FILE;
	const std::string ETH_DEFAULT_MAIN_BYTECODE_FILE;
	const std::string ETH_MAIN_FUNCTION;

	Platform::FileIOHubPtr m_fileIOHub;

	const bool m_testing, m_compileAndRun, m_autoStartScriptEngine;
	bool m_hasBeenResumed, m_scriptEngineReady;

	static const std::string SD_CURRENT_TIME_MILLIS;

	static const std::string SCRIPT_EXCEPTION_LOG_SHARED_DATA_KEY;
	static void MessageCallback(const asSMessageInfo* msg);
	static void ExceptionCallback(asIScriptContext* ctx, void* param);
	static bool CheckAngelScriptError(const bool error, const std::string& description);
	static std::string RemoveResourceDirectoryFromSectionString(const std::string& section);

	ETHEngine &operator=(const ETHEngine &other);

	bool PrepareScriptingEngine(const std::vector<std::string>& definedWords);
	bool BuildModule(const std::vector<std::string>& definedWords);
	asIScriptFunction* GetMainFunction() const;
	bool RunOnResumeFunction() const;
	bool LoadNextSceneIfRequested();

	void DrawTopLayer(const unsigned long lastFrameElapsedTimeMS);

	std::vector<std::string> m_definedWords;
	
	SpritePtr m_loadingScreenSymbol;

public:

	static boost::shared_ptr<ETHEngine> Cast(gs2d::BaseApplicationPtr application);
	bool StartScriptEngine();
	bool IsScriptEngineLoaded() const;

	static const std::string ETH_SCRIPT_MODULE;

	ETHEngine(const bool testing, const bool compileAndRun, const bool autoStartScriptEngine);
	~ETHEngine();
	void Start(gs2d::VideoPtr video, gs2d::InputPtr input, gs2d::AudioPtr audio);
	gs2d::Application::APP_STATUS Update(const float lastFrameDeltaTimeMS); 
	void RenderFrame();
	void Destroy();
	void ReleaseSpriteResources();
	void Restore();
	void Resume();

	ETHResourceProviderPtr GetProvider();
};

typedef boost::shared_ptr<ETHEngine> ETHEnginePtr;

} // namespace gs2d

#endif
