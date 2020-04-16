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
    static const std::string SD_EXPANSION_FILE_PATH;

	ETHGraphicResourceManager(const ETHSpriteDensityManager& densityManager);

	class SpriteResource
	{
		friend class ETHGraphicResourceManager;
		SpritePtr m_sprite;
		std::string m_fullOriginPath;
		bool m_temporary;
		bool m_customFramesXMLFound;
	public:
		SpriteResource(
			const Platform::FileManagerPtr& fileManager,
			const std::string& resourceDirectory,
			const std::string& fullOriginPath,
			const SpritePtr& sprite,
			const bool temporary);
		bool IsTemporary() const;
		bool IsCustomFramesXMLFound() const;
		SpritePtr GetSprite() const;

		bool flipX, flipY;
		unsigned int frame;
		SpriteRectsPtr packedFrames;
	};

	const SpriteResource* GetPointer(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const std::string& fileRelativePath,
		const std::string& resourceDirectory,
		const std::string &searchPath,
		const bool temporary = false);

	SpritePtr GetSprite(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const std::string& fileRelativePath,
		const std::string& resourceDirectory,
		const std::string &searchPath,
		const bool temporary = false);

	SpriteRectsPtr GetPackedFrames(const std::string& fileName);
	SpriteResource* GetSpriteResource(const std::string& fileName);

	std::size_t GetNumResources();
	void ReleaseResources();
	void ReleaseTemporaryResources();

	const SpriteResource* AddFile(
		const Platform::FileManagerPtr& fileManager,
		VideoPtr video,
		const std::string& path,
		const std::string& resourceDirectory,
		const bool temporary);

	bool ReleaseResource(const std::string& file);

private:
	const SpriteResource* FindSprite(
		const std::string& fullFilePath,
		const std::string& fileName,
		const std::string& resourceDirectory);

	std::string AssembleResourceFullPath(
		const std::string& programPath,
		const std::string& searchPath,
		const std::string& fileName);

	tsl::hopscotch_map<std::string, SpriteResource> m_resource;
	ETHSpriteDensityManager m_densityManager;
};

typedef boost::shared_ptr<ETHGraphicResourceManager> ETHGraphicResourceManagerPtr;

class ETHAudioResourceManager
{
public:
	ETHAudioResourceManager();

	AudioSamplePtr GetPointer(
		AudioPtr audio,
		const Platform::FileIOHubPtr& fileIOHub,
		const std::string &fileRelativePath,
		const std::string &alternative,
		const Audio::SAMPLE_TYPE type);

	AudioSamplePtr AddFile(
		AudioPtr audio,
		const Platform::FileIOHubPtr& fileIOHub,
		const std::string& path,
		const Audio::SAMPLE_TYPE type);

	void ReleaseResource(const std::string& file);

	std::size_t GetNumResources();
	
	void SetMusicVolume(const float volume);

	void ReleaseResources();

private:
	tsl::hopscotch_map<std::string, AudioSamplePtr> m_resource;
	float m_defaultMusicVolume;
};

typedef boost::shared_ptr<ETHAudioResourceManager> ETHAudioResourceManagerPtr;

#endif
