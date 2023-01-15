#ifndef ETH_RESOURCE_PROVIDER_H_
#define ETH_RESOURCE_PROVIDER_H_

#include "ETHResourceManager.h"

#include "../Platform/ETHPlatform.h"

#include <Platform/Platform.h>
#include <Platform/FileLogger.h>
#include <Platform/FileIOHub.h>
#include <Input.h>

class ETHShaderManager;

class ETHResourceProvider
{
	static ETHGraphicResourceManagerPtr m_graphicResources;
	static ETHAudioResourceManagerPtr m_audioResources;
	static boost::shared_ptr<ETHShaderManager> m_shaderManager;
	static VideoPtr m_video;
	static AudioPtr m_audio;
	static InputPtr m_input;
	static Platform::FileLoggerPtr m_logger;
	static Platform::FileIOHubPtr m_fileIOHub;

	ETHResourceProvider& operator=(const ETHResourceProvider &tmp);

public:
	ETHResourceProvider(
		ETHGraphicResourceManagerPtr graphicResources,
		ETHAudioResourceManagerPtr audioResources,
		boost::shared_ptr<ETHShaderManager> shaderManager,
		VideoPtr video,
		AudioPtr audio,
		InputPtr input,
		Platform::FileIOHubPtr fileIOHub);

	static void Log(const std::string& str, const Platform::Logger::TYPE& type);

	bool IsInEditor() const;

	void EnableLightmaps(const bool enable);
	bool AreLightmapsEnabled() const;

	void EnableRealTimeShadows(const bool enable);
	bool AreRealTimeShadowsEnabled() const;

	bool IsRichLightingEnabled() const;
	void SetRichLighting(const bool enable);

	const Platform::FileLogger* GetLogger() const;
	ETHGraphicResourceManagerPtr GetGraphicResourceManager();
	ETHAudioResourceManagerPtr GetAudioResourceManager();
	boost::shared_ptr<ETHShaderManager> GetShaderManager();
	const VideoPtr& GetVideo();
	const AudioPtr& GetAudio();
	const InputPtr& GetInput();
	std::string GetByteCodeSaveDirectory();
	const Platform::FileManagerPtr& GetFileManager();
	Platform::FileIOHubPtr GetFileIOHub();
};

#endif
