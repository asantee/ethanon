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

#include "ETHScene.h"

#include "../Entity/ETHEntityArray.h"

#include "../Shader/ETHLightmapGen.h"
#include "../Shader/ETHShaderManager.h"

#include "../Resource/ETHResourceProvider.h"

#include "../Physics/ETHPhysicsSimulator.h"

#include "../Renderer/ETHEntitySpriteRenderer.h"
#include "../Renderer/ETHEntityParticleRenderer.h"
#include "../Renderer/ETHEntityHaloRenderer.h"

#ifdef GS2D_STR_TYPE_WCHAR
#include "../../addons/utf16/scriptbuilder.h"
#else
#include "../../addons/ansi/scriptbuilder.h"
#endif

#include <Math/Randomizer.h>

#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <map>
#include <assert.h>

ETHScene::ETHScene(
	const str_type::string& fileName,
	ETHResourceProviderPtr provider,
	const bool richLighting,
	const ETHSceneProperties& props,
	asIScriptModule *pModule,
	asIScriptContext *pContext,
	const bool isInEditor,
	const Vector2& v2BucketSize) :
	m_buckets(provider, v2BucketSize, true),
	m_activeEntityHandler(provider),
	m_richLighting(richLighting),
	m_isInEditor(isInEditor),
	m_physicsSimulator(provider->GetGlobalScaleManager(), provider->GetVideo()->GetFPSRate())
{
	Init(provider, props, pModule, pContext);
	LoadFromFile(fileName);
}

ETHScene::ETHScene(
	ETHResourceProviderPtr provider,
	const bool richLighting,
	const ETHSceneProperties& props,
	asIScriptModule *pModule,
	asIScriptContext *pContext,
	const bool isInEditor,
	const Vector2 &v2BucketSize) :
	m_buckets(provider, v2BucketSize, true),
	m_activeEntityHandler(provider),
	m_richLighting(richLighting),
	m_isInEditor(isInEditor),
	m_physicsSimulator(provider->GetGlobalScaleManager(), provider->GetVideo()->GetFPSRate())
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

	ReleaseMappedEntities(m_piecesToRender);
}

void ETHScene::Init(ETHResourceProviderPtr provider, const ETHSceneProperties& props, asIScriptModule *pModule, asIScriptContext *pContext)
{
	m_provider = provider;
	m_sceneProps = props;
	m_pModule = pModule;
	m_pContext = pContext;
	m_idCounter = 0;
	m_minSceneHeight = 0.0f;
	m_enableLightmaps = false;
	m_usingRTShadows = true;
	m_nCurrentLights = 0;
	m_nRenderedEntities = -1;
	m_showingLightmaps = true;
	m_rotatingHalos = false;
	m_enableZBuffer = true;
	m_maxSceneHeight = m_provider->GetVideo()->GetScreenSizeF().y;
	const ETHShaderManagerPtr& shaderManager = m_provider->GetShaderManager();
	shaderManager->SetParallaxIntensity(m_sceneProps.parallaxIntensity);
	m_destructorManager = ETHEntityDestructorManagerPtr(new ETHEntityDestructorManager(pContext));
	m_buckets.SetDestructionListener(m_destructorManager);
}

void ETHScene::ClearResources()
{
	m_provider->GetGraphicResourceManager()->ReleaseResources();
	m_provider->GetAudioResourceManager()->ReleaseResources();
}

bool ETHScene::SaveToFile(const str_type::string& fileName)
{
	if (m_buckets.IsEmpty())
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHScene::Save: there are no entities to save: ") << fileName;
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}

	// Write the header to the file
	TiXmlDocument doc;
	TiXmlDeclaration *pDecl = new TiXmlDeclaration(GS_L("1.0"), GS_L(""), GS_L(""));
	doc.LinkEndChild(pDecl);

	TiXmlElement *pElement = new TiXmlElement(GS_L("Ethanon"));
	doc.LinkEndChild(pElement);
	TiXmlElement *pRoot = doc.RootElement();

	// write the property header
	m_sceneProps.WriteToXMLFile(pRoot);

	// start writing entities
	TiXmlElement *pEntities = new TiXmlElement(GS_L("EntitiesInScene"));
	pRoot->LinkEndChild(pEntities);

	// Write every entity
	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		ETHEntityList::const_iterator iEnd = bucketIter->second.end();
		for (ETHEntityList::iterator iter = bucketIter->second.begin(); iter != iEnd; ++iter)
		{
			(*iter)->WriteToXMLFile(pEntities);
			#if defined(_DEBUG) || defined(DEBUG)
			ETH_STREAM_DECL(ss) << GS_L("Entity written to file: ") << (*iter)->GetEntityName();
			m_provider->Log(ss.str(), Platform::FileLogger::INFO);
			#endif
		}
	}

	doc.SaveFile(fileName);
	#ifdef GS2D_STR_TYPE_ANSI
	  m_provider->GetFileManager()->ConvertAnsiFileToUTF16LE(fileName);
	#endif
	return true;
}

bool ETHScene::LoadFromFile(const str_type::string& fileName)
{
	Platform::FileManagerPtr fileManager = m_provider->GetFileManager();

	if (!fileManager->FileExists(fileName))
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHScene::Open: file not found (") << fileName << GS_L(")");
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}
	m_minSceneHeight = 0.0f;
	m_maxSceneHeight = m_provider->GetVideo()->GetScreenSizeF().y;

	// Read the header and check if the file is valid
	TiXmlDocument doc(fileName);
	str_type::string content;
	fileManager->GetUTF16FileString(fileName, content);
	if (!doc.LoadFile(content, TIXML_ENCODING_LEGACY))
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHScene::Open: file found, but parsing failed (") << fileName << GS_L(")");
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	TiXmlElement *pElement = hDoc.FirstChildElement().Element();
	if (!pElement)
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHScene::Open: couldn't find root element (") << fileName << GS_L(")");
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}
	return ReadFromXMLFile(pElement);
}

bool ETHScene::ReadFromXMLFile(TiXmlElement *pRoot)
{
	m_sceneProps.ReadFromXMLFile(pRoot);
	TiXmlNode *pNode = pRoot->FirstChild(GS_L("EntitiesInScene"));
	if (pNode)
	{
		TiXmlElement *pEntities = pNode->ToElement();
		if (pEntities)
		{
			pNode = pEntities->FirstChild(GS_L("Entity"));
			if (pNode)
			{
				TiXmlElement *pEntityIter = pNode->ToElement();
				if (pEntityIter)
				{
					do
					{
						ETHRenderEntity* entity = new ETHRenderEntity(pEntityIter, m_provider);
						AddEntity(entity);
						pEntityIter = pEntityIter->NextSiblingElement();
					} while (pEntityIter);
				}
			}
		}
	}
	m_provider->GetShaderManager()->SetParallaxIntensity(m_sceneProps.parallaxIntensity);
	return true;
}

int ETHScene::AddEntity(ETHRenderEntity* pEntity, const str_type::string& alternativeName)
{
	// sets an alternative name if there is any
	if (!alternativeName.empty())
	{
		pEntity->ChangeEntityName(alternativeName);
	}

	m_idCounter = Max(pEntity->GetID() + 1, m_idCounter + 1);

	// generate an unique id
	if (pEntity->GetID() < 0)
	{
		pEntity->SetID(m_idCounter);
	}

	m_buckets.Add(pEntity, (pEntity->GetType() == ETHEntityProperties::ET_HORIZONTAL) ? ETHBucketManager::FRONT : ETHBucketManager::BACK);

	m_maxSceneHeight = Max(m_maxSceneHeight, pEntity->GetMaxHeight());
	m_minSceneHeight = Min(m_minSceneHeight, pEntity->GetMinHeight());

	// find a callback function for this one
	if (m_pContext && m_pModule)
	{
		AssignCallbackScript(pEntity);
	}

	// if it has a callback and is dynamic, or if it's temporary, add it to the "callback constant run list"
	m_activeEntityHandler.AddEntityWhenEligible(pEntity);

	// restart all sound effects for this one
	// It's useful in case of explosion sfx's for example. It'll start all over again
	pEntity->StartSFX();

	return pEntity->GetID();
}

int ETHScene::AddEntity(ETHRenderEntity* pEntity)
{
	return AddEntity(pEntity, GS_L(""));	
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
	m_provider->GetShaderManager()->SetParallaxIntensity(m_sceneProps.parallaxIntensity);
}

void ETHScene::AddLight(const ETHLight &light)
{
	if (light.color == Vector3(0,0,0))
		return;

	// if the light isn't in screen, don't add it
	if (!light.staticLight)
		if (!ETHGlobal::IsSphereInScreen(light.pos, light.range, GetZAxisDirection(), m_provider->GetVideo()))
			return;

	m_lights.push_back(light);
}

void ETHScene::AddLight(const ETHLight &light, const Vector3& v3Pos, const Vector2& scale)
{
	ETHLight childLight = light;
	childLight.pos *= scale.y;
	childLight.range *= scale.y;
	childLight.pos += v3Pos;
	AddLight(childLight);
}

bool ETHScene::GenerateLightmaps(const int id)
{
	if (!m_richLighting)
	{
		return false;
	}

	// save current global scale and temporarily set it to 1
	const ETHGlobalScaleManagerPtr& scaleManager = m_provider->GetGlobalScaleManager();
	const float globalScale = scaleManager->GetScale();
	scaleManager->SetScaleFactor(1.0f);

	const ETHSpriteEntity *pRender = (id >= 0) ? m_buckets.SeekEntity(id) : 0;
	const Vector2 v2Bucket = (pRender) ? ETHBucketManager::GetBucket(pRender->GetPositionXY(), GetBucketSize()) : Vector2(0,0);

	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		// if we're lighting only one entity and it is not in this bucket, skip it.
		// I know we could have used the find method to go directly to that bucket
		// but this function os not that critical to make the effort worth it.
		if (id >= 0) 
			if (v2Bucket != bucketIter->first)
				continue;

		// iterate over all entities in this bucket
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			ETHRenderEntity* entity = (*iter);
			// if nID is valid, let's try to generate the lightmap for this one and only entity
			if (id >= 0)
				if (id != entity->GetID())
					continue;

			Vector2 v2Size(1,1);
			Vector2 v2AbsoluteOrigin(0,0);
			if (entity->GetSprite())
			{
				v2Size = entity->GetCurrentSize();
				v2AbsoluteOrigin = entity->ComputeAbsoluteOrigin(v2Size);
			}

			// Place the current entity at the top-left corner to align
			// it to the render target
			const Vector3 oldPos = entity->GetPosition();
			const Vector3 newPos = Vector3(v2AbsoluteOrigin.x, v2AbsoluteOrigin.y, 0);

			// fill the light list
			for (ETHBucketMap::iterator lbucketIter = m_buckets.GetFirstBucket(); lbucketIter != m_buckets.GetLastBucket(); ++lbucketIter)
			{
				ETHEntityList& lEntityList = lbucketIter->second;
				ETHEntityList::const_iterator liEnd = lEntityList.end();
				for (ETHEntityList::iterator liter = lEntityList.begin(); liter != liEnd; ++liter)
				{
					ETHRenderEntity* lightEntity = (*liter);
					if (lightEntity->IsStatic() && lightEntity->HasLightSource())
					{
						AddLight(*(lightEntity->GetLight()), newPos - oldPos + lightEntity->GetPosition(), lightEntity->GetScale());
					}
				}
			}

			if (m_lights.size() > 0)
			{
				ETHLightmapGen((*iter), m_provider->GetShaderManager(), m_lights.begin(), m_lights.end(),
					m_buckets, oldPos, newPos, m_minSceneHeight, m_maxSceneHeight, m_sceneProps);
			}
			else
			{
				entity->ReleaseLightmap();
			}

			entity->SetOrphanPosition(oldPos);
			m_lights.clear();
		}
	}
	#if defined(_DEBUG) || defined(DEBUG)
	ETH_STREAM_DECL(ss) << GS_L("Lightmaps created... ");
	m_provider->Log(ss.str(), Platform::FileLogger::INFO);
	#endif

	// go back to the previous global scale
	scaleManager->SetScaleFactor(globalScale);
	return true;
}

void ETHScene::Update(
	const unsigned long lastFrameElapsedTime,
	const ETHBackBufferTargetManagerPtr& backBuffer,
	SpritePtr outline,
	SpritePtr invisibleEntSymbol)
{
	m_destructorManager->RunDestructors();
	m_physicsSimulator.Update(lastFrameElapsedTime);
	RunCallbacksFromList();

	float minHeight, maxHeight;

	assert(m_piecesToRender.empty());

	// fill a map containing all entities we should render
	MapEntitiesToBeRendered(
		minHeight,
		maxHeight,
		backBuffer,
		outline,
		invisibleEntSymbol,
		lastFrameElapsedTime);

	m_minSceneHeight = minHeight;
	m_maxSceneHeight = maxHeight;

	Randomizer::Seed(m_provider->GetVideo()->GetElapsedTime());
}

void ETHScene::UpdateActiveEntities(const unsigned long lastFrameElapsedTime)
{
	m_activeEntityHandler.UpdateActiveEntities(GetZAxisDirection(), m_buckets, lastFrameElapsedTime);
}

void ETHScene::RenderScene(
	const bool roundUp,
	const ETHBackBufferTargetManagerPtr& backBuffer)
{
	const VideoPtr& video = m_provider->GetVideo();

	video->SetBlendMode(1, Video::BM_ADD);

	video->SetZWrite(GetZBuffer());
	video->SetZBuffer(GetZBuffer());

	// temporary list that will tell what to render
	std::list<ETHRenderEntity*> halos;

	// draw ambient pass
	video->RoundUpPosition(roundUp);

	DrawEntityMultimap(
		backBuffer,
		roundUp,
		halos);

	// call this guy after both update and rendering are finished
	ReleaseMappedEntities(m_piecesToRender);

	m_buckets.ResolveMoveRequests();
	video->RoundUpPosition(false);

	m_lights.clear();
}

void ETHScene::DecomposeEntityIntoPiecesToMultimap(
	ETHRenderEntity* entity,
	const float minHeight,
	const float maxHeight,
	const ETHBackBufferTargetManagerPtr& backBuffer,
	SpritePtr pOutline,
	SpritePtr pInvisibleEntSymbol)
{
	const VideoPtr& video = m_provider->GetVideo();
	const ETHShaderManagerPtr& shaderManager = m_provider->GetShaderManager();
	const ETHEntityProperties::ENTITY_TYPE type = entity->GetType();

	const bool spriteVisible = entity->IsSpriteVisible(m_sceneProps, backBuffer);
	
	// decompose entity sprite
	if (spriteVisible)
	{
		ETHEntityPieceRendererPtr spritePiece(
			new ETHEntitySpriteRenderer(
				entity,
				shaderManager,
				video,
				pOutline,
				pInvisibleEntSymbol,
				(m_enableLightmaps && m_showingLightmaps),
				AreRealTimeShadowsEnabled(),
				m_isInEditor,
				&m_lights));

		// add this entity to the multimap to sort it for an alpha-friendly rendering list
		const float depth = entity->ComputeDepth(maxHeight, minHeight);
		const float drawHash = ComputeDrawHash(depth, type);

		// add the entity to the render map
		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, spritePiece));
	}

	// decompose halo
	if (entity->HasLightSource() && entity->GetHalo())
	{
		const float depth = ETHEntity::ComputeDepth(entity->GetPositionZ() + entity->GetCurrentSize().y, maxHeight, minHeight);
		const float drawHash = ComputeDrawHash(depth, type);

		ETHEntityPieceRendererPtr haloPiece(new ETHEntityHaloRenderer(entity, shaderManager, GetHaloRotation(), depth));
		m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, haloPiece));
	}

	// decompose the particle list for this entity
	if (entity->HasParticleSystems())
	{
		for (std::size_t t = 0; t < entity->GetNumParticleSystems(); t++)
		{
			ETHEntityPieceRendererPtr particlePiece(
				new ETHEntityParticleRenderer(entity, shaderManager, t));

			ETHParticleManagerPtr particle = entity->GetParticleManager(t);

			const float depth = ETHEntity::ComputeDepth(
				particle->GetZPosition() + entity->GetPositionZ() + _ETH_PARTICLE_DEPTH_SHIFT,
				maxHeight,
				minHeight);

			const float drawHash = ComputeDrawHash(depth, type);

			m_piecesToRender.insert(std::pair<float, ETHEntityPieceRendererPtr>(drawHash, particlePiece));
		}
	}
}

void ETHScene::MapEntitiesToBeRendered(
	float &minHeight,
	float &maxHeight,
	const ETHBackBufferTargetManagerPtr& backBuffer,
	SpritePtr outline,
	SpritePtr invisibleEntSymbol,
	const unsigned int lastFrameElapsedTime)
{
	// store the max and min height to assign when everything is drawn
	maxHeight = m_maxSceneHeight;
	minHeight = m_minSceneHeight;

	m_nRenderedEntities = 0;

	const VideoPtr& video = m_provider->GetVideo();

	// don't let bucket size equal to 0
	assert(GetBucketSize().x != 0 || GetBucketSize().y != 0);

	// Gets the list of visible buckets
	std::list<Vector2> bucketList;
	const Vector2& camPos = video->GetCameraPos(); //for debugging purposes
	m_buckets.GetIntersectingBuckets(bucketList, camPos, backBuffer->GetBufferSize(), IsDrawingBorderBuckets(), IsDrawingBorderBuckets());

	assert(m_activeEntityHandler.IsCallbackListEmpty());

	// Loop through all visible Buckets
	for (std::list<Vector2>::iterator bucketPositionIter = bucketList.begin(); bucketPositionIter != bucketList.end(); ++bucketPositionIter)
	{
		ETHBucketMap::iterator bucketIter = m_buckets.Find(*bucketPositionIter);

		if (bucketIter == m_buckets.GetLastBucket())
			continue;

		ETHEntityList& entityList = bucketIter->second;
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

			// fill the light list for this frame
			if (entity->HasLightSource() && m_richLighting)
			{
				ETHLight light = *(entity->GetLight());
				// if it has a particle system in the first slot, adjust the light
				// brightness according to the number os active particles
				if (entity->GetParticleManager(0) && !entity->IsStatic())
				{
					#warning create method for this
					boost::shared_ptr<ETHParticleManager> paticleManager = entity->GetParticleManager(0);
					light.color *= 
						static_cast<float>(paticleManager->GetNumActiveParticles()) /
						static_cast<float>(paticleManager->GetNumParticles());
				}
				AddLight(light, entity->GetPosition(), entity->GetScale());
			}

			// If it is not going to be executed during the temp/dynamic entity management, update it now
			if (m_activeEntityHandler.IsCallbackEligible(entity))
			{
				entity->Update(lastFrameElapsedTime, GetZAxisDirection(), m_buckets);
			}

			// fill the callback list
			m_activeEntityHandler.AddCallbackWhenEligible(entity);

			DecomposeEntityIntoPiecesToMultimap(entity, minHeight, maxHeight, backBuffer, outline, invisibleEntSymbol);

			m_nRenderedEntities++;
		}
	}

	// Add persistent entities (the ones the user wants to force to render)
	FillMultimapAndClearPersistenList(bucketList, backBuffer);

	m_nCurrentLights = m_lights.size();
}

void ETHScene::DrawEntityMultimap(
	const ETHBackBufferTargetManagerPtr& backBuffer,
	const bool roundUp,
	std::list<ETHRenderEntity*> &outHalos)
{
	const VideoPtr& video = m_provider->GetVideo();

	video->RoundUpPosition(roundUp);

	// Draw visible entities ordered in an alpha-friendly map
	for (std::multimap<float, ETHEntityPieceRendererPtr>::iterator iter = m_piecesToRender.begin(); iter != m_piecesToRender.end(); ++iter)
	{
		iter->second->Render(m_sceneProps, m_maxSceneHeight, m_minSceneHeight);
	}

	// Show buckets outline in debug mode
	bool debug = false;
	#if defined(_DEBUG) || defined(DEBUG)
	debug = true;
	#endif

	if (m_isInEditor || debug)
	{
		if (m_provider->GetInput()->IsKeyDown(GSK_PAUSE) || m_provider->GetInput()->IsKeyDown(GSK_F12))
		{
			DrawBucketOutlines();
		}
	}
}

void ETHScene::ReleaseMappedEntities(std::multimap<float, ETHEntityPieceRendererPtr>& mm)
{
	mm.clear();
}

void ETHScene::EnableLightmaps(const bool enable)
{
	m_enableLightmaps = enable;
}

bool ETHScene::AreLightmapsEnabled() const
{
	return m_enableLightmaps;
}

void ETHScene::EnableRealTimeShadows(const bool enable)
{
	m_usingRTShadows = enable;
}

bool ETHScene::AreRealTimeShadowsEnabled() const
{
	return m_usingRTShadows;
}

// number of lights last-time-drawn
int ETHScene::GetNumLights()
{
	return m_nCurrentLights;
}

// number of actual lights (slower)
int ETHScene::CountLights()
{
	m_nCurrentLights = 0;
	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		ETHEntityList& entityList = bucketIter->second;
		ETHEntityList::const_iterator iEnd = entityList.end();
		for (ETHEntityList::iterator iter = entityList.begin(); iter != iEnd; ++iter)
		{
			if ((*iter)->HasLightSource())
				m_nCurrentLights++;
		}
	}
	return m_nCurrentLights;
}

void ETHScene::ShowLightmaps(const bool show)
{
	m_showingLightmaps = show;
}

bool ETHScene::AreLightmapsShown()
{
	return m_showingLightmaps;
}

bool ETHScene::DrawBucketOutlines()
{
	const VideoPtr& video = m_provider->GetVideo();

	// Gets the list of visible buckets
	std::list<Vector2> bucketList;
	ETHBucketManager::GetIntersectingBuckets(bucketList, m_provider->GetVideo()->GetCameraPos(), video->GetScreenSizeF(),
									  GetBucketSize(), IsDrawingBorderBuckets(), IsDrawingBorderBuckets());

	int nVisibleBuckets = 0;

	// Loop through all visible Buckets
	for (std::list<Vector2>::iterator bucketPositionIter = bucketList.begin(); bucketPositionIter != bucketList.end(); ++bucketPositionIter)
	{
		nVisibleBuckets++;

		const float width = video->GetLineWidth();
		video->SetLineWidth(2.0f);
		const Vector2 v2BucketPos = *bucketPositionIter * GetBucketSize() - video->GetCameraPos();
		video->DrawLine(v2BucketPos, v2BucketPos + Vector2(GetBucketSize().x, 0.0f), gs2d::constant::WHITE, gs2d::constant::WHITE);
		video->DrawLine(v2BucketPos, v2BucketPos + Vector2(0.0f, GetBucketSize().y), gs2d::constant::WHITE, gs2d::constant::WHITE);
		video->DrawLine(v2BucketPos + GetBucketSize(), v2BucketPos + Vector2(0.0f, GetBucketSize().y), gs2d::constant::WHITE, gs2d::constant::WHITE);
		video->DrawLine(v2BucketPos + GetBucketSize(), v2BucketPos + Vector2(GetBucketSize().x, 0.0f), gs2d::constant::WHITE, gs2d::constant::WHITE);
		video->SetLineWidth(width);
		
		// draw bucket key
		str_type::stringstream ss;

		if (m_buckets.Find(*bucketPositionIter) != m_buckets.GetLastBucket())
		{
			ss << GS_L("(") << bucketPositionIter->x << GS_L(",") << bucketPositionIter->y << GS_L(")")
				<< GS_L(" - entities: ") << m_buckets.GetNumEntities(*bucketPositionIter);
		}
		else
		{
			ss << GS_L("(") << bucketPositionIter->x << GS_L(",") << bucketPositionIter->y << GS_L(")");
		}

		const Vector2 v2TextPos(*bucketPositionIter * GetBucketSize() - video->GetCameraPos());
		video->DrawBitmapText(v2TextPos, ss.str(), ETH_DEFAULT_BITMAP_FONT, gs2d::constant::WHITE);
	}

	str_type::stringstream ss;
	ss << GS_L("Visible buckets: ") << nVisibleBuckets;

	video->DrawBitmapText(video->GetScreenSizeF()/2, ss.str(), ETH_DEFAULT_BITMAP_FONT, gs2d::constant::WHITE);
	return true;
}

float ETHScene::ComputeDrawHash(const float entityDepth, const ETHEntityProperties::ENTITY_TYPE& type) const
{
	float drawHash;
	switch (type)
	{
	case ETHEntityProperties::ET_HORIZONTAL:
		drawHash = entityDepth / 2.0f;
		break;
	case ETHEntityProperties::ET_VERTICAL:
		drawHash = (entityDepth / 2.0f) + 0.01f;
		break;
	case ETHEntityProperties::ET_GROUND_DECAL:
	case ETHEntityProperties::ET_OPAQUE_DECAL:
		drawHash = entityDepth / 2.0f + 0.01f;
		break;
	default:
		drawHash = entityDepth;
	}
	return drawHash;
}

int ETHScene::GetNumRenderedEntities()
{
	return m_nRenderedEntities;
}

void ETHScene::RunCallbacksFromList()
{
	m_activeEntityHandler.RunCallbacksFromLists();
}

void ETHScene::ForceAllSFXStop()
{
	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		ETHEntityList::const_iterator iEnd = bucketIter->second.end();
		for (ETHEntityList::iterator iter = bucketIter->second.begin(); iter != iEnd; ++iter)
		{
			(*iter)->ForceSFXStop();
		}
	}
}

bool ETHScene::AssignCallbackScript(ETHSpriteEntity* entity)
{
	const int callbackId = ETHGlobal::FindCallbackFunction(m_pModule, entity, ETH_CALLBACK_PREFIX, *m_provider->GetLogger());
	const int constructorCallbackId = ETHGlobal::FindCallbackFunction(m_pModule, entity, ETH_CONSTRUCTOR_CALLBACK_PREFIX, *m_provider->GetLogger());
	const int destructorCallbackId = ETHGlobal::FindCallbackFunction(m_pModule, entity, ETH_DESTRUCTOR_CALLBACK_PREFIX, *m_provider->GetLogger());
	AssignControllerToEntity(entity, callbackId, constructorCallbackId, destructorCallbackId);
	return true;
} 

void ETHScene::AssignControllerToEntity(ETHEntity* entity, const int callbackId, const int constructorCallbackId, const int destructorCallbackId)
{
	entity->SetDestructorCallbackId(destructorCallbackId);
	if (callbackId >= 0 || constructorCallbackId >= 0)
	{
		ETHEntityControllerPtr currentController(entity->GetController());
		ETHEntityControllerPtr newController(new ETHRawEntityController(currentController, m_pContext, callbackId, constructorCallbackId));
		entity->SetController(newController);
	}
	if (entity->IsBody())
	{
		ETHEntityControllerPtr controller = m_physicsSimulator.CreatePhysicsController(entity, m_pModule, m_pContext);
		#if defined(_DEBUG) || defined(DEBUG)
			str_type::stringstream ss; Platform::Logger::TYPE logType;
			if (controller)
			{
				logType = Platform::Logger::INFO;
				ss << GS_L("Physics controller successfuly created: ");
			}
			else
			{
				logType = Platform::Logger::ERROR;
				ss << GS_L("Couldn't create physics controller: ");
			}
			ss << entity->GetEntityName() << GS_L(" (#") << entity->GetID() << GS_L(")");
			m_provider->Log(ss.str(), logType);
		#endif
		entity->SetController(controller);
	}
}

bool ETHScene::AddFloatData(const str_type::string &entity, const str_type::string &name, const float value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHFloatData(value)));
}

bool ETHScene::AddIntData(const str_type::string &entity, const str_type::string &name, const int value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHIntData(value)));
}

bool ETHScene::AddUIntData(const str_type::string &entity, const str_type::string &name, const unsigned int value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHUIntData(value)));
}

bool ETHScene::AddStringData(const str_type::string &entity, const str_type::string &name, const str_type::string &value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHStringData(value)));
}

bool ETHScene::AddVector2Data(const str_type::string &entity, const str_type::string &name, const Vector2 &value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHVector2Data(value)));
}

bool ETHScene::AddVector3Data(const str_type::string &entity, const str_type::string &name, const Vector3 &value)
{
	return AddCustomData(entity, name, ETHCustomDataPtr(new ETHVector3Data(value)));
}

bool ETHScene::AddCustomData(const str_type::string &entity, const str_type::string &name, const ETHCustomDataConstPtr &inData)
{
	unsigned int count = 0;
	for (ETHBucketMap::iterator bucketIter = m_buckets.GetFirstBucket(); bucketIter != m_buckets.GetLastBucket(); ++bucketIter)
	{
		ETHEntityList::const_iterator iEnd = bucketIter->second.end();
		for (ETHEntityList::iterator iter = bucketIter->second.begin(); iter != iEnd; ++iter)
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

void ETHScene::SetLightIntensity(const float intensity)
{
	m_sceneProps.lightIntensity = intensity;
}

float ETHScene::GetLightIntensity() const
{
	return m_sceneProps.lightIntensity;
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

void ETHScene::SetHaloRotation(const bool enable)
{
	m_rotatingHalos = enable;
}

bool ETHScene::GetHaloRotation() const
{
	return m_rotatingHalos;
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
		entities[t]->RecoverResources();
	}
}

void ETHScene::SetZBuffer(const bool enable)
{
	m_enableZBuffer = enable;
}

bool ETHScene::GetZBuffer() const
{
	return m_enableZBuffer;
}

void ETHScene::ClearLightmaps()
{
	ETHEntityArray entities;
	m_buckets.GetEntityArray(entities);
	for (unsigned int t = 0; t < entities.size(); t++)
	{
		entities[t]->ReleaseLightmap();
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
	return m_buckets.DeleteEntity(pEntity->GetID(), ETHBucketManager::GetBucket(pEntity->GetPositionXY(), GetBucketSize()));
}

void ETHScene::ScaleEntities(const float scale, const bool scalePosition)
{
	ETHEntityArray entities;
	m_buckets.GetEntityArray(entities);
	for (unsigned int t = 0; t < entities.size(); t++)
	{
		entities[t]->Scale(scale);
		if (scalePosition)
		{
			entities[t]->SetPosition(entities[t]->GetPosition() * scale, m_buckets);
		}
	}
}

void ETHScene::AddEntityToPersistentList(ETHRenderEntity* entity)
{
	entity->AddRef();
	m_persistentEntities.push_back(entity);
}

void ETHScene::FillMultimapAndClearPersistenList(
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
		const bool bucketNotProcessed = std::find(currentBucketList.begin(), currentBucketList.end(), currentBucket) == currentBucketList.end();
		if (bucketNotProcessed)
		{
			DecomposeEntityIntoPiecesToMultimap(entity, m_minSceneHeight, m_maxSceneHeight, backBuffer, SpritePtr(), SpritePtr());
			m_nRenderedEntities++;
		}
		entity->Release();
	}
	m_persistentEntities.clear();
}
