/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "ETHResourceProvider.h"
#include <Platform/Platform.h>

ETHGraphicResourceManagerPtr ETHResourceProvider::m_graphicResources;
ETHAudioResourceManagerPtr ETHResourceProvider::m_audioResources;
boost::shared_ptr<ETHShaderManager> ETHResourceProvider::m_shaderManager;
VideoPtr ETHResourceProvider::m_video;
AudioPtr ETHResourceProvider::m_audio;
InputPtr ETHResourceProvider::m_input;
Platform::FileIOHubPtr ETHResourceProvider::m_fileIOHub;
bool ETHResourceProvider::m_enableLightmaps = true;
bool ETHResourceProvider::m_usingRTShadows = true;
bool ETHResourceProvider::m_richLighting = true;
SpritePtr ETHResourceProvider::m_outline;
SpritePtr ETHResourceProvider::m_invisibleEntSymbol;

Platform::FileLoggerPtr ETHResourceProvider::m_logger(
	new Platform::FileLogger(Platform::FileLogger::GetLogDirectory() + GS_L("eth.log.txt")));

ETHResourceProvider::ETHResourceProvider(
	ETHGraphicResourceManagerPtr graphicResources,
	ETHAudioResourceManagerPtr audioResources,
	boost::shared_ptr<ETHShaderManager> shaderManager,
	VideoPtr video,
	AudioPtr audio,
	InputPtr input,
	Platform::FileIOHubPtr fileIOHub,
	const bool isInEditor) :
	m_isInEditor(isInEditor)
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
	GS2D_UNUSED_ARGUMENT(tmp);
	return *this;
}

void ETHResourceProvider::Log(const str_type::string& str, const Platform::Logger::TYPE& type)
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

str_type::string ETHResourceProvider::GetByteCodeSaveDirectory()
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

void ETHResourceProvider::SetEditorSprites(SpritePtr outline, SpritePtr invisibleEntSymbol)
{
	m_outline = outline;
	m_invisibleEntSymbol = invisibleEntSymbol;
}

SpritePtr ETHResourceProvider::GetOutlineSprite()
{
	return m_outline;
}

SpritePtr ETHResourceProvider::GetInvisibleEntSymbol()
{
	return m_invisibleEntSymbol;
}

void ETHResourceProvider::EnableLightmaps(const bool enable)
{
	m_enableLightmaps = enable;
}

bool ETHResourceProvider::AreLightmapsEnabled() const
{
	return m_enableLightmaps;
}

bool ETHResourceProvider::IsRichLightingEnabled() const
{
	return m_richLighting;
}

void ETHResourceProvider::SetRichLighting(const bool enable)
{
	m_richLighting = enable;
}

void ETHResourceProvider::EnableRealTimeShadows(const bool enable)
{
	m_usingRTShadows = enable;
}

bool ETHResourceProvider::AreRealTimeShadowsEnabled() const
{
	return m_usingRTShadows;
}

bool ETHResourceProvider::IsInEditor() const
{
	return m_isInEditor;
}
