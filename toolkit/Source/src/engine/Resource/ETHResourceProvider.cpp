#include "ETHResourceProvider.h"

#include <Platform/Platform.h>

#define UNUSED_ARGUMENT(argument) ((void)(argument))

ETHGraphicResourceManagerPtr ETHResourceProvider::m_graphicResources;
ETHAudioResourceManagerPtr ETHResourceProvider::m_audioResources;
boost::shared_ptr<ETHShaderManager> ETHResourceProvider::m_shaderManager;
VideoPtr ETHResourceProvider::m_video;
AudioPtr ETHResourceProvider::m_audio;
InputPtr ETHResourceProvider::m_input;
Platform::FileIOHubPtr ETHResourceProvider::m_fileIOHub;

Platform::FileLoggerPtr ETHResourceProvider::m_logger(
	new Platform::FileLogger(Platform::FileLogger::GetLogDirectory() + ("eth.log.txt")));

ETHResourceProvider::ETHResourceProvider(
	ETHGraphicResourceManagerPtr graphicResources,
	ETHAudioResourceManagerPtr audioResources,
	boost::shared_ptr<ETHShaderManager> shaderManager,
	VideoPtr video,
	AudioPtr audio,
	InputPtr input,
	Platform::FileIOHubPtr fileIOHub)
{
	// all static
	m_graphicResources = graphicResources;
	m_audioResources = audioResources;
	m_shaderManager = shaderManager;
	m_video = video;
	m_audio = audio;
	m_input = input;
	m_fileIOHub = fileIOHub;
}

ETHResourceProvider& ETHResourceProvider::operator=(const ETHResourceProvider &tmp)
{
	// dummy... not allowed
	UNUSED_ARGUMENT(tmp);
	return *this;
}

void ETHResourceProvider::Log(const std::string& str, const Platform::Logger::TYPE& type)
{
	m_logger->Log(str, type);
}

const Platform::FileLogger* ETHResourceProvider::GetLogger() const
{
	return m_logger.get();
}

ETHGraphicResourceManagerPtr ETHResourceProvider::GetGraphicResourceManager()
{
	return m_graphicResources;
}

ETHAudioResourceManagerPtr ETHResourceProvider::GetAudioResourceManager()
{
	return m_audioResources;
}

boost::shared_ptr<ETHShaderManager> ETHResourceProvider::GetShaderManager()
{
	return m_shaderManager;
}

const VideoPtr& ETHResourceProvider::GetVideo()
{
	return m_video;
}

const AudioPtr& ETHResourceProvider::GetAudio()
{
	return m_audio;
}

const InputPtr& ETHResourceProvider::GetInput()
{
	return m_input;
}

std::string ETHResourceProvider::GetByteCodeSaveDirectory()
{
	return m_fileIOHub->GetExternalStorageDirectory();
}

const Platform::FileManagerPtr& ETHResourceProvider::GetFileManager()
{
	return m_fileIOHub->GetFileManager();
}

Platform::FileIOHubPtr ETHResourceProvider::GetFileIOHub()
{
	return m_fileIOHub;
}
