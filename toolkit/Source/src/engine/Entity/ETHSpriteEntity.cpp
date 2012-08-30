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

#include "ETHSpriteEntity.h"
#include "../Shader/ETHShaderManager.h"
#include "../Physics/ETHPhysicsController.h"
#include <iostream>

const float ETHSpriteEntity::m_layrableMinimumDepth(0.001f);
const float ETHSpriteEntity::m_haloRotation(90.0f);

ETHSpriteEntity::ETHSpriteEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId) :
	m_provider(provider),
	ETHEntity(filePath, nId, provider->GetVideo()->GetFileManager())
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

	const str_type::string& programPath = m_provider->GetResourcePath();

	m_pSprite = graphicResources->GetPointer(video, m_properties.spriteFile, programPath, ETHDirectories::GetEntityPath(), false);
	m_pNormal = graphicResources->GetPointer(video, m_properties.normalFile, programPath, ETHDirectories::GetNormalMapPath(), false);
	m_pGloss  = graphicResources->GetPointer(video, m_properties.glossFile,  programPath, ETHDirectories::GetEntityPath(), false);

	if (m_properties.light)
		m_pHalo = graphicResources->GetPointer(video, m_properties.light->haloBitmap, programPath, ETHDirectories::GetHaloPath(), true);

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

bool ETHSpriteEntity::SetSprite(const str_type::string &fileName)
{
	m_pSprite = m_provider->GetGraphicResourceManager()->GetPointer(
		m_provider->GetVideo(),
		fileName,
		m_provider->GetResourcePath(),
		ETHDirectories::GetEntityPath(),
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
		m_provider->GetResourcePath(),
		ETHDirectories::GetNormalMapPath(),
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
		m_provider->GetResourcePath(),
		ETHDirectories::GetEntityPath(),
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
			m_provider->GetResourcePath(),
			ETHDirectories::GetHaloPath(),
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

void ETHSpriteEntity::LoadParticleSystem()
{
	ETHGraphicResourceManagerPtr graphicResources = m_provider->GetGraphicResourceManager();
	ETHAudioResourceManagerPtr audioResources = m_provider->GetAudioResourceManager();
	VideoPtr video = m_provider->GetVideo();
	AudioPtr audio = m_provider->GetAudio();
	const str_type::string& resourcePath = m_provider->GetResourcePath();

	m_particles.clear();
	m_particles.resize(m_properties.particleSystems.size());
	for (std::size_t t=0; t<m_properties.particleSystems.size(); t++)
	{
		const ETH_PARTICLE_SYSTEM *pSystem = m_properties.particleSystems[t].get();
		if (pSystem->nParticles > 0)
		{
			str_type::string path = resourcePath;
			// path += GS_L("/");
			path += ETHDirectories::GetParticlesPath();
			path += ETHGlobal::GetFileName(pSystem->GetActualBitmapFile());

			if (!graphicResources->AddFile(video, path, (pSystem->alphaMode == GSAM_ADD)))
				continue;

			m_particles[t] = ETHParticleManagerPtr(
				new ETHParticleManager(m_provider, *pSystem, GetPositionXY(), GetPosition(), GetAngle(), m_properties.soundVolume));
			m_particles[t]->ScaleParticleSystem((GetScale().x + GetScale().y) / 2.0f);
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

float ETHSpriteEntity::ComputeHaloAngle() const
{
	VideoPtr video = m_provider->GetVideo();
	const Vector2 v2HalfScreen = video->GetScreenSizeF() / 2;
	const Vector2 v2ScreenPos = ETHGlobal::ToVector2(GetLightPosition()) - video->GetCameraPos() - v2HalfScreen;
	const float horizontalBias = (Clamp(v2ScreenPos.x / v2HalfScreen.x,-1.0f, 1.0f));
	const float verticalBias   = (Clamp(v2ScreenPos.y / v2HalfScreen.y,-1.0f, 1.0f));
	const float angleX = Square(horizontalBias) * Sign(horizontalBias) * m_haloRotation;
	const float angleY = Square(verticalBias) * Sign(verticalBias) * m_haloRotation;
	return (angleX) + (angleY);
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
	const Vector2 v2Size = GetCurrentSize();
	const Vector2 v2Origin = ComputeOrigin(v2Size)/v2Size;
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

Vector2 ETHSpriteEntity::GetCurrentSize() const
{
	Vector2 v2R(-1,-1);

	if (!m_pSprite)
	{
		// if it has a light source
		if (HasLightSource())
		{
			// if it has a halo, use it's halo size
			if (HasHalo())
			{
				const float haloSize = m_properties.light->haloSize;
				if (m_pHalo && haloSize > 0.0f)
				{
					v2R = Vector2(haloSize, haloSize);
				}
				else
				{
					v2R = Vector2(32,32);
				}
			}
			else // otherwise use a default size
			{
				v2R = Vector2(32,32);
			}
		}
		else if (IsCollidable()) // if it has no light source BUT is collidable
		{
			v2R = ETHGlobal::ToVector2(m_properties.collision->size);
		}
		else
		{
			v2R = Vector2(32,32);
		}
	}
	else
	{
		ValidateSpriteCut(m_pSprite);
		v2R = m_pSprite->GetFrameSize();
	}

	return v2R * m_properties.scale;
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

void ETHSpriteEntity::Update(const unsigned long lastFrameElapsedTime, const Vector2& zAxisDir, ETHBucketManager& buckets)
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
		boost::shared_ptr<ETH_PARTICLE_SYSTEM> system = m_properties.particleSystems[t];
		if (system->nParticles > 0)
			maxHeight = Max(maxHeight, GetPosition().z+system->v3StartPoint.z+(system->boundingSphere*2));
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
		boost::shared_ptr<ETH_PARTICLE_SYSTEM> system = m_properties.particleSystems[t];
		if (system->nParticles > 0)
			minHeight = Min(minHeight, GetPosition().z+system->v3StartPoint.z-(system->boundingSphere*2));
	}
	if (HasLightSource() && HasHalo())
		minHeight = Min(minHeight, m_properties.light->pos.z);
	return minHeight;
}

void ETHSpriteEntity::SetParticleBMP(const unsigned int n, SpritePtr pBMP)
{
	if (n < m_particles.size())
		m_particles[n]->SetParticleBitmap(pBMP);
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

void ETHSpriteEntity::UpdateParticleSystems(const Vector2& zAxisDirection, const unsigned long lastFrameElapsedTime)
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

float ETHSpriteEntity::ComputeDepth(const float maxHeight, const float minHeight)
{
	float r = 0.f;
	switch (GetType())
	{
	case ETH_VERTICAL:
	case ETH_HORIZONTAL:
		r = ETHGlobal::ComputeDepth(GetPosition().z, maxHeight, minHeight);
		break;
	case ETH_OPAQUE_DECAL:
	case ETH_GROUND_DECAL:
		r = ETHGlobal::ComputeDepth(GetPosition().z+_ETH_SMALL_NUMBER, maxHeight, minHeight);
		break;
	case ETH_OVERALL:
		r = (1.0f);
		break;
	case ETH_LAYERABLE:
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

ETH_VIEW_RECT ETHSpriteEntity::GetScreenRect(const ETHSceneProperties& sceneProps) const
{
	ETH_VIEW_RECT box;
	const Vector2& v2Pos = GetScreenPosition(sceneProps);
	const Vector2& v2Size = GetCurrentSize();
	const Vector2& parallaxOffset = ComputeParallaxOffset();
	if (GetType() == ETH_VERTICAL && m_pSprite)
	{
		box.v2Min.x = v2Pos.x - (v2Size.x / 2.0f);
		box.v2Max.x = v2Pos.x + (v2Size.x / 2.0f);

		box.v2Min.y = v2Pos.y - (v2Size.y);
		box.v2Max.y = v2Pos.y;
	}
	else
	{
		box.v2Min.x = v2Pos.x - (v2Size.x / 2.0f);
		box.v2Max.x = v2Pos.x + (v2Size.x / 2.0f);

		box.v2Min.y = v2Pos.y - (v2Size.y / 2.0f);
		box.v2Max.y = v2Pos.y + (v2Size.y / 2.0f);
	}
	const Vector2& pivot = m_properties.pivotAdjust;
	box.v2Max -= pivot + m_provider->GetVideo()->GetCameraPos()-parallaxOffset;
	box.v2Min -= pivot + m_provider->GetVideo()->GetCameraPos()-parallaxOffset;
	return box;
}

Vector2 ETHSpriteEntity::GetScreenRectMin(const ETHSceneProperties& sceneProps) const
{
	const Vector2& v2Pos = GetScreenPosition(sceneProps);
	const Vector2& v2Size = GetCurrentSize();
	const Vector2& parallaxOffset = ComputeParallaxOffset();
	Vector2 v2Min;
	if (GetType() == ETH_VERTICAL && m_pSprite)
	{
		v2Min.x = v2Pos.x - (v2Size.x / 2.0f);
		v2Min.y = v2Pos.y - (v2Size.y);
	}
	else
	{
		v2Min.x = v2Pos.x - (v2Size.x / 2.0f);
		v2Min.y = v2Pos.y - (v2Size.y / 2.0f);
	}
	v2Min -= m_properties.pivotAdjust + m_provider->GetVideo()->GetCameraPos();
	if (!m_pSprite)
		v2Min = ETHGlobal::ToScreenPos(GetLightRelativePosition(), sceneProps.zAxisDirection);
	return v2Min+parallaxOffset;
}

Vector2 ETHSpriteEntity::GetScreenRectMax(const ETHSceneProperties& sceneProps) const
{
	const Vector2& v2Pos = GetScreenPosition(sceneProps);
	const Vector2& v2Size = GetCurrentSize();
	const Vector2& parallaxOffset = ComputeParallaxOffset();
	Vector2 v2Max;
	if (GetType() == ETH_VERTICAL && m_pSprite)
	{
		v2Max.x = v2Pos.x + (v2Size.x / 2.0f);
		v2Max.y = v2Pos.y;
	}
	else
	{
		v2Max.x = v2Pos.x + (v2Size.x / 2.0f);
		v2Max.y = v2Pos.y + (v2Size.y / 2.0f);
	}
	v2Max -= m_properties.pivotAdjust + m_provider->GetVideo()->GetCameraPos();
	if (!m_pSprite)
		v2Max = ETHGlobal::ToScreenPos(GetLightRelativePosition(), sceneProps.zAxisDirection);
	return v2Max+parallaxOffset;
}

Vector2 ETHSpriteEntity::GetScreenPosition(const ETHSceneProperties& sceneProps) const 
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
			r = GetPosition()+m_particles[0]->GetSystem()->v3StartPoint;
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
		/*#ifdef _DEBUG
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
