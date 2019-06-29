#ifndef ETH_ENGINE_H_
#define ETH_ENGINE_H_

#include <BaseApplication.h>
#include "Resource/ETHResourceProvider.h"
#include "Script/ETHScriptWrapper.h"
#include "Shader/ETHShaderManager.h"

#define _ETH_DEFAULT_MAIN_SCRIPT_FILE (GS_L("main.angelscript"))

#ifdef WIN32
#define _ETH_DYNAMIC_LIBRARY_LIST GS_L("plugin.list")
typedef bool (__cdecl *ETH_PLUGIN_PROC)(asIScriptEngine*);
#define _ETH_PLUGIN_FUNCTION_NAME GS_L("ETHCall")
#endif

namespace gs2d {

class ETHEngine : public gs2d::BaseApplication, public ETHScriptWrapper
{
	Color m_lastBGColor;

	const str_type::string ETH_DEFAULT_MAIN_SCRIPT_FILE;
	const str_type::string ETH_DEFAULT_MAIN_BYTECODE_FILE;
	const str_type::string ETH_MAIN_FUNCTION;

	Platform::FileIOHubPtr m_fileIOHub;

	const bool m_testing, m_compileAndRun, m_autoStartScriptEngine;
	bool m_hasBeenResumed, m_scriptEngineReady, m_mainFunctionRunned;

	static const str_type::string SCRIPT_EXCEPTION_LOG_SHARED_DATA_KEY;
	static void MessageCallback(const asSMessageInfo* msg);
	static void ExceptionCallback(asIScriptContext* ctx, void* param);
	static bool CheckAngelScriptError(const bool error, const str_type::string& description);
	static str_type::string RemoveResourceDirectoryFromSectionString(const str_type::string& section);

	ETHEngine &operator=(const ETHEngine &other);

	bool PrepareScriptingEngine(const std::vector<gs2d::str_type::string>& definedWords);
	bool BuildModule(const std::vector<gs2d::str_type::string>& definedWords);
	asIScriptFunction* GetMainFunction() const;
	bool RunOnResumeFunction() const;
	bool RunFunction(asIScriptFunction* func) const;
	bool LoadNextSceneIfRequested();

	void DrawTopLayer(const unsigned long lastFrameElapsedTimeMS);

	std::vector<gs2d::str_type::string> m_definedWords;

public:

	static boost::shared_ptr<ETHEngine> Cast(gs2d::BaseApplicationPtr application);
	bool StartScriptEngine();
	bool IsScriptEngineLoaded() const;

	static const str_type::string ETH_SCRIPT_MODULE;

	ETHEngine(const bool testing, const bool compileAndRun, const bool autoStartScriptEngine);
	~ETHEngine();
	void Start(gs2d::VideoPtr video, gs2d::InputPtr input, gs2d::AudioPtr audio);
	gs2d::Application::APP_STATUS Update(const float lastFrameDeltaTimeMS); 
	void RenderFrame();
	void Destroy();
	void Restore();

	ETHResourceProviderPtr GetProvider();
};

typedef boost::shared_ptr<ETHEngine> ETHEnginePtr;

} // namespace gs2d

#endif
