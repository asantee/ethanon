#ifndef ETH_RESOURCE_MANAGER_H_
#define ETH_RESOURCE_MANAGER_H_

#include <Platform/FileIOHub.h>
#include <SpriteRects.h>
#include <Audio.h>

#include "../ETHTypes.h"
#include "ETHSpriteDensityManager.h"

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
	public:
		SpriteResource(
			const Platform::FileManagerPtr& fileManager,
			const str_type::string& resourceDirectory,
			const str_type::string& fullOriginPath,
			const SpritePtr& sprite,
			const bool temporary);
		bool IsTemporary() const;
		SpritePtr GetSprite() const;

		bool flipX, flipY;
		unsigned int frame;
		SpriteRectsPtr packedFrames;
	};

	const SpriteResource* GetPointer(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const str_type::string& fileRelativePath,
		const str_type::string& resourceDirectory,
		const str_type::string &searchPath,
		const bool temporary = false);

	SpritePtr GetSprite(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const str_type::string& fileRelativePath,
		const str_type::string& resourceDirectory,
		const str_type::string &searchPath,
		const bool temporary = false);

	SpriteRectsPtr GetPackedFrames(const str_type::string& fileName);
	SpriteResource* GetSpriteResource(const str_type::string& fileName);

	std::size_t GetNumResources();
	void ReleaseResources();
	void ReleaseTemporaryResources();

	const SpriteResource* AddFile(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const str_type::string& path,
		const str_type::string& resourceDirectory,
		const bool temporary);

	bool ReleaseResource(const str_type::string& file);

private:
	const SpriteResource* FindSprite(
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
