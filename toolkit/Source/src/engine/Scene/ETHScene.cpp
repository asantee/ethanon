#include "ETHScene.h"

#include "../Entity/ETHEntityArray.h"

#include "../Shader/ETHShaderManager.h"

#include "../Resource/ETHResourceProvider.h"
#include "../Resource/ETHDirectories.h"

#include "../Physics/ETHPhysicsSimulator.h"

#include "../../addons/scriptbuilder.h"

#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <assert.h>

int ETHScene::m_idCounter = 0;

ETHScene::ETHScene(
	const std::string& fileName,
	const std::string& lightmapDirectory,
	ETHResourceProviderPtr provider,
	const ETHSceneProperties& props,
	asIScriptModule *pModule,
	asIScriptContext *pContext,
	ETHEntityCache& entityCache,
	const Vector2& v2BucketSize) :
	m_renderingManager(provider),
	m_buckets(provider, v2BucketSize, true),
	m_activeEntityHandler(provider),
	m_physicsSimulator(provider->GetVideo()->GetFPSRate()),
	m_lightmapDirectory(lightmapDirectory)
{
	if (m_lightmapDirectory == "")
		m_lightmapDirectory = fileName;

	Init(provider, props, pModule, pContext);
	ETHEntityArray out;
	AddSceneFromFile(
		fileName,
		entityCache,
		AssembleEntityPath(),
		true /*readSceneProperties*/,
		Vector3(0.0f, 0.0f, 0.0f),
		out,
		false /*shouldGenerateNewID*/,
		false /*immediatelyLoadSprites*/);
	
	// TO-DO
}

ETHScene::ETHScene(
	const std::string& lightmapDirectory,
	ETHResourceProviderPtr provider,
	const ETHSceneProperties& props,
	asIScriptModule *pModule,
	asIScriptContext *pContext,
	const Vector2 &v2BucketSize) :
	m_lightmapDirectory(lightmapDirectory),
	m_renderingManager(provider),
	m_buckets(provider, v2BucketSize, true),
	m_activeEntityHandler(provider),
	m_physicsSimulator(provider->GetVideo()->GetFPSRate())
{
	Init(provider, props, pModule, pContext);
}

ETHScene::~ETHScene()
{
	for (std::list<ETHRenderEntity*>::iterator iter = m_persistentEntities.begin();
		iter != m_persistentEntities.end(); ++iter)
	{
		(*iter)->Release();
	}
	m_persistentEntities.clear();

	m_renderingManager.ReleaseMappedPieces();
}

void ETHScene::Init(ETHResourceProviderPtr provider, const ETHSceneProperties& props, asIScriptModule *pModule, asIScriptContext *pContext)
{
	m_provider = provider;
	m_sceneProps = props;
	m_pModule = pModule;
	m_pContext = pContext;
	m_idCounter = 0;
	m_minSceneHeight = 0.0f;
	m_nProcessedEntities = m_nRenderedPieces = 0;
	m_bucketClearenceFactor = 0.0f;
	m_maxSceneHeight = m_provider->GetVideo()->GetScreenSizeF().y;
	m_onCreatedFunctionExecuted = false;
}

std::string ETHScene::AssembleEntityPath() const
{
	return m_provider->GetFileIOHub()->GetResourceDirectory() + ETHDirectories::GetEntityDirectory();
}

std::string ETHScene::GetLightmapDirectory() const
{
	return m_lightmapDirectory;
}

void ETHScene::ClearResources()
{
	m_provider->GetGraphicResourceManager()->ReleaseResources();
	m_provider->GetAudioResourceManager()->ReleaseResources();
}

bool ETHScene::SaveToFile(const std::string& fileName, ETHEntityCache& entityCache)
{
	if (m_buckets.IsEmpty())
	{
		ETH_STREAM_DECL(ss) << ("ETHScene::Save: there are no entities to save: ") << fileName;
		m_provider->Log(ss.str(), Platform::FileLogger::LT_ERROR);
		return false;
	}

	// Write the header to the file
	TiXmlDocument doc;
	TiXmlDeclaration *pDecl = new TiXmlDeclaration(("1.0"), (""), (""));
	doc.LinkEndChild(pDecl);

	TiXmlElement *pElement = new TiXmlElement(("Ethanon"));
	doc.LinkEndChild(pElement);
	TiXmlElement *pRoot = doc.RootElement();

	// write the property header
	m_sceneProps.WriteToXMLFile(pRoot);

	// start writing entities
	TiXmlElement *pEntities = new TiXmlElement(("EntitiesInScene"));
	pRoot->LinkEndChild(pEntities);

	// Write every entity as an ordered bucket map
	ETHOrderedBucketMap map(m_buckets.GetFirstBucket(), m_buckets.GetLastBucket());
	for (ETHOrderedBucketMap::iterator bucketIter = map.begin(); bucketIter != map.end(); ++bucketIter)
	{
		ETHEntityList::const_iterator iEnd = bucketIter->second.end();
		for (ETHEntityList::iterator iter = bucketIter->second.begin(); iter != iEnd; ++iter)
		{
			(*iter)->WriteToXMLFile(
				pEntities,
				entityCache,
				m_provider->GetFileIOHub()->GetResourceDirectory() + ETHDirectories::GetEntityDirectory(),
				m_provider->GetFileManager());

			#if defined(_DEBUG) || defined(DEBUG)
			ETH_STREAM_DECL(ss) << ("Entity written to file: ") << (*iter)->GetEntityName();
			m_provider->Log(ss.str(), Platform::FileLogger::LT_INFO);
			#endif
		}
	}

	doc.SaveFile(fileName);
	return true;
}

bool ETHScene::AddSceneFromFile(
	const std::string& fileName,
	ETHEntityCache& entityCache,
	const std::string &entityPath,
	const bool readSceneProperties,
	const Vector3& offset,
	ETHEntityArray &outVector,
	const bool shouldGenerateNewID,
	const bool immediatelyLoadSprites)
{
	Platform::FileManagerPtr fileManager = m_provider->GetFileManager();

	if (!fileManager->FileExists(fileName))
	{
		ETH_STREAM_DECL(ss) << ("ETHScene::LoadFromFile: file not found (") << fileName << (")");
		m_provider->Log(ss.str(), Platform::FileLogger::LT_ERROR);
		return false;
	}

	// Read the header and check if the file is valid
	TiXmlDocument doc(fileName);
	std::string content;
	fileManager->GetUTFFileString(fileName, content);
	return AddSceneFromString(fileName, content, entityCache, entityPath, readSceneProperties, offset, outVector, shouldGenerateNewID, immediatelyLoadSprites);
}

bool ETHScene::AddSceneFromString(
	const std::string& fileName,
	const std::string& xmlContent,
	ETHEntityCache& entityCache,
	const std::string &entityPath,
	const bool readSceneProperties,
	const Vector3& offset,
	ETHEntityArray &outVector,
	const bool shouldGenerateNewID,
	const bool immediatelyLoadSprites)
{
	m_minSceneHeight = 0.0f;
	m_maxSceneHeight = m_provider->GetVideo()->GetScreenSizeF().y;

	// Read the header and check if the file is valid
	TiXmlDocument doc(fileName);
	if (!doc.LoadFile(xmlContent, TIXML_ENCODING_LEGACY))
	{
		ETH_STREAM_DECL(ss) << ("ETHScene::LoadFromFile: file found, but parsing failed (") << fileName << (")");
		m_provider->Log(ss.str(), Platform::FileLogger::LT_ERROR);
		return false;
	}

	TiXmlHandle hDoc(&doc);

	TiXmlElement *pElement = hDoc.FirstChildElement().Element();
	if (!pElement)
	{
		ETH_STREAM_DECL(ss) << ("ETHScene::LoadFromFile: couldn't find root element (") << fileName << (")");
		m_provider->Log(ss.str(), Platform::FileLogger::LT_ERROR);
		return false;
	}
	return AddEntitiesFromXMLFile(fileName, pElement, entityCache, entityPath, readSceneProperties, offset, outVector, shouldGenerateNewID, immediatelyLoadSprites);
}

bool ETHScene::AddEntitiesFromXMLFile(
	const std::string& fileName,
	TiXmlElement *pRoot,
	ETHEntityCache& entityCache,
	const std::string &entityPath,
	const bool readSceneProperties,
	const Vector3& offset,
	ETHEntityArray &outVector,
	const bool shouldGenerateNewIDs,
	const bool immediatelyLoadSprites)
{
	std::stringstream ss;
	const std::string sceneFileName = Platform::GetFileName(fileName.c_str());

	if (readSceneProperties)
	{
		m_sceneProps.ReadFromXMLFile(pRoot);
	}

	TiXmlNode *pNode = pRoot->FirstChild(("EntitiesInScene"));

	if (pNode)
	{
		TiXmlElement *pEntities = pNode->ToElement();
		if (pEntities)
		{
			pNode = pEntities->FirstChild(("Entity"));
			if (pNode)
			{
				TiXmlElement *pEntityIter = pNode->ToElement();
				if (pEntityIter)
				{
					do
					{
						bool shouldGenerateNewID = shouldGenerateNewIDs;
						if (shouldGenerateNewID)
						{
							int id = -1;
							pEntityIter->QueryIntAttribute(("id"), &id);
 							if (m_buckets.SeekEntity(id) == 0)
 							{
 								shouldGenerateNewID = false;
							}
						}

						ETHRenderEntity* entity = new ETHRenderEntity(pEntityIter, m_provider, entityCache, entityPath, m_lightmapDirectory, shouldGenerateNewID, immediatelyLoadSprites);
						entity->GetController()->AddToPos(offset);
						const ETHEntityProperties* entityProperties = entity->GetProperties();

						if (entityProperties->IsSuccessfullyLoaded())
						{
							AddEntity(entity);
							outVector.push_back(entity);
						}
						else
						{
							ss << std::endl
								<< sceneFileName
								<< (": couldn't load entity ")
								<< entityProperties->entityName
								<< ("(")
								<< entityPath
								<< entityProperties->entityName
								<< (")")
								<< std::endl;
						}

						pEntityIter = pEntityIter->NextSiblingElement();
					} while (pEntityIter);
				}
			}
		}
	}

	if (!ss.str().empty())
	{
		m_provider->Log(ss.str(), Platform::FileLogger::LT_ERROR);
	}
	return true;
}

int ETHScene::AddEntity(ETHRenderEntity* pEntity, const std::string& alternativeName)
{
	// sets an alternative name if there is any
	if (!alternativeName.empty())
	{
		pEntity->ChangeEntityName(alternativeName);
	}

	UpdateIDCounter(pEntity);

	// generate an unique id
	if (pEntity->GetID() < 0)
	{
		pEntity->SetID(m_idCounter);
	}

	m_buckets.Add(pEntity, ETHBucketManager::BACK);

	m_maxSceneHeight = Max(m_maxSceneHeight, pEntity->GetMaxHeight());
	m_minSceneHeight = Min(m_minSceneHeight, pEntity->GetMinHeight());

	// find a callback function for this one
	if (m_pContext && m_pModule)
	{
		AssignCallbackScript(pEntity);
	}

	// if it has a callback and is dynamic, or if it's temporary, add it to the "callback constant run list"
	m_activeEntityHandler.AddEntityWhenEligible(pEntity);

	return pEntity->GetID();
}

int ETHScene::AddEntity(ETHRenderEntity* pEntity)
{
	return AddEntity(pEntity, (""));
}

int ETHScene::UpdateIDCounter(ETHEntity* pEntity)
{
	return (m_idCounter = Max(pEntity->GetID() + 1, m_idCounter + 1));
}

const ETHSceneProperties* ETHScene::GetSceneProperties() const
{
	return &m_sceneProps;
}

ETHSceneProperties* ETHScene::GetEditableSceneProperties()
{
	return &m_sceneProps;
}

void ETHScene::SetSceneProperties(const ETHSceneProperties &prop)
{
	m_sceneProps = prop;
}

void ETHScene::LoadLightmapsFromBitmapFiles(const std::string& lightmapDirectory)
{
	m_lightmapDirectory = lightmapDirectory;
	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		// iterate over all entities in this bucket
		ETHEntityList& entityList = bucketIter.value();
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			ETHRenderEntity* entity = (*iter);
			entity->LoadLightmapFromFile(lightmapDirectory);
		}
	}
}

void ETHScene::Update(
	const float lastFrameElapsedTime,
	const ETHBackBufferTargetManagerPtr& backBuffer,
	asIScriptFunction* onCreateCallbackFunction,
	asIScriptFunction* onUpdateCallbackFunction)
{
	// Run onSceneCreated function
	if (onCreateCallbackFunction && !m_onCreatedFunctionExecuted)
	{
		ETHGlobal::ExecuteContext(m_pContext, onCreateCallbackFunction);
		m_onCreatedFunctionExecuted = true;
	}

	m_physicsSimulator.Update(lastFrameElapsedTime);

	// update entities that are always active (dynamic entities with callback or physics and temporary entities)
	m_activeEntityHandler.UpdateAlwaysActiveEntities(
		GetZAxisDirection(),
		m_sceneProps.parallaxIntensity,
		m_buckets,
		lastFrameElapsedTime * m_physicsSimulator.GetTimeStepScale());

	// Run onSceneUpdate functon
	if (onUpdateCallbackFunction)
	{
		ETHGlobal::ExecuteContext(m_pContext, onUpdateCallbackFunction);
	}

	// start mapping process
	float minHeight, maxHeight;

	assert(m_renderingManager.IsEmpty());

	// fill a map containing all entities we should render
	MapEntitiesToBeRendered(
		minHeight,
		maxHeight,
		backBuffer);

	// update static mapped entities that weren't called in UpdateAlwaysActiveEntities
	m_activeEntityHandler.UpdateCurrentFrameEntities(
		GetZAxisDirection(),
		m_sceneProps.parallaxIntensity,
		m_buckets,
		lastFrameElapsedTime * m_physicsSimulator.GetTimeStepScale());

	m_minSceneHeight = minHeight;
	m_maxSceneHeight = maxHeight;
}

void ETHScene::RenderScene(const ETHBackBufferTargetManagerPtr& backBuffer)
{
	// draw ambient pass
	DrawEntityMultimap(backBuffer);

	m_buckets.ResolveMoveRequests();
}

void ETHScene::EmptyRenderingQueue()
{
	m_renderingManager.ReleaseMappedPieces();
}

void ETHScene::FillCurrentlyVisibleBucketList(std::list<Vector2>& bucketList, const ETHBackBufferTargetManagerPtr& backBuffer)
{
	const VideoPtr& video = m_provider->GetVideo();
	const Vector2 clearence(GetBucketSize() * m_bucketClearenceFactor);
	const Vector2 min(video->GetCameraPos() - clearence);
	const Vector2 max(video->GetScreenSizeF() + (clearence * 2.0f));
	m_buckets.GetIntersectingBuckets(bucketList, min, max, IsDrawingBorderBuckets(), IsDrawingBorderBuckets());
}

void ETHScene::MapEntitiesToBeRendered(
	float &minHeight,
	float &maxHeight,
	const ETHBackBufferTargetManagerPtr& backBuffer)
{
	// store the max and min height to assign when everything is drawn
	maxHeight = m_maxSceneHeight;
	minHeight = m_minSceneHeight;

	m_nProcessedEntities = m_nRenderedPieces = 0;

	// don't let bucket size equal to 0
	const Vector2 bucketSize(GetBucketSize());
	assert(bucketSize.x != 0 || bucketSize.y != 0);

	// Gets the list of visible buckets
	std::list<Vector2> bucketList;
	FillCurrentlyVisibleBucketList(bucketList, backBuffer);

	assert(m_activeEntityHandler.IsCallbackListEmpty());

	// Loop through all visible Buckets
	for (std::list<Vector2>::iterator bucketPositionIter = bucketList.begin(); bucketPositionIter != bucketList.end(); ++bucketPositionIter)
	{
		ETHBucketMap::iterator bucketIter = m_buckets.Find(*bucketPositionIter);

		if (bucketIter == m_buckets.GetLastBucket())
			continue;

		ETHEntityList& entityList = bucketIter.value();
		if (entityList.empty())
			continue;

		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			ETHRenderEntity *entity = (*iter);

			// update scene bounding for depth buffer
			maxHeight = Max(maxHeight, entity->GetMaxHeight());
			minHeight = Min(minHeight, entity->GetMinHeight());

			if (entity->IsHidden())
				continue;

			// fill the callback list
			m_activeEntityHandler.AddStaticCallbackWhenEligible(entity);

			m_renderingManager.AddDecomposedPieces(
				entity,
				minHeight,
				maxHeight,
				backBuffer,
				m_sceneProps,
				m_nRenderedPieces);

			m_nProcessedEntities++;
		}
	}

	// Add persistent entities (the ones the user wants to force to render)
	FillMultimapAndClearPersistentList(bucketList, backBuffer);
}

void ETHScene::DrawEntityMultimap(const ETHBackBufferTargetManagerPtr& backBuffer)
{
	m_renderingManager.RenderAndReleasePieces(m_sceneProps, m_minSceneHeight, m_maxSceneHeight);
}

int ETHScene::GetNumProcessedEntities()
{
	return static_cast<int>(m_nProcessedEntities);
}

int ETHScene::GetNumRenderedPieces()
{
	return static_cast<int>(m_nRenderedPieces);
}

bool ETHScene::AssignCallbackScript(ETHSpriteEntity* entity)
{
	asIScriptFunction* callbackId = ETHGlobal::FindCallbackFunction(m_pModule, entity, ETH_CALLBACK_PREFIX, *m_provider->GetLogger());
	asIScriptFunction* constructorCallback = ETHGlobal::FindCallbackFunction(m_pModule, entity, ETH_CONSTRUCTOR_CALLBACK_PREFIX, *m_provider->GetLogger());
	AssignControllerToEntity(entity, callbackId, constructorCallback);
	return true;
} 

void ETHScene::AssignControllerToEntity(ETHEntity* entity, asIScriptFunction* callback, asIScriptFunction* constructorCallback)
{
	if (callback || constructorCallback)
	{
		ETHEntityControllerPtr currentController(entity->GetController());
		ETHEntityControllerPtr newController(new ETHRawEntityController(currentController, m_pContext, callback, constructorCallback));
		entity->SetController(newController);
	}
	if (entity->IsBody())
	{
		ETHEntityControllerPtr controller = m_physicsSimulator.CreatePhysicsController(entity, m_pModule, m_pContext);
		#if defined(_DEBUG) || defined(DEBUG)
			std::stringstream ss; Platform::Logger::TYPE logType;
			if (controller)
			{
				logType = Platform::Logger::LT_INFO;
				ss << ("Physics controller successfuly created: ");
			}
			else
			{
				logType = Platform::Logger::LT_ERROR;
				ss << ("Couldn't create physics controller: ");
			}
			ss << entity->GetEntityName() << (" (#") << entity->GetID() << (")");
			m_provider->Log(ss.str(), logType);
		#endif
		entity->SetController(controller);
	}
}

bool ETHScene::AddFloatData(const std::string &entity, const std::string &name, const float value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHFloatData(value)));
}

bool ETHScene::AddIntData(const std::string &entity, const std::string &name, const int value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHIntData(value)));
}

bool ETHScene::AddUIntData(const std::string &entity, const std::string &name, const unsigned int value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHUIntData(value)));
}

bool ETHScene::AddStringData(const std::string &entity, const std::string &name, const std::string &value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHStringData(value)));
}

bool ETHScene::AddVector2Data(const std::string &entity, const std::string &name, const Vector2 &value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHVector2Data(value)));
}

bool ETHScene::AddVector3Data(const std::string &entity, const std::string &name, const Vector3 &value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHVector3Data(value)));
}

bool ETHScene::AddCustomData(const std::string &entity, const std::string &name, const ETHCustomDataConstPtr &inData)
{
	unsigned int count = 0;
	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		ETHEntityList::const_iterator iEnd = bucketIter->second.end();
		for (ETHEntityList::iterator iter = bucketIter.value().begin(); iter != iEnd; ++iter)
		{
			ETHSpriteEntity *pEntity = (*iter);
			if (entity == pEntity->GetEntityName())
			{
				pEntity->AddData(name, inData);
				count++;
			}
		}
	}
	return (count > 0);
}

void ETHScene::GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace)
{
	m_buckets.GetIntersectingEntities(v2Here, outVector, screenSpace, m_sceneProps);
}

void ETHScene::SetBucketClearenceFactor(const float factor)
{
	m_bucketClearenceFactor = factor;
}

float ETHScene::GetBucketClearenceFactor() const
{
	return m_bucketClearenceFactor;
}

void ETHScene::SetAmbientLight(const Vector3 &color)
{
	m_sceneProps.ambient = color;
}

Vector3 ETHScene::GetAmbientLight() const
{
	return m_sceneProps.ambient;
}

void ETHScene::SetBorderBucketsDrawing(const bool enable)
{
	m_buckets.SetBorderBucketsDrawing(enable);
}

bool ETHScene::IsDrawingBorderBuckets() const
{
	return m_buckets.IsDrawingBorderBuckets();
}

void ETHScene::SetZAxisDirection(const Vector2 &v2)
{
	m_sceneProps.zAxisDirection = v2;
}

Vector2 ETHScene::GetZAxisDirection() const
{
	return m_sceneProps.zAxisDirection;
}

void ETHScene::SetParallaxIntensity(const float intensity)
{
	m_sceneProps.parallaxIntensity = intensity;
}

float ETHScene::GetParallaxIntensity() const
{
	return m_sceneProps.parallaxIntensity;
}

int ETHScene::GetLastID() const
{
	return m_idCounter;
}

float ETHScene::GetMaxHeight() const
{
	return m_maxSceneHeight;
}

float ETHScene::GetMinHeight() const
{
	return m_minSceneHeight;
}

Vector2 ETHScene::GetBucketSize() const
{
	return m_buckets.GetBucketSize();
}

ETHBucketManager& ETHScene::GetBucketManager()
{
	return m_buckets;
}

unsigned int ETHScene::GetNumEntities() const
{
	return m_buckets.GetNumEntities();
}

void ETHScene::RecoverResources()
{
	ETHEntityArray entities;
	m_buckets.GetEntityArray(entities);
	for (std::size_t t = 0; t < entities.size(); t++)
	{
		entities[static_cast<uint64_t>(t)]->RecoverResources();
	}
}

ETHPhysicsSimulator& ETHScene::GetSimulator()
{
	return m_physicsSimulator;
}

void ETHScene::ResolveJoints()
{
	ETHEntityArray entities;
	m_buckets.GetEntityArray(entities);
	m_physicsSimulator.ResolveJoints(entities);
}

bool ETHScene::DeleteEntity(ETHEntity *pEntity)
{
	return m_buckets.DeleteEntity(pEntity, ETHBucketManager::GetBucket(pEntity->GetPositionXY(), GetBucketSize()));
}

void ETHScene::ScaleEntities(const ETHEntityArray& entities, ETHBucketManager& buckets, const float scale, const bool scalePosition)
{
	if (scale == 1.0f)
		return;

	for (unsigned int t = 0; t < entities.size(); t++)
	{
		entities[t]->Scale(scale);
		if (scalePosition)
		{
			entities[t]->SetPosition(entities[t]->GetPosition() * scale, buckets);
		}
	}
	buckets.ResolveMoveRequests();
}

void ETHScene::AddEntityToPersistentList(ETHRenderEntity* entity)
{
	entity->AddRef();
	m_persistentEntities.push_back(entity);
}

void ETHScene::FillMultimapAndClearPersistentList(
	const std::list<Vector2>& currentBucketList,
	const ETHBackBufferTargetManagerPtr& backBuffer)
{
	if (m_persistentEntities.empty())
		return;

	for (std::list<ETHRenderEntity*>::iterator iter = m_persistentEntities.begin();
		iter != m_persistentEntities.end(); ++iter)
	{
		ETHRenderEntity* entity = *iter;
		const Vector2& currentBucket = entity->GetCurrentBucket(m_buckets);

		// only add decomposed pieces if bucket has not already been processed
		const bool bucketNotProcessed = std::find(currentBucketList.begin(), currentBucketList.end(), currentBucket) == currentBucketList.end();
		if (bucketNotProcessed)
		{
			m_renderingManager.AddDecomposedPieces(
				entity,
				m_minSceneHeight,
				m_maxSceneHeight,
				backBuffer,
				m_sceneProps,
				m_nRenderedPieces);

			m_nProcessedEntities++;
		}
		entity->Release();
	}
	m_persistentEntities.clear();
}

std::string ETHScene::ConvertSceneFileNameToLightmapDirectory(std::string filePath)
{
	std::string fileName = Platform::GetFileName(filePath);
	for (std::size_t t = 0; t < fileName.length(); t++)
	{
		if (fileName[t] == ('.'))
		{
			fileName[t] = ('-');
		}
	}
	
	const std::string directory(fileName + ("/"));
	std::string r = std::string(Platform::GetFileDirectory(filePath.c_str())).append(directory);
	return r;
}
