#include "ETHResourceManager.h"

#include <Platform/Platform.h>

#include "ETHResourceProvider.h"

const gs2d::str_type::string ETHGraphicResourceManager::SD_EXPANSION_FILE_PATH = "com.ethanonengine.expansionFile.path";

static str_type::string RemoveResourceDirectory(
	const str_type::string& resourceDirectory,
	const str_type::string& fullOriginPath)
{
	str_type::string r = fullOriginPath, fixedResourceDirectory = resourceDirectory;
	Platform::FixSlashes(fixedResourceDirectory);
	if (r.find(fixedResourceDirectory) == 0)
		r = r.substr(fixedResourceDirectory.length(), str_type::string::npos);
	return r;
}

ETHGraphicResourceManager::SpriteResource::SpriteResource(
	const Platform::FileManagerPtr& fileManager,
	const str_type::string& resourceDirectory,
	const str_type::string& fullOriginPath,
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

	const str_type::string frameXmlFileName = Platform::RemoveExtension(fullOriginPath.c_str()) + ".xml";
	if (fileManager->FileExists(frameXmlFileName))
	{
		TiXmlDocument doc(frameXmlFileName);
		str_type::string content;
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
				TiXmlNode* pNode = pSprites->FirstChild(GS_L("sprite"));
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
							pSpriteIter->QueryIntAttribute(GS_L("x"), &x);
							pSpriteIter->QueryIntAttribute(GS_L("y"), &y);
							pSpriteIter->QueryIntAttribute(GS_L("w"), &width);
							pSpriteIter->QueryIntAttribute(GS_L("h"), &height);
							pSpriteIter->QueryIntAttribute(GS_L("oW"), &originalWidth);
							pSpriteIter->QueryIntAttribute(GS_L("oH"), &originalHeight);
							pSpriteIter->QueryIntAttribute(GS_L("oX"), &offsetX);
							pSpriteIter->QueryIntAttribute(GS_L("oY"), &offsetY);

							packedFrames->AddRect(
								gs2d::math::Rect2D(
									Vector2(float(x) / sheetSize.x, float(y) / sheetSize.y),
									Vector2(float(width) / sheetSize.x, float(height) / sheetSize.y),
									Vector2(offsetX, offsetY),
									Vector2(originalWidth, originalHeight)));
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
	m_resource.clear();
}

void ETHGraphicResourceManager::ReleaseTemporaryResources()
{
	std::list<str_type::string> deleteList;
	for (tsl::hopscotch_map<str_type::string, SpriteResource>::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
	{
		if (iter->second.IsTemporary())
			deleteList.push_back(iter->first);
	}
	for (std::list<str_type::string>::iterator iter = deleteList.begin(); iter != deleteList.end(); ++iter)
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
	const str_type::string &fileRelativePath,
	const str_type::string &resourceDirectory,
	const str_type::string &searchPath,
	const bool temporary)
{
	if (fileRelativePath == "")
	{
		return 0;
	}

	str_type::string fileName = Platform::GetFileName(fileRelativePath);
	str_type::string resourceFullPath = AssembleResourceFullPath(resourceDirectory, searchPath, fileName);

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
	if (searchPath != GS_L(""))
	{
		AddFile(fileManager, video, resourceFullPath, resourceDirectory, temporary);
		return FindSprite(resourceFullPath, fileName, resourceDirectory);
	}
	return 0;
}

SpritePtr ETHGraphicResourceManager::GetSprite(
	const Platform::FileManagerPtr& fileManager,
	VideoPtr video,
	const str_type::string &fileRelativePath,
	const str_type::string &resourceDirectory,
	const str_type::string &searchPath,
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

SpriteRectsPtr ETHGraphicResourceManager::GetPackedFrames(const str_type::string& fileName)
{
	tsl::hopscotch_map<str_type::string, SpriteResource>::iterator iter = m_resource.find(fileName);
	if (iter != m_resource.end())
	{
		return (iter->second.packedFrames);
	}
	else
	{
		return SpriteRectsPtr();
	}
}

ETHGraphicResourceManager::SpriteResource* ETHGraphicResourceManager::GetSpriteResource(const str_type::string& fileName)
{
	tsl::hopscotch_map<str_type::string, SpriteResource>::iterator iter = m_resource.find(fileName);
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
	const str_type::string &path,
	const str_type::string& resourceDirectory,
	const bool temporary)
{
	str_type::string fileName = Platform::GetFileName(path);
	{
		const SpriteResource* resource = FindSprite(path, fileName, resourceDirectory);
		if (resource)
		{
			return resource;
		}
	}

	SpritePtr pBitmap;
	str_type::string fixedName(path);
	Platform::FixSlashes(fixedName);

	ETHSpriteDensityManager::DENSITY_LEVEL densityLevel;
	const str_type::string finalFileName(m_densityManager.ChooseSpriteVersion(fixedName, video, densityLevel));

	if (!(pBitmap = SpritePtr(new Sprite(video.get(), finalFileName))))
	{
		pBitmap.reset();
		ETH_STREAM_DECL(ss) << GS_L("(Not loaded) ") << path;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		return 0;
	}

	m_densityManager.SetSpriteDensity(pBitmap, densityLevel);

	//#if defined(_DEBUG) || defined(DEBUG)
	ETH_STREAM_DECL(ss) << GS_L("(Loaded) ") << fileName;
	ETHResourceProvider::Log(ss.str(), Platform::Logger::INFO);
	//#endif
	m_resource.insert(std::pair<str_type::string, SpriteResource>(fileName, SpriteResource(fileManager, resourceDirectory, fixedName, pBitmap, temporary)));
	return FindSprite(path, fileName, resourceDirectory);
}

std::size_t ETHGraphicResourceManager::GetNumResources()
{
	return m_resource.size();
}

const ETHGraphicResourceManager::SpriteResource* ETHGraphicResourceManager::FindSprite(
	const str_type::string& fullFilePath,
	const str_type::string& fileName,
	const str_type::string& resourceDirectory)
{
	tsl::hopscotch_map<str_type::string, SpriteResource>::iterator iter = m_resource.find(fileName);
	if (iter != m_resource.end())
	{
		str_type::string fixedPath(fullFilePath);
		Platform::FixSlashes(fixedPath);
		const SpriteResource& resource = iter->second;
		if (RemoveResourceDirectory(resourceDirectory, fixedPath) != resource.m_fullOriginPath)
		{
			str_type::stringstream ss; ss << GS_L("Duplicate resource name found: ") << fixedPath
				<< GS_L(" <-> ") << resource.m_fullOriginPath;
			ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		}
		return &resource;
	}
	else
	{
		return 0;
	}
}

str_type::string ETHGraphicResourceManager::AssembleResourceFullPath(
	const str_type::string& programPath,
	const str_type::string& searchPath,
	const str_type::string& fileName )
{
	return programPath + searchPath + fileName;
}

bool ETHGraphicResourceManager::ReleaseResource(const str_type::string &file)
{
	tsl::hopscotch_map<str_type::string, SpriteResource>::iterator iter = m_resource.find(Platform::GetFileName(file));
	if (iter != m_resource.end())
	{
		str_type::string fileName = Platform::GetFileName(file);
		ETH_STREAM_DECL(ss) << GS_L("(Released) ") << fileName;
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
	tsl::hopscotch_map<str_type::string, AudioSamplePtr> musicTracks;
	for (tsl::hopscotch_map<str_type::string, AudioSamplePtr>::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
	{
		if ((iter->second)->GetType() == Audio::MUSIC)
			musicTracks[iter.key()] = iter.value();
	}
	m_resource.clear(); // probably just paranoia
	m_resource = musicTracks;
}

AudioSamplePtr ETHAudioResourceManager::GetPointer(
	AudioPtr audio,
	const Platform::FileIOHubPtr& fileIOHub,
	const str_type::string &fileRelativePath,
	const str_type::string &searchPath,
	const Audio::SAMPLE_TYPE type)
{
	if (fileRelativePath == GS_L(""))
		return AudioSamplePtr();

	if (!m_resource.empty())
	{
		str_type::string fileName = Platform::GetFileName(fileRelativePath);
		tsl::hopscotch_map<str_type::string, AudioSamplePtr>::iterator iter = m_resource.find(fileName);
		if (iter != m_resource.end())
			return iter.value();
	}

	// we can set a search path to search the file in case
	// it hasn't been loaded yet
	if (searchPath != GS_L(""))
	{
		str_type::string fileName = Platform::GetFileName(fileRelativePath);

		str_type::string path = fileIOHub->GetResourceDirectory();
		path += searchPath;
		path += fileName;
		AddFile(audio, fileIOHub, path, type);
		return GetPointer(audio, fileIOHub, fileName, GS_L(""), type);
	}
	return AudioSamplePtr();
}

AudioSamplePtr ETHAudioResourceManager::AddFile(
	AudioPtr audio,
	const Platform::FileIOHubPtr& fileIOHub,
	const str_type::string& path,
	const Audio::SAMPLE_TYPE type)
{
	if (!m_resource.empty())
	{
		str_type::string fileName = Platform::GetFileName(path);
		tsl::hopscotch_map<str_type::string, AudioSamplePtr>::iterator iter = m_resource.find(fileName);
		if (iter != m_resource.end())
			return iter.value();
	}

	AudioSamplePtr pSample;
	str_type::string fixedName(path);
	Platform::FixSlashes(fixedName);
	if (!(pSample = audio->LoadSampleFromFile(fixedName, fileIOHub->GetFileManager(), type)))
	{
		pSample.reset();
		ETH_STREAM_DECL(ss) << GS_L("(Not loaded) \"") << fixedName << GS_L("\"");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		return AudioSamplePtr();
	}
	//#if defined(_DEBUG) || defined(DEBUG)
	str_type::string fileName = Platform::GetFileName(path);
	ETH_STREAM_DECL(ss) << GS_L("(Loaded) ") << fileName;
	ETHResourceProvider::Log(ss.str(), Platform::Logger::INFO);
	//#endif
	m_resource[fileName] = pSample;
	return pSample;
}

std::size_t ETHAudioResourceManager::GetNumResources()
{
	return m_resource.size();
}
