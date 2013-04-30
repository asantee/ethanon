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

#include "ETHSpriteEntity.h"
#include "../Shader/ETHShaderManager.h"
#include "../Physics/ETHPhysicsController.h"
#include "../Resource/ETHDirectories.h"
#include <iostream>

const float ETHSpriteEntity::m_layrableMinimumDepth(0.001f);

ETHSpriteEntity::ETHSpriteEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId) :
	m_provider(provider),
	ETHEntity(filePath, nId, provider->GetFileManager())
{
	Zero();
	Create();
}

ETHSpriteEntity::ETHSpriteEntity(TiXmlElement *pElement, ETHResourceProviderPtr provider) :
	ETHEntity(pElement),
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
	m_stopSFXWhenDestroyed = true;
}

void ETHSpriteEntity::Create()
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	VideoPtr video = m_provider->GetVideo();

	// don't go further if it doesn't have any context
	if (!video || !graphicResources)
		return;

	const str_type::string& resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();

	m_pSprite = graphicResources->GetPointer(video, m_properties.spriteFile, resourceDirectory, ETHDirectories::GetEntityDirectory(), false);
	m_pNormal = graphicResources->GetPointer(video, m_properties.normalFile, resourceDirectory, ETHDirectories::GetNormalMapDirectory(), false);
	m_pGloss  = graphicResources->GetPointer(video, m_properties.glossFile,  resourceDirectory, ETHDirectories::GetEntityDirectory(), false);

	if (m_properties.light)
		m_pHalo = graphicResources->GetPointer(video, m_properties.light->haloBitmap, resourceDirectory, ETHDirectories::GetHaloDirectory(), true);

	LoadParticleSystem();

	if (m_pSprite)
	{
		//TODO/TO-DO: Remove duplicated code
		m_properties.spriteCut.x = Max(1, m_properties.spriteCut.x);
		m_properties.spriteCut.y = Max(1, m_properties.spriteCut.y);
		m_pSprite->SetupSpriteRects(m_properties.spriteCut.x, m_properties.spriteCut.y);
		m_pSprite->SetRect(m_spriteFrame);
		SetOrigin();
	}
}

void ETHSpriteEntity::RecoverResources()
{
	Create();
}

bool ETHSpriteEntity::LoadLightmapFromFile(const str_type::string& filePath)
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	VideoPtr video = m_provider->GetVideo();

	// don't go further if it doesn't have any context
	if (!video || !graphicResources)
		return false;

	if (ETHGlobal::FileExists(filePath, m_provider->GetFileManager()))
	{
		m_pLightmap = graphicResources->GetPointer(
			video,
			Platform::GetFileName(filePath),
			GS_L(""),
			Platform::GetFileDirectory(filePath.c_str()),
			false,
			true);
	}

	return (m_pLightmap);
}

bool ETHSpriteEntity::SetSprite(const str_type::string &fileName)
{
	m_pSprite = m_provider->GetGraphicResourceManager()->GetPointer(
		m_provider->GetVideo(),
		fileName,
		m_provider->GetFileIOHub()->GetResourceDirectory(),
		ETHDirectories::GetEntityDirectory(),
		false);

	if (m_pSprite)
	{
		//TODO/TO-DO: Remove duplicated code
		m_properties.spriteFile = fileName;
		m_properties.spriteCut.x = Max(1, m_properties.spriteCut.x);
		m_properties.spriteCut.y = Max(1, m_properties.spriteCut.y);
		m_pSprite->SetupSpriteRects(m_properties.spriteCut.x, m_properties.spriteCut.y);
		m_pSprite->SetRect(m_spriteFrame);
		return true;
	}
	else
	{
		m_properties.spriteFile = GS_L("");
		return false;
	}
}

bool ETHSpriteEntity::SetNormal(const str_type::string &fileName)
{
	m_pNormal = m_provider->GetGraphicResourceManager()->GetPointer(
		m_provider->GetVideo(),
		fileName,
		m_provider->GetFileIOHub()->GetResourceDirectory(),
		ETHDirectories::GetNormalMapDirectory(),
		false);
	if (m_pNormal)
	{
		m_properties.normalFile = fileName;
		return true;
	}
	else
	{
		m_properties.normalFile = GS_L("");
		return false;
	}
}

bool ETHSpriteEntity::SetGloss(const str_type::string &fileName)
{
	m_pGloss = m_provider->GetGraphicResourceManager()->GetPointer(
		m_provider->GetVideo(),
		fileName,
		m_provider->GetFileIOHub()->GetResourceDirectory(),
		ETHDirectories::GetEntityDirectory(),
		false);
	if (m_pGloss)
	{
		m_properties.glossFile = fileName;
		return true;
	}
	else
	{
		m_properties.glossFile = GS_L("");
		return false;
	}
}

bool ETHSpriteEntity::SetHalo(const str_type::string &fileName)
{
	if (m_properties.light)
	{
		m_pHalo = m_provider->GetGraphicResourceManager()->GetPointer(
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
			m_properties.light->haloBitmap = GS_L("");
			return false;
		}
	}
	else
	{
		return false;
	}
}

str_type::string ETHSpriteEntity::GetSpriteName() const
{
	return m_properties.spriteFile;
}

str_type::string ETHSpriteEntity::GetNormalName() const
{
	return m_properties.normalFile;
}

str_type::string ETHSpriteEntity::GetGlossName() const
{
	return m_properties.glossFile;
}

str_type::string ETHSpriteEntity::GetHaloName() const
{
	if (m_properties.light)
		return m_properties.light->haloBitmap;
	else
		return GS_L("");
}

void ETHSpriteEntity::LoadParticleSystem()
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	ETHAudioResourceManagerPtr audioResources = m_provider->GetAudioResourceManager();
	VideoPtr video = m_provider->GetVideo();
	AudioPtr audio = m_provider->GetAudio();
	const str_type::string& resourcePath = m_provider->GetFileIOHub()->GetResourceDirectory();

	m_particles.clear();
	m_particles.resize(m_properties.particleSystems.size());
	for (std::size_t t=0; t<m_properties.particleSystems.size(); t++)
	{
		const ETHParticleSystem *pSystem = m_properties.particleSystems[t].get();
		if (pSystem->nParticles > 0)
		{
			str_type::string path = resourcePath;
			// path += GS_L("/");
			path += ETHDirectories::GetParticlesDirectory();
			path += Platform::GetFileName(pSystem->GetActualBitmapFile());

			if (!graphicResources->AddFile(video, path, resourcePath, (pSystem->alphaMode == Video::AM_ADD), false))
				continue;

			const float particleScale = (GetScale().x + GetScale().y) / 2.0f;
			m_particles[t] = ETHParticleManagerPtr(
				new ETHParticleManager(m_provider, *pSystem, GetPositionXY(), GetPosition(),
									   GetAngle(), m_properties.soundVolume, particleScale));
		}
	}
}

bool ETHSpriteEntity::SetSpriteCut(const unsigned int col, const unsigned int row)
{
	if (col >= 1 && row >= 1)
	{
		m_properties.spriteCut.x = col;
		m_properties.spriteCut.y = row;
	}
	else
	{
		ETH_STREAM_DECL(ss) << GS_L("Invalid cut value");
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return false;
	}

	if (m_pSprite)
	{
		m_pSprite->SetupSpriteRects(col, row);
	}

	m_spriteFrame = 0;
	return true;
}

Vector2 ETHSpriteEntity::GetSpriteCut() const
{
	return Vector2(
		static_cast<float>(m_properties.spriteCut.x),
		static_cast<float>(m_properties.spriteCut.y));
}

Vector2 ETHSpriteEntity::ComputeParallaxOffset() const
{
	return m_provider->GetShaderManager()->ComputeParallaxOffset(m_provider->GetVideo(), GetPosition(), GetParallaxIndividualIntensity());
}

bool ETHSpriteEntity::SetDepth(const float maxHeight, const float minHeight)
{
	const float depth = ComputeDepth(maxHeight, minHeight);
	// assert(depth >= 0.0f && depth <= 1.0f);
	m_provider->GetVideo()->SetSpriteDepth(depth);
	return true;
}

void ETHSpriteEntity::SetOrigin()
{
	const Vector2 v2Origin = ComputeOrigin(GetCurrentSize());
	m_pSprite->SetOrigin(v2Origin);
	if (m_pLightmap)
		m_pLightmap->SetOrigin(v2Origin);
}

void ETHSpriteEntity::ValidateSpriteCut(const SpritePtr& sprite) const
{
	const Vector2i cut(sprite->GetNumColumns(), sprite->GetNumRows());
	const Vector2i& entityCut = m_properties.spriteCut;
	if (cut != entityCut)
	{
		sprite->SetupSpriteRects(entityCut.x, entityCut.y);
	}
}

ETHParticleManagerPtr ETHSpriteEntity::GetParticleManager(const std::size_t n)
{
	if (n >= m_particles.size())
		return ETHParticleManagerPtr();
	else
		return m_particles[n];
}

SpritePtr ETHSpriteEntity::GetSprite()
{
	return m_pSprite;
}

SpritePtr ETHSpriteEntity::GetGloss()
{
	return m_pGloss;
}

SpritePtr ETHSpriteEntity::GetNormal()
{
	return m_pNormal;
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

AudioSamplePtr ETHSpriteEntity::GetParticleSFX(const unsigned int n)
{
	if (n >= m_particles.size())
		return AudioSamplePtr();
	else
		return m_particles[n]->GetSoundEffect();
}

void ETHSpriteEntity::DestroyParticleSystem(const unsigned int n)
{
	if (n < m_particles.size())
	{
		m_particles[n].reset();
	}
}

str_type::string ETHSpriteEntity::AssembleLightmapFileName(const str_type::string& directory) const
{
	str_type::stringstream ss;
	ss << directory << GS_L("add") << GetID() << GS_L(".bmp");
	return ss.str();
}

bool ETHSpriteEntity::SaveLightmapToFile(const str_type::string& directory)
{
	if (!m_pLightmap)
	{
		return false;
	}
	
	m_pLightmap->SaveBitmap(AssembleLightmapFileName(directory).c_str(), Texture::BF_BMP);
	return true;
}

void ETHSpriteEntity::Update(const float lastFrameElapsedTime, const Vector2& zAxisDir, ETHBucketManager& buckets)
{
	if (!IsStatic())
	{
		m_controller->Update(lastFrameElapsedTime, buckets);
	}
	UpdateParticleSystems(zAxisDir, lastFrameElapsedTime);
}

float ETHSpriteEntity::GetMaxHeight()
{
	float maxHeight = GetPosition().z+GetCurrentSize().y;
	for(std::size_t t=0; t<m_particles.size(); t++)
	{
		boost::shared_ptr<ETHParticleSystem> system = m_properties.particleSystems[t];
		if (system->nParticles > 0)
			maxHeight = Max(maxHeight, GetPosition().z+system->startPoint.z+(system->boundingSphere*2));
	}
	if (HasLightSource() && HasHalo())
		maxHeight = Max(maxHeight, m_properties.light->pos.z);
	return maxHeight;
}

float ETHSpriteEntity::GetMinHeight()
{
	float minHeight = GetPosition().z-GetCurrentSize().y;
	for(std::size_t t=0; t<m_particles.size(); t++)
	{
		boost::shared_ptr<ETHParticleSystem> system = m_properties.particleSystems[t];
		if (system->nParticles > 0)
			minHeight = Min(minHeight, GetPosition().z+system->startPoint.z-(system->boundingSphere*2));
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

void ETHSpriteEntity::SetParticleBitmap(const unsigned int n, const str_type::string& bitmap)
{
	if (n < m_particles.size())
	{
		if (n < m_properties.particleSystems.size())
			m_properties.particleSystems[n]->bitmapFile = bitmap;
		m_particles[n]->SetParticleBitmap(bitmap);
	}
}

void ETHSpriteEntity::SetParticleSFX(const unsigned int n, AudioSamplePtr pSample)
{
	if (n < m_particles.size())
		m_particles[n]->SetSoundEffect(pSample);
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

void ETHSpriteEntity::UpdateParticleSystems(const Vector2& zAxisDirection, const float lastFrameElapsedTime)
{
	for (std::size_t t=0; t<m_particles.size(); t++)
	{
		if (m_particles[t])
			m_particles[t]->UpdateParticleSystem(ETHGlobal::ToScreenPos(GetPosition(), zAxisDirection), GetPosition(), GetAngle(), lastFrameElapsedTime);
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
	case ETHEntityProperties::ET_VERTICAL:
	case ETHEntityProperties::ET_HORIZONTAL:
		r = ETHEntity::ComputeDepth(GetPositionZ(), maxHeight, minHeight);
		break;
	case ETHEntityProperties::ET_OPAQUE_DECAL:
	case ETHEntityProperties::ET_GROUND_DECAL:
		r = ETHEntity::ComputeDepth(GetPositionZ() + ETH_SMALL_NUMBER, maxHeight, minHeight);
		break;
	case ETHEntityProperties::ET_OVERALL:
		r = (1.0f);
		break;
	case ETHEntityProperties::ET_LAYERABLE:
		r = Max(m_layrableMinimumDepth, m_properties.layerDepth);
		break;
	};
	return r;
}

void ETHSpriteEntity::SetScale(const Vector2& scale)
{
	ETHEntity::SetScale(scale * m_provider->GetGlobalScaleManager()->GetScale());
}

ETHPhysicsController* ETHSpriteEntity::GetPhysicsController()
{
	ETHPhysicsEntityControllerPtr controller = boost::dynamic_pointer_cast<ETHPhysicsEntityController>(m_controller);
	if (controller)
	{
		return new ETHPhysicsController(controller, m_provider->GetGlobalScaleManager());
	}
	else
	{
		return 0;
	}
}

Vector2 ETHSpriteEntity::GetCurrentSize() const
{
	Vector2 v2R(32.0f, 32.0f);

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
					v2R = Vector2(haloSize, haloSize);
				}
			}
		}
		else if (IsCollidable()) // if it has no light source BUT is collidable
		{
			v2R = ETHGlobal::ToVector2(m_properties.collision->size);
		}
	}
	else
	{
		ValidateSpriteCut(m_pSprite);
		v2R = m_pSprite->GetFrameSize();
	}

	return v2R * m_properties.scale;
}

Vector2 ETHSpriteEntity::ComputeInScreenPosition(const ETHSceneProperties& sceneProps) const
{
	const Vector2 offset(ComputeParallaxOffset() - m_provider->GetVideo()->GetCameraPos());
	return (ComputePositionWithZAxisApplied(sceneProps) + offset);
}

Vector2 ETHSpriteEntity::ComputeInScreenSpriteCenter(const ETHSceneProperties& sceneProps) const
{
	const float angle = GetAngle();
	if (GetType() == ETHEntityProperties::ET_VERTICAL || angle == 0.0f)
	{
		const ETHEntityProperties::VIEW_RECT& rect = GetScreenRect(sceneProps);
		return ((rect.max + rect.min) * 0.5f);
	}
	else
	{
		const Vector2& pos = ComputeInScreenPosition(sceneProps);
		const Vector2& pivotAdjust(m_properties.pivotAdjust * m_properties.scale);
		const float radianAngle =-DegreeToRadian(GetAngle());
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
	const Vector2& size = GetCurrentSize();
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
	if (GetType() == ETHEntityProperties::ET_VERTICAL || angle == 0.0f)
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
		// TODO/TO-DO perform this in the OrientedBoundingBox class
		const float radianAngle = -DegreeToRadian(angle);
		const OrientedBoundingBox pointObb(absolutePointPos, math::constant::ONE_VECTOR2, 0.0f);
		const OrientedBoundingBox entityObb(ComputeInScreenSpriteCenter(sceneProps), size, radianAngle);
		return entityObb.Overlaps(pointObb);
	}
}

void ETHSpriteEntity::StartSFX()
{
	for(std::size_t t=0; t<m_particles.size(); t++)
	{
		AudioSamplePtr pSample = m_particles[t]->GetSoundEffect();
		if (pSample)
			pSample->Play();
	}
}

void ETHSpriteEntity::ForceSFXStop()
{
	for(std::size_t t=0; t<m_particles.size(); t++)
	{
		if (m_particles[t])
		{
			AudioSamplePtr pSample = m_particles[t]->GetSoundEffect();
			if (pSample)
			{
				//if (m_particles[t].IsSoundLooping())
					pSample->Stop();
			}
		}
	}
}

void ETHSpriteEntity::SetStopSFXWhenDestroyed(const bool enable)
{
	m_stopSFXWhenDestroyed = enable;
}

void ETHSpriteEntity::SetSoundVolume(const float volume)
{
	for(std::size_t t = 0; t < m_particles.size(); t++)
	{
		if (m_particles[t])
			if (m_particles[t]->GetSoundEffect())
				m_particles[t]->SetSoundVolume(volume);
	}
}

void ETHSpriteEntity::SilenceParticleSystems(const bool silence)
{
	for(std::size_t t = 0; t < m_particles.size(); t++)
	{
		AudioSamplePtr pSample = m_particles[t]->GetSoundEffect();
		if (pSample)
		{
			pSample->Stop();
		}
		m_particles[t]->StopSFX(silence);
	}
}

bool ETHSpriteEntity::PlayParticleSystem(const unsigned int n, const Vector2& zAxisDirection)
{
	if (n >= m_particles.size())
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHRenderEntity::PlayParticleSystem: n > m_particles.size()");
		m_provider->Log(ss.str(), Platform::Logger::ERROR);
		return false;
	}
	else
	{
		m_particles[n]->Kill(false);
		const Vector3 v3Pos = GetPosition();
		m_particles[n]->Play(ETHGlobal::ToScreenPos(v3Pos, zAxisDirection), v3Pos, GetAngle());
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
		if (m_stopSFXWhenDestroyed)
		{
			ForceSFXStop();
		}
		/*#if defined(_DEBUG) || defined(DEBUG)
		if (GetID() >= 0)
		{
			ETH_STREAM_DECL(ss) << GS_L("Entity destroyed: #") << GetID();
			m_provider->Log(ss.str(), Platform::Logger::INFO);
		}
		#endif*/
		delete this;
	}
}

void ETHSpriteEntity::ReleaseLightmap()
{
	m_pLightmap.reset();
}
