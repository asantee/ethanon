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

#ifndef ETH_RESOURCE_MANAGER_H_
#define ETH_RESOURCE_MANAGER_H_

#include <Platform/FileIOHub.h>
#include "../ETHTypes.h"
#include "ETHSpriteDensityManager.h"
#include <Audio.h>

class ETHGraphicResourceManager
{
public:
	ETHGraphicResourceManager(const ETHSpriteDensityManager& densityManager);

	class SpriteResource
	{
		friend class ETHGraphicResourceManager;
		SpritePtr m_sprite;
		str_type::string m_fullOriginPath;
	public:
		SpriteResource(
			const str_type::string& resourceDirectory,
			const str_type::string& fullOriginPath,
			const SpritePtr& sprite);
	};

	SpritePtr GetPointer(
		VideoPtr video,
		const str_type::string& fileRelativePath,
		const str_type::string& resourceDirectory,
		const str_type::string &searchPath,
		const bool cutOutBlackPixels);

	int GetNumResources();
	void ReleaseResources();

	SpritePtr AddFile(
		VideoPtr video,
		const str_type::string& path,
		const str_type::string& resourceDirectory,
		const bool cutOutBlackPixels);

	void RemoveResource(const str_type::string &file);

private:
	SpritePtr FindSprite(
		const str_type::string& fullFilePath,
		const str_type::string& fileName,
		const str_type::string& resourceDirectory);

	str_type::string AssembleResourceFullPath(
		const str_type::string& programPath,
		const str_type::string& searchPath,
		const str_type::string& fileName);

	std::map<str_type::string, SpriteResource> m_resource;
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

	int GetNumResources();

	void ReleaseResources();

private:
	std::map<str_type::string, AudioSamplePtr> m_resource;
	void ReleaseAllButMusic();
};

typedef boost::shared_ptr<ETHAudioResourceManager> ETHAudioResourceManagerPtr;

#endif
