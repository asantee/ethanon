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

#ifndef ETH_RESOURCE_MANAGER_H_
#define ETH_RESOURCE_MANAGER_H_

#include <Platform/FileIOHub.h>

#include "../ETHTypes.h"

#include "ETHSpriteDensityManager.h"

#include <Audio.h>

class ETHGraphicResourceManager
{
public:
    static const gs2d::str_type::string SD_EXPANSION_FILE_PATH;

	ETHGraphicResourceManager(const ETHSpriteDensityManager& densityManager);

	class SpriteResource
	{
		friend class ETHGraphicResourceManager;
		SpritePtr m_sprite;
		str_type::string m_fullOriginPath;
		bool m_temporary;
		Sprite::RectsPtr m_packedFrames;
	public:
		SpriteResource(
			const Platform::FileManagerPtr& fileManager,
			const str_type::string& resourceDirectory,
			const str_type::string& fullOriginPath,
			const SpritePtr& sprite,
			const bool temporary);
		bool IsTemporary() const;
	};

	SpritePtr GetPointer(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const str_type::string& fileRelativePath,
		const str_type::string& resourceDirectory,
		const str_type::string &searchPath,
		const bool cutOutBlackPixels,
		const bool temporary = false);

	Sprite::RectsPtr GetPackedFrames(const str_type::string& fileName);
	SpriteResource GetSpriteResource(const str_type::string& fileName);

	std::size_t GetNumResources();
	void ReleaseResources();
	void ReleaseTemporaryResources();

	SpritePtr AddFile(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const str_type::string& path,
		const str_type::string& resourceDirectory,
		const bool cutOutBlackPixels,
		const bool temporary);

	bool ReleaseResource(const str_type::string& file);

private:
	SpritePtr FindSprite(
		const str_type::string& fullFilePath,
		const str_type::string& fileName,
		const str_type::string& resourceDirectory);

	str_type::string AssembleResourceFullPath(
		const str_type::string& programPath,
		const str_type::string& searchPath,
		const str_type::string& fileName);

	tsl::hopscotch_map<str_type::string, SpriteResource> m_resource;
	ETHSpriteDensityManager m_densityManager;
};

typedef boost::shared_ptr<ETHGraphicResourceManager> ETHGraphicResourceManagerPtr;

class ETHAudioResourceManager
{
public:
	AudioSamplePtr GetPointer(
		AudioPtr audio,
		const Platform::FileIOHubPtr& fileIOHub,
		const str_type::string &fileRelativePath,
		const str_type::string &alternative,
		const Audio::SAMPLE_TYPE type);

	AudioSamplePtr AddFile(
		AudioPtr audio,
		const Platform::FileIOHubPtr& fileIOHub,
		const str_type::string& path,
		const Audio::SAMPLE_TYPE type);

	std::size_t GetNumResources();

	void ReleaseResources();

private:
	tsl::hopscotch_map<str_type::string, AudioSamplePtr> m_resource;
	void ReleaseAllButMusic();
};

typedef boost::shared_ptr<ETHAudioResourceManager> ETHAudioResourceManagerPtr;

#endif
