#include "ETHResourceManager.h"

#include <Platform/Platform.h>

#include "ETHResourceProvider.h"

const std::string ETHGraphicResourceManager::SD_EXPANSION_FILE_PATH = "com.ethanonengine.expansionFile.path";

static std::string RemoveResourceDirectory(
	const std::string& resourceDirectory,
	const std::string& fullOriginPath)
{
	std::string r = fullOriginPath, fixedResourceDirectory = resourceDirectory;
	Platform::FixSlashes(fixedResourceDirectory);
	if (r.find(fixedResourceDirectory) == 0)
		r = r.substr(fixedResourceDirectory.length(), std::string::npos);
	return r;
}

ETHGraphicResourceManager::SpriteResource::SpriteResource(
	const Platform::FileManagerPtr& fileManager,
	const std::string& resourceDirectory,
	const std::string& fullOriginPath,
	const SpritePtr& sprite,
	const bool temporary) :
	m_sprite(sprite),
	m_temporary(temporary),
	m_fullOriginPath(RemoveResourceDirectory(resourceDirectory, fullOriginPath)),
	flipX(false),
	flipY(false),
	frame(0),
	m_customFramesXMLFound(false)
{
	if (!fileManager)
		return;

	const std::string frameXmlFileName = Platform::RemoveExtension(fullOriginPath.c_str()) + ".xml";
	if (fileManager->FileExists(frameXmlFileName))
	{
		TiXmlDocument doc(frameXmlFileName);
		std::string content;
		fileManager->GetUTFFileString(frameXmlFileName, content);

		if (!doc.LoadFile(content, TIXML_ENCODING_LEGACY))
		{
			return;
		}
		
		TiXmlHandle hDoc(&doc);
		
		TiXmlElement *pRoot = hDoc.FirstChildElement().Element();

		if (pRoot)
		{
			TiXmlElement *pSprites = pRoot->ToElement();
			if (pSprites)
			{
				TiXmlNode* pNode = pSprites->FirstChild(("sprite"));
				if (pNode)
				{
					TiXmlElement *pSpriteIter = pNode->ToElement();
					if (pSpriteIter)
					{
						m_customFramesXMLFound = true;
						const Vector2 sheetSize(sprite->GetSize(Rect2D()));
						packedFrames = SpriteRectsPtr(new SpriteRects());
						do
						{
							int x, y, width, height, offsetX, offsetY, originalWidth, originalHeight;
							pSpriteIter->QueryIntAttribute(("x"), &x);
							pSpriteIter->QueryIntAttribute(("y"), &y);
							pSpriteIter->QueryIntAttribute(("w"), &width);
							pSpriteIter->QueryIntAttribute(("h"), &height);
							pSpriteIter->QueryIntAttribute(("oW"), &originalWidth);
							pSpriteIter->QueryIntAttribute(("oH"), &originalHeight);
							pSpriteIter->QueryIntAttribute(("oX"), &offsetX);
							pSpriteIter->QueryIntAttribute(("oY"), &offsetY);

							packedFrames->AddRect(
								gs2d::math::Rect2D(
									Vector2(float(x) / sheetSize.x, float(y) / sheetSize.y),
									Vector2(float(width) / sheetSize.x, float(height) / sheetSize.y),
									Vector2((float)offsetX, (float)offsetY),
									Vector2((float)originalWidth, (float)originalHeight)));
							pSpriteIter = pSpriteIter->NextSiblingElement();
						} while (pSpriteIter);
					}
				}
			}
		}
	}
}

bool ETHGraphicResourceManager::SpriteResource::IsTemporary() const
{
	return m_temporary;
}

bool ETHGraphicResourceManager::SpriteResource::IsCustomFramesXMLFound() const
{
	return m_customFramesXMLFound;
}

SpritePtr ETHGraphicResourceManager::SpriteResource::GetSprite() const
{
	return m_sprite;
}

void ETHGraphicResourceManager::ReleaseResources()
{
	for (tsl::hopscotch_map<std::string, SpriteResource>::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
	{
		iter->second.GetSprite()->GetTexture()->Free();
	}
	m_resource.clear();
}

void ETHGraphicResourceManager::ReleaseTemporaryResources()
{
	std::list<std::string> deleteList;
	for (tsl::hopscotch_map<std::string, SpriteResource>::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
	{
		if (iter->second.IsTemporary())
			deleteList.push_back(iter->first);
	}
	for (std::list<std::string>::iterator iter = deleteList.begin(); iter != deleteList.end(); ++iter)
	{
		m_resource.erase(*iter);
	}
}

ETHGraphicResourceManager::ETHGraphicResourceManager(const ETHSpriteDensityManager& densityManager) :
	m_densityManager(densityManager)
{
}

const ETHGraphicResourceManager::SpriteResource* ETHGraphicResourceManager::GetPointer(
	const Platform::FileManagerPtr& fileManager,
	VideoPtr video,
	const std::string &fileRelativePath,
	const std::string &resourceDirectory,
	const std::string &searchPath,
	const bool temporary)
{
	if (fileRelativePath == "")
	{
		return 0;
	}

	std::string fileName = Platform::GetFileName(fileRelativePath);
	std::string resourceFullPath = AssembleResourceFullPath(resourceDirectory, searchPath, fileName);

	if (!m_resource.empty())
	{
		const SpriteResource* resource = FindSprite(resourceFullPath, fileName, resourceDirectory);
		if (resource)
		{
			return resource;
		}
	}

	// we can set a search path to search the file in case
	// it hasn't been loaded yet
	if (searchPath != (""))
	{
		AddFile(fileManager, video, resourceFullPath, resourceDirectory, temporary);
		return FindSprite(resourceFullPath, fileName, resourceDirectory);
	}
	return 0;
}

SpritePtr ETHGraphicResourceManager::GetSprite(
	const Platform::FileManagerPtr& fileManager,
	VideoPtr video,
	const std::string &fileRelativePath,
	const std::string &resourceDirectory,
	const std::string &searchPath,
	const bool temporary)
{
	const SpriteResource* resource = GetPointer(fileManager, video, fileRelativePath, resourceDirectory, searchPath, temporary);
	if (resource)
	{
		return resource->GetSprite();
	}
	else
	{
		return SpritePtr();
	}
}

SpriteRectsPtr ETHGraphicResourceManager::GetPackedFrames(const std::string& fileName)
{
	tsl::hopscotch_map<std::string, SpriteResource>::iterator iter = m_resource.find(fileName);
	if (iter != m_resource.end())
	{
		return (iter->second.packedFrames);
	}
	else
	{
		return SpriteRectsPtr();
	}
}

ETHGraphicResourceManager::SpriteResource* ETHGraphicResourceManager::GetSpriteResource(const std::string& fileName)
{
	tsl::hopscotch_map<std::string, SpriteResource>::iterator iter = m_resource.find(fileName);
	if (iter != m_resource.end())
	{
		return &iter.value();
	}
	else
	{
		return 0;
	}
}

const ETHGraphicResourceManager::SpriteResource* ETHGraphicResourceManager::AddFile(
	const Platform::FileManagerPtr& fileManager,
	VideoPtr video,
	const std::string &path,
	const std::string& resourceDirectory,
	const bool temporary)
{
	std::string fileName = Platform::GetFileName(path);
	{
		const SpriteResource* resource = FindSprite(path, fileName, resourceDirectory);
		if (resource)
		{
			return resource;
		}
	}

	SpritePtr pBitmap;
	std::string fixedName(path);
	Platform::FixSlashes(fixedName);

	ETHSpriteDensityManager::DENSITY_LEVEL densityLevel;
	const std::string finalFileName(m_densityManager.ChooseSpriteVersion(fixedName, video, densityLevel));

	if (!(pBitmap = SpritePtr(new Sprite(video.get(), finalFileName))))
	{
		pBitmap.reset();
		ETH_STREAM_DECL(ss) << ("(Not loaded) ") << path;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		return 0;
	}

	m_densityManager.SetSpriteDensity(pBitmap, densityLevel);

	ETH_STREAM_DECL(ss) << ("(Loaded) ") << fileName;
	ETHResourceProvider::Log(ss.str(), Platform::Logger::INFO);
	m_resource.insert(std::pair<std::string, SpriteResource>(fileName, SpriteResource(fileManager, resourceDirectory, fixedName, pBitmap, temporary)));
	return FindSprite(path, fileName, resourceDirectory);
}

std::size_t ETHGraphicResourceManager::GetNumResources()
{
	return m_resource.size();
}

const ETHGraphicResourceManager::SpriteResource* ETHGraphicResourceManager::FindSprite(
	const std::string& fullFilePath,
	const std::string& fileName,
	const std::string& resourceDirectory)
{
	tsl::hopscotch_map<std::string, SpriteResource>::iterator iter = m_resource.find(fileName);
	if (iter != m_resource.end())
	{
		std::string fixedPath(fullFilePath);
		Platform::FixSlashes(fixedPath);
		const SpriteResource& resource = iter->second;
		if (RemoveResourceDirectory(resourceDirectory, fixedPath) != resource.m_fullOriginPath)
		{
			std::stringstream ss; ss << ("Duplicate resource name found: ") << fixedPath
				<< (" <-> ") << resource.m_fullOriginPath;
			ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		}
		return &resource;
	}
	else
	{
		return 0;
	}
}

std::string ETHGraphicResourceManager::AssembleResourceFullPath(
	const std::string& programPath,
	const std::string& searchPath,
	const std::string& fileName )
{
	return programPath + searchPath + fileName;
}

bool ETHGraphicResourceManager::ReleaseResource(const std::string &file)
{
	tsl::hopscotch_map<std::string, SpriteResource>::iterator iter = m_resource.find(Platform::GetFileName(file));
	if (iter != m_resource.end())
	{
		std::string fileName = Platform::GetFileName(file);
		ETH_STREAM_DECL(ss) << ("(Released) ") << fileName;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::INFO);
		m_resource.erase(iter);
		return true;
	}
	else
	{
		return false;
	}
}

void ETHAudioResourceManager::ReleaseResources()
{
	ReleaseAllButMusic();
}

void ETHAudioResourceManager::ReleaseAllButMusic()
{
	tsl::hopscotch_map<std::string, AudioSamplePtr> musicTracks;
	for (tsl::hopscotch_map<std::string, AudioSamplePtr>::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
	{
		if ((iter->second)->GetType() == Audio::MUSIC)
			musicTracks[iter.key()] = iter.value();
	}
	m_resource.clear(); // probably just paranoia
	m_resource = musicTracks;
}

void ETHAudioResourceManager::SetMusicVolume(const float volume)
{
	m_default_music_volume = volume;
	for (tsl::hopscotch_map<std::string, AudioSamplePtr>::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
		if ((iter->second)->GetType() == Audio::MUSIC)
			iter.value()->SetVolume(m_default_music_volume);
}

AudioSamplePtr ETHAudioResourceManager::GetPointer(
	AudioPtr audio,
	const Platform::FileIOHubPtr& fileIOHub,
	const std::string &fileRelativePath,
	const std::string &searchPath,
	const Audio::SAMPLE_TYPE type)
{
	if (fileRelativePath == (""))
		return AudioSamplePtr();

	if (!m_resource.empty())
	{
		std::string fileName = Platform::GetFileName(fileRelativePath);
		tsl::hopscotch_map<std::string, AudioSamplePtr>::iterator iter = m_resource.find(fileName);
		if (iter != m_resource.end())
			return iter.value();
	}

	// we can set a search path to search the file in case
	// it hasn't been loaded yet
	if (searchPath != (""))
	{
		std::string fileName = Platform::GetFileName(fileRelativePath);

		std::string path = fileIOHub->GetResourceDirectory();
		path += searchPath;
		path += fileName;
		AddFile(audio, fileIOHub, path, type);
		return GetPointer(audio, fileIOHub, fileName, (""), type);
	}
	return AudioSamplePtr();
}

AudioSamplePtr ETHAudioResourceManager::AddFile(
	AudioPtr audio,
	const Platform::FileIOHubPtr& fileIOHub,
	const std::string& path,
	const Audio::SAMPLE_TYPE type)
{
	if (!m_resource.empty())
	{
		std::string fileName = Platform::GetFileName(path);
		tsl::hopscotch_map<std::string, AudioSamplePtr>::iterator iter = m_resource.find(fileName);
		if (iter != m_resource.end())
			return iter.value();
	}

	AudioSamplePtr pSample;
	std::string fixedName(path);
	Platform::FixSlashes(fixedName);
	if (!(pSample = audio->LoadSampleFromFile(fixedName, fileIOHub->GetFileManager(), type)))
	{
		pSample.reset();
		ETH_STREAM_DECL(ss) << ("(Not loaded) \"") << fixedName << ("\"");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		return AudioSamplePtr();
	}
	if (type == Audio::MUSIC)
		pSample->SetVolume(m_default_music_volume);

	std::string fileName = Platform::GetFileName(path);
	ETH_STREAM_DECL(ss) << ("(Loaded) ") << fileName;
	ETHResourceProvider::Log(ss.str(), Platform::Logger::INFO);
	m_resource[fileName] = pSample;
	return pSample;
}

std::size_t ETHAudioResourceManager::GetNumResources()
{
	return m_resource.size();
}
