#include "ETHSpriteEntity.h"
#include "ETHEntityCache.h"

#include "../Shader/ETHShaderManager.h"

#include "../Physics/ETHPhysicsController.h"

#include "../Resource/ETHDirectories.h"

#include <Math/OrientedBoundingBox.h>

#include <iostream>

#define UNUSED_ARGUMENT(argument) ((void)(argument))

const float ETHSpriteEntity::m_layrableMinimumDepth(0.001f);

ETHSpriteEntity::ETHSpriteEntity(const std::string& filePath, ETHResourceProviderPtr provider, const int nId) :
	m_provider(provider),
	ETHEntity(filePath, nId, provider->GetFileManager())
{
	Zero();
	Create();
}

ETHSpriteEntity::ETHSpriteEntity(
	TiXmlElement *pElement,
	ETHResourceProviderPtr provider,
	ETHEntityCache& entityCache,
	const std::string& entityPath,
	const bool shouldGenerateNewID) :
	ETHEntity(pElement, entityCache, entityPath, provider->GetFileManager(), shouldGenerateNewID),
	m_provider(provider)
{
	Zero();
	Create();
}

ETHSpriteEntity::ETHSpriteEntity(ETHResourceProviderPtr provider, const ETHEntityProperties& properties, const float angle, const float scale) :
	ETHEntity(),
	m_provider(provider)
{
	m_properties = properties;
	m_properties.scale *= scale;
	Zero();
	SetAngle(angle); // sets angle before Create() to start particles correctly
	Create();
}

ETHSpriteEntity::ETHSpriteEntity(ETHResourceProviderPtr provider) :
	ETHEntity(),
	m_provider(provider)
{
	Zero();
}

void ETHSpriteEntity::Refresh(const ETHEntityProperties& properties)
{
	m_properties = properties;
	Create();
}

void ETHSpriteEntity::Zero()
{
}

void ETHSpriteEntity::Create()
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	VideoPtr video = m_provider->GetVideo();

	// don't go further if it doesn't have any context
	if (!video || !graphicResources)
		return;

	const std::string& resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();
	
	const Platform::FileManagerPtr& fileManager = m_provider->GetFileManager();

	SetSprite(m_properties.spriteFile);

	if (m_properties.light)
	{
		 const ETHGraphicResourceManager::SpriteResource* haloResource =
		 	graphicResources->GetPointer(fileManager, video, m_properties.light->haloBitmap, resourceDirectory, ETHDirectories::GetHaloDirectory(), true);

		if (haloResource)
		{
			 m_pHalo = haloResource->GetSprite();
		}
	}

	LoadParticleSystem();

	if (m_pSprite)
	{
		SetOrigin();
	}
}

void ETHSpriteEntity::RecoverResources(const Platform::FileManagerPtr& expansionFileManager)
{
	Create();

	// if it has had a pre-rendered lightmap, reload it
	if (!m_preRenderedLightmapFilePath.empty())
	{
		Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
		Platform::FileManagerPtr currentFileManager     = fileIOHub->GetFileManager();
		const std::string currentResourceDirectory = fileIOHub->GetResourceDirectory();

		// gather from expansion file if it has one
		if (expansionFileManager)
		{
			fileIOHub->SetFileManager(expansionFileManager, (""));
		}

		LoadLightmapFromFile(m_preRenderedLightmapFilePath);

		fileIOHub->SetFileManager(currentFileManager, currentResourceDirectory);
	}
}

bool ETHSpriteEntity::LoadLightmapFromFile(const std::string& filePath)
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	VideoPtr video = m_provider->GetVideo();

	// don't go further if it doesn't have any context
	if (!video || !graphicResources)
		return false;

	if (ETHGlobal::FileExists(filePath, m_provider->GetFileManager()))
	{
		const ETHGraphicResourceManager::SpriteResource* lightmapResource = graphicResources->GetPointer(
			m_provider->GetFileManager(),
			video,
			Platform::GetFileName(filePath),
			(""),
			Platform::GetFileDirectory(filePath.c_str()),
			true);

		if (lightmapResource)
		{
			m_pLightmap = lightmapResource->GetSprite();
		}

		// store bitmap name to restore when necessary
		m_preRenderedLightmapFilePath = filePath;
	}

	return static_cast<bool>(m_pLightmap);
}

bool ETHSpriteEntity::ShouldUseHighlightPixelShader() const
{
	return (
			m_v4Color.x > 1.0f
		 || m_v4Color.y > 1.0f
		 || m_v4Color.z > 1.0f
	);
}

bool ETHSpriteEntity::ShouldUseSolidColorPixelShader() const
{
	return (m_v4SolidColor.w != 0.0f);
}

bool ETHSpriteEntity::ShouldUsePass1AddPixelShader() const
{
	return static_cast<bool>(m_pLightmap);
}

bool ETHSpriteEntity::SetSprite(const std::string &fileName)
{
	m_pSprite = m_provider->GetGraphicResourceManager()->GetSprite(
		m_provider->GetFileManager(),
		m_provider->GetVideo(),
		fileName,
		m_provider->GetFileIOHub()->GetResourceDirectory(),
		ETHDirectories::GetEntityDirectory(),
		false);

	if (m_pSprite)
	{
		m_properties.spriteFile = fileName;

		m_packedFrames = m_provider->GetGraphicResourceManager()->GetPackedFrames(m_properties.spriteFile);
		if (!m_packedFrames)
		{
			m_packedFrames = SpriteRectsPtr(new SpriteRects());
			m_properties.spriteCut.x = Max(1, m_properties.originalSpriteCut.x);
			m_properties.spriteCut.y = Max(1, m_properties.originalSpriteCut.y);
			m_packedFrames->SetRects(m_properties.spriteCut.x, m_properties.spriteCut.y);
		}
		return true;
	}
	else
	{
		m_properties.spriteFile = ("");
		return false;
	}
}

bool ETHSpriteEntity::SetHalo(const std::string &fileName)
{
	if (m_properties.light)
	{
		m_pHalo = m_provider->GetGraphicResourceManager()->GetSprite(
			m_provider->GetFileManager(),
			m_provider->GetVideo(),
			fileName,
			m_provider->GetFileIOHub()->GetResourceDirectory(),
			ETHDirectories::GetHaloDirectory(),
			true);

		if (m_pHalo)
		{
			m_properties.light->haloBitmap = fileName;
			return true;
		}
		else
		{
			m_properties.light->haloBitmap = ("");
			return false;
		}
	}
	else
	{
		return false;
	}
}

std::string ETHSpriteEntity::GetSpriteName() const
{
	return m_properties.spriteFile;
}

std::string ETHSpriteEntity::GetHaloName() const
{
	if (m_properties.light)
		return m_properties.light->haloBitmap;
	else
		return ("");
}

void ETHSpriteEntity::LoadParticleSystem()
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	ETHAudioResourceManagerPtr audioResources = m_provider->GetAudioResourceManager();
	VideoPtr video = m_provider->GetVideo();
	AudioPtr audio = m_provider->GetAudio();
	const std::string& resourcePath = m_provider->GetFileIOHub()->GetResourceDirectory();

	m_particles.clear();
	m_particles.resize(m_properties.particleSystems.size());
	for (std::size_t t=0; t<m_properties.particleSystems.size(); t++)
	{
		const ETHParticleSystem *pSystem = m_properties.particleSystems[t].get();
		if (pSystem->nParticles > 0)
		{
			std::string path = resourcePath;
			// path += GS_L("/");
			path += ETHDirectories::GetParticlesDirectory();
			path += Platform::GetFileName(pSystem->GetActualBitmapFile());

			if (!graphicResources->AddFile(m_provider->GetFileManager(), video, path, resourcePath, false))
			{
				continue;
			}

			const float particleScale = (GetScale().x + GetScale().y) / 2.0f;
			m_particles[t] = ETHParticleManagerPtr(
				new ETHParticleManager(
					m_provider,
					*pSystem,
					GetPosition(),
					GetAngle(),
					particleScale));
		}
	}
}

bool ETHSpriteEntity::SetSpriteCut(const unsigned int col, const unsigned int row)
{
	if (col >= 1 && row >= 1)
	{
		m_properties.spriteCut.x = m_properties.originalSpriteCut.x = col;
		m_properties.spriteCut.y = m_properties.originalSpriteCut.y = row;
	}
	else
	{
		ETH_STREAM_DECL(ss) << ("Invalid cut value");
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return false;
	}

	m_packedFrames = SpriteRectsPtr(new SpriteRects());
	m_packedFrames->SetRects(col, row);

	m_spriteFrame = m_spriteFrame % m_packedFrames->GetNumRects();
	return true;
}

Vector2 ETHSpriteEntity::GetSpriteCut() const
{
	return Vector2(
		static_cast<float>(m_properties.spriteCut.x),
		static_cast<float>(m_properties.spriteCut.y));
}

Vector2 ETHSpriteEntity::ComputeParallaxOffset(const float sceneParallaxIntensity) const
{
	Sprite::SetParallaxIntensity(sceneParallaxIntensity * GetParallaxIntensity());
	return Sprite::ComputeParallaxOffset(m_provider->GetVideo()->GetCameraPos(), GetPosition());
}

bool ETHSpriteEntity::SetDepth(const float maxHeight, const float minHeight)
{
	// feature no longer needed after eth-supersimple
	return true;
}

void ETHSpriteEntity::SetOrigin()
{
	const Vector2 v2Origin = ComputeOrigin(GetSize());
	m_pSprite->SetOrigin(v2Origin);
	if (m_pLightmap)
	{
		m_pLightmap->SetOrigin(v2Origin);
	}
}

Rect2D ETHSpriteEntity::GetFrameRect() const
{
	if (m_packedFrames)
	{
		return m_packedFrames->GetRect(GetFrame());
	}
	else
	{
		return Rect2D();
	}
}

unsigned int ETHSpriteEntity::GetFrame() const
{
	return m_spriteFrame;
}

bool ETHSpriteEntity::SetFrame(const unsigned int frame)
{
	const unsigned int numFrames = GetNumFrames();
	if (frame >= numFrames)
	{
		m_spriteFrame = frame % numFrames;
		return false;
	}
	else
	{
		m_spriteFrame = (frame);
		return true;
	}
}

bool ETHSpriteEntity::SetFrame(const unsigned int column, const unsigned int row)
{
	const Vector2i *pv2Cut = &m_properties.spriteCut;
	const unsigned int cutX = static_cast<unsigned int>(pv2Cut->x);
	const unsigned int cutY = static_cast<unsigned int>(pv2Cut->y);
	if (column >= cutX || row >= cutY)
	{
		m_spriteFrame = (0);
		return false;
	}
	else
	{
		m_spriteFrame = ((row*cutX)+column);
		return true;
	}
}

Vector2 ETHSpriteEntity::ComputeAbsoluteOrigin(const Vector2 &v2Size) const
{
	const Rect2D rect(GetFrameRect());
	const Vector2 virtualSize = (rect.originalSize == Vector2(0.0f)) ? v2Size : (rect.originalSize * m_properties.scale);

	Vector2 virtualCenter((virtualSize / 2.0f) + ((m_properties.pivotAdjust) * m_properties.scale));

	Vector2 offset(rect.offset * m_properties.scale);
	virtualCenter -= offset;
	
	const Vector2 fillBitmapSize(m_pSprite ? m_pSprite->GetSize(Rect2D()) : v2Size);
	
	if (GetFlipX()) virtualCenter.x = ((rect.size.x * fillBitmapSize.x) * m_properties.scale.x) - virtualCenter.x;
	if (GetFlipY()) virtualCenter.y = ((rect.size.y * fillBitmapSize.y) * m_properties.scale.y) - virtualCenter.y;

	return (virtualCenter);
}

Vector2 ETHSpriteEntity::ComputeOrigin(const Vector2 &v2Size) const
{
	return (ComputeAbsoluteOrigin(v2Size) / v2Size);
}

ETHParticleManagerPtr ETHSpriteEntity::GetParticleManager(const std::size_t n)
{
	if (n >= m_particles.size())
		return ETHParticleManagerPtr();
	else
		return m_particles[n];
}

ETHParticleManagerConstPtr ETHSpriteEntity::GetConstParticleManager(const std::size_t n) const
{
	if (n >= m_particles.size())
		return ETHParticleManagerConstPtr();
	else
		return m_particles[n];
}

SpritePtr ETHSpriteEntity::GetSprite()
{
	return m_pSprite;
}

SpritePtr ETHSpriteEntity::GetLightmap()
{
	return m_pLightmap;
}

SpritePtr ETHSpriteEntity::GetHalo()
{
	return m_pHalo;
}

SpritePtr ETHSpriteEntity::GetParticleBMP(const unsigned int n)
{
	if (n >= m_particles.size())
		return SpritePtr();
	else
		return m_particles[n]->GetParticleBitmap();
}

void ETHSpriteEntity::DestroyParticleSystem(const unsigned int n)
{
	if (n < m_particles.size())
	{
		m_particles[n].reset();
	}
}

std::string ETHSpriteEntity::AssembleLightmapFileName(const std::string& directory, const std::string& extension) const
{
	std::stringstream ss;
	ss << directory << ("add") << GetID() << (".") << extension;
	return ss.str();
}

void ETHSpriteEntity::Update(const float lastFrameElapsedTime, const Vector2& zAxisDir, const float sceneParallaxIntensity, ETHBucketManager& buckets)
{
	if (!IsStatic())
	{
		m_controller->Update(lastFrameElapsedTime, buckets);
	}
	UpdateParticleSystems(zAxisDir, sceneParallaxIntensity, lastFrameElapsedTime);
}

float ETHSpriteEntity::GetMaxHeight()
{
	float maxHeight = GetPosition().z + GetSize().y;
	for(std::size_t t = 0; t < m_particles.size(); t++)
	{
		boost::shared_ptr<ETHParticleSystem> system = m_properties.particleSystems[t];
		if (system->nParticles > 0)
			maxHeight = Max(maxHeight, GetPosition().z + system->startPoint.z);
	}
	if (HasLightSource() && HasHalo())
		maxHeight = Max(maxHeight, m_properties.light->pos.z);
	return maxHeight;
}

float ETHSpriteEntity::GetMinHeight()
{
	float minHeight = GetPosition().z - GetSize().y;
	for(std::size_t t = 0; t < m_particles.size(); t++)
	{
		boost::shared_ptr<ETHParticleSystem> system = m_properties.particleSystems[t];
		if (system->nParticles > 0)
			minHeight = Min(minHeight, GetPosition().z + system->startPoint.z);
	}
	if (HasLightSource() && HasHalo())
		minHeight = Min(minHeight, m_properties.light->pos.z);
	return minHeight;
}

void ETHSpriteEntity::SetParticleBitmap(const unsigned int n, SpritePtr bitmap)
{
	if (n < m_particles.size())
		m_particles[n]->SetParticleBitmap(bitmap);
}

void ETHSpriteEntity::SetParticleBitmap(const unsigned int n, const std::string& bitmap)
{
	if (n < m_particles.size())
	{
		if (n < m_properties.particleSystems.size())
			m_properties.particleSystems[n]->bitmapFile = bitmap;
		m_particles[n]->SetParticleBitmap(bitmap);
	}
}

void ETHSpriteEntity::SetParticlePosition(const unsigned int n, const Vector3 &v3Pos)
{
	if (n < m_particles.size())
		m_particles[n]->SetStartPos(v3Pos);
}

void ETHSpriteEntity::ScaleParticleSystem(const unsigned int n, const float scale)
{
	if (n < m_particles.size())
	{
		// m_properties.particleSystems[n]->Scale(scale);
		m_particles[n]->ScaleParticleSystem(scale);
	}
}

bool ETHSpriteEntity::MirrorParticleSystemX(const unsigned int n, const bool mirrorGravity)
{
	if (n < m_particles.size())
	{
		m_particles[n]->MirrorX(mirrorGravity);
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHSpriteEntity::MirrorParticleSystemY(const unsigned int n, const bool mirrorGravity)
{
	if (n < m_particles.size())
	{
		m_particles[n]->MirrorY(mirrorGravity);
		return true;
	}
	else
	{
		return false;
	}
}

void ETHSpriteEntity::SetParticleColorA(const unsigned int n, const Vector3 &color, const float alpha)
{
	if (n < m_particles.size())
	{
		m_particles[n]->SetParticleColorA(color, alpha);
	}
}

void ETHSpriteEntity::SetParticleColorB(const unsigned int n, const Vector3 &color, const float alpha)
{
	if (n < m_particles.size())
	{
		m_particles[n]->SetParticleColorB(color, alpha);
	}
}

bool ETHSpriteEntity::AreParticlesOver() const
{
	unsigned int nFinished = 0, nExistent = 0;
	for (std::size_t t=0; t<m_particles.size(); t++)
	{
		if (m_particles[t])
		{
			if (m_particles[t]->Finished())
			{
				nFinished++;
			}
			nExistent++;
		}
	}
	if (nExistent && nFinished == nExistent)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ETHSpriteEntity::UpdateParticleSystems(const Vector2& zAxisDirection, const float sceneParallaxIntensity, const float lastFrameElapsedTime)
{
	for (std::size_t t=0; t<m_particles.size(); t++)
	{
		if (m_particles[t])
		{
			m_particles[t]->UpdateParticleSystem(
				GetPosition(),
				zAxisDirection,
				ComputeParallaxOffset(sceneParallaxIntensity),
				GetAngle(),
				lastFrameElapsedTime);
		}
	}
}

bool ETHSpriteEntity::HasParticleSystem(const unsigned int n) const
{
	if (n >= m_particles.size())
	{
		return false;
	}
	else
	{
		if (m_particles[n])
			return true;
		else
			return false;
	}
}

float ETHSpriteEntity::ComputeLightIntensity()
{
	// if it has a particle system in the first slot, adjust the light
	// brightness according to the number os active particles
	ETHParticleManagerPtr particleManager = GetParticleManager(0);
	if (particleManager && !IsStatic())
	{
		return static_cast<float>(particleManager->GetNumActiveParticles()) /
			   static_cast<float>(particleManager->GetNumParticles());
	}
	else
	{
		return 1.0f;
	}
}

float ETHSpriteEntity::ComputeDepth(const float maxHeight, const float minHeight) const
{
	float r = 0.f;
	switch (GetType())
	{
	case ETHEntityProperties::ET_HORIZONTAL:
		r = ETHEntity::ComputeDepth(GetPositionZ(), maxHeight, minHeight);
		break;
	case ETHEntityProperties::ET_LAYERABLE:
		r = Max(m_layrableMinimumDepth, m_properties.layerDepth);
		break;
	};
	return r;
}

void ETHSpriteEntity::SetScale(const Vector2& scale)
{
	ETHEntity::SetScale(scale);
}

ETHPhysicsController* ETHSpriteEntity::GetPhysicsController()
{
	ETHPhysicsEntityControllerPtr controller = boost::dynamic_pointer_cast<ETHPhysicsEntityController>(m_controller);
	if (controller)
	{
		return new ETHPhysicsController(controller);
	}
	else
	{
		return 0;
	}
}

unsigned int ETHSpriteEntity::GetNumFrames() const
{
	return (m_packedFrames) ? m_packedFrames->GetNumRects() : 1;
}

Vector2 ETHSpriteEntity::GetSize() const
{
	Vector2 r(32.0f, 32.0f);

	if (!m_pSprite)
	{
		// if it has a light source
		if (HasLightSource())
		{
			// if it has a halo, use it's halo size
			if (HasHalo())
			{
				const float haloSize = m_properties.light->haloSize;
				if (m_pHalo && haloSize > 16.0f)
				{
					r = Vector2(haloSize, haloSize);
				}
			}
		}
		else if (IsCollidable()) // if it has no light source BUT is collidable
		{
			r = ETHGlobal::ToVector2(m_properties.collision->size);
		}
		else if (HasParticleSystem(0))
		{
			r = GetConstParticleManager(0)->ComputeBoundingRectangle(GetAngle()).size;
		}
	}
	else
	{
		r = m_pSprite->GetSize(m_packedFrames->GetRect(GetFrame()));
	}

	return r * m_properties.scale;
}

Vector2 ETHSpriteEntity::ComputeInScreenPosition(const ETHSceneProperties& sceneProps) const
{
	const Vector2 offset(ComputeParallaxOffset(sceneProps.parallaxIntensity) - m_provider->GetVideo()->GetCameraPos());
	return (ComputePositionWithZAxisApplied(sceneProps) + offset);
}

Vector2 ETHSpriteEntity::ComputeInScreenSpriteCenter(const ETHSceneProperties& sceneProps) const
{
	const float angle = GetAngle();
	if (angle == 0.0f)
	{
		const ETHEntityProperties::VIEW_RECT& rect = GetScreenRect(sceneProps);
		return ((rect.max + rect.min) * 0.5f);
	}
	else
	{
		const Vector2& pos = ComputeInScreenPosition(sceneProps);
		const Vector2& pivotAdjust(m_properties.pivotAdjust * m_properties.scale);
		const float radianAngle =-Util::DegreeToRadian(GetAngle());
		const float angleSin = sinf(radianAngle);
		const float angleCos = cosf(radianAngle);
		const Vector2 center(angleCos * pivotAdjust.x - angleSin * pivotAdjust.y,
							 angleSin * pivotAdjust.x + angleCos * pivotAdjust.y);
		return (pos - center);
	}
}

Vector2 ETHSpriteEntity::ComputePositionWithZAxisApplied(const ETHSceneProperties& sceneProps) const 
{
	Vector3 r;
	if (!m_pSprite)
	{
		if (HasLightSource() && !HasParticleSystems())
		{
			r = GetLightPosition();
		}
		else if (HasParticleSystems())
		{
			r = GetPosition() + m_particles[0]->GetSystem()->startPoint;
		}
		else if (IsCollidable())
		{
			const Vector3& collisionPos = m_properties.collision->pos;
			r = GetPosition() + (Vector3(ETHGlobal::ToVector2(collisionPos) * GetScale(), collisionPos.z));
		}
		else
		{
			r = GetPosition();
		}
	}
	else
	{
		r = GetPosition();
	}
	return ETHGlobal::ToScreenPos(r, sceneProps.zAxisDirection);
}

ETHEntityProperties::VIEW_RECT ETHSpriteEntity::GetScreenRect(const ETHSceneProperties& sceneProps) const
{
	ETHEntityProperties::VIEW_RECT box;
	const Vector2& pos = ComputeInScreenPosition(sceneProps);
	const Vector2& size = GetSize();
	const Vector2& absoluteOrigin = ComputeAbsoluteOrigin(size);

	box.min = pos - absoluteOrigin;
	box.max = pos + (size - absoluteOrigin);
	return box;
}

Vector2 ETHSpriteEntity::GetScreenRectMin(const ETHSceneProperties& sceneProps) const
{
	return GetScreenRect(sceneProps).min;
}

Vector2 ETHSpriteEntity::GetScreenRectMax(const ETHSceneProperties& sceneProps) const
{
	return GetScreenRect(sceneProps).max;
}

bool ETHSpriteEntity::IsPointOnSprite(const ETHSceneProperties& sceneProps, const Vector2& absolutePointPos, const Vector2& size) const
{
	const float angle = GetAngle();
	if (angle == 0.0f)
	{
		const ETHEntityProperties::VIEW_RECT& rect = GetScreenRect(sceneProps);
		const Vector2& min = rect.min;
		const Vector2& max = rect.max;
		if (min.x > absolutePointPos.x || min.y > absolutePointPos.y)
		{
			return false;
		}
		else if (max.x < absolutePointPos.x || max.y < absolutePointPos.y)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		const float radianAngle = -Util::DegreeToRadian(angle);
		const OrientedBoundingBox pointObb(absolutePointPos, math::constant::ONE_VECTOR2, 0.0f);
		const OrientedBoundingBox entityObb(ComputeInScreenSpriteCenter(sceneProps), size, radianAngle);
		return entityObb.Overlaps(pointObb);
	}
}

bool ETHSpriteEntity::PlayParticleSystem(const unsigned int n, const Vector2& zAxisDirection)
{
	UNUSED_ARGUMENT(zAxisDirection);
	if (n >= m_particles.size())
	{
		ETH_STREAM_DECL(ss) << ("ETHRenderEntity::PlayParticleSystem: n > m_particles.size()");
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return false;
	}
	else
	{
		m_particles[n]->Kill(false);
		const Vector3 v3Pos = GetPosition();
		m_particles[n]->Play(v3Pos, GetAngle());
		return true;
	}
}

void ETHSpriteEntity::KillParticleSystem(const unsigned int n)
{
	if (n >= m_particles.size())
	{
		return;
	}
	m_particles[n]->Kill(true);
}

bool ETHSpriteEntity::ParticlesKilled(const unsigned int n) const
{
	if (n >= m_particles.size())
	{
		return false;
	}
	return m_particles[n]->Killed();
}

void ETHSpriteEntity::AddRef()
{
	++m_ref;
}

void ETHSpriteEntity::Release()
{
	if (--m_ref == 0)
	{
		delete this;
	}
}

void ETHSpriteEntity::ReleaseLightmap()
{
	m_pLightmap.reset();
}
