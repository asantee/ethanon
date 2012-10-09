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

#include "ETHParticleManager.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Resource/ETHDirectories.h"
#include "../Util/ETHASUtil.h"
#include "../Entity/ETHEntity.h"
#include <Math/Randomizer.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

ETHParticleManager::ETHParticleManager(
	ETHResourceProviderPtr provider,
	const str_type::string& file,
	const Vector2& v2Pos,
	const Vector3& v3Pos,
	const float angle,
	const float entityVolume) :
	m_provider(provider)
{
	ETHParticleSystem partSystem;
	if (partSystem.ReadFromFile(file, m_provider->GetFileManager()))
	{
		CreateParticleSystem(partSystem, v2Pos, v3Pos, angle, entityVolume, 1.0f);
	}
	else
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHParticleManager: file not found: ") << file;
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
	}
}

ETHParticleManager::ETHParticleManager(
	ETHResourceProviderPtr provider,
	const ETHParticleSystem& partSystem,
	const Vector2& v2Pos,
	const Vector3& v3Pos,
	const float angle,
	const float entityVolume,
	const float scale) :
	m_provider(provider)
{
	CreateParticleSystem(partSystem, v2Pos, v3Pos, angle, entityVolume, scale);
}

bool ETHParticleManager::CreateParticleSystem(
	const ETHParticleSystem& partSystem,
	const Vector2& v2Pos,
	const Vector3& v3Pos,
	const float angle,
	const float entityVolume,
	const float scale)
{
	GS2D_UNUSED_ARGUMENT(v3Pos);
	if (partSystem.nParticles <= 0)
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHParticleManager::CreateParticleSystem: The number of particles must be greater than 0.");
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}

	m_finished = false;
	m_killed = false;
	m_nActiveParticles = 0;
	m_soundVolume = 1.0f;
	m_isSoundLooping = false;
	m_isSoundStopped = false;
	m_generalVolume = 1.0f;
	m_system = partSystem;
	m_entityVolume = entityVolume;

	m_system.Scale(scale);

	if (m_system.bitmapFile.empty())
	{
		m_system.bitmapFile = ETH_DEFAULT_PARTICLE_BITMAP;
	}

	ETHGraphicResourceManagerPtr graphics = m_provider->GetGraphicResourceManager();
	ETHAudioResourceManagerPtr samples = m_provider->GetAudioResourceManager();
	Platform::FileIOHubPtr fileIOHub = m_provider->GetFileIOHub();
	Platform::FileManagerPtr fileManager = m_provider->GetFileManager();

	// if there's no resource path, search the current module's path
	const str_type::string& resourcePath = fileIOHub->GetResourceDirectory();
	const str_type::string& programPath  = fileIOHub->GetProgramDirectory();
	const str_type::string currentPath = (resourcePath.empty() && !fileManager->IsPacked()) ? programPath : resourcePath;

	m_pBMP = graphics->GetPointer(m_provider->GetVideo(), m_system.bitmapFile, currentPath,
		ETHDirectories::GetParticlesDirectory(), (m_system.alphaMode == Video::AM_ADD));

	// find the particle sound effect
	if (m_system.soundFXFile != GS_L(""))
	{
		m_pSound = samples->GetPointer(m_provider->GetAudio(), m_provider->GetFileIOHub(), m_system.soundFXFile,
			ETHDirectories::GetSoundFXDirectory(), GSST_SOUND_EFFECT);
	}

	if (m_system.allAtOnce)
	{
		m_nActiveParticles = m_system.nParticles;
	}
	else
	{
		m_nActiveParticles = 0;
	}

	m_particles.resize(m_system.nParticles);

	Matrix4x4 rot = RotateZ(DegreeToRadian(angle));
	for (int t = 0; t < m_system.nParticles; t++)
	{
		m_particles[t].id = t;
		m_particles[t].released = false;
		ResetParticle(t, v2Pos, Vector3(v2Pos,0), angle, rot);
	}
	return true;
}

void ETHParticleManager::SetSoundVolume(const float volume)
{
	m_entityVolume = Min(Max(volume, 0.0f), 1.0f);
}

float ETHParticleManager::GetSoundVolume() const
{
	return m_entityVolume;
}

void ETHParticleManager::StopSFX(const bool stopped)
{
	m_isSoundStopped = stopped;
}

Vector3 ETHParticleManager::GetStartPos() const
{
	return m_system.startPoint;
}

void ETHParticleManager::SetStartPos(const Vector3& v3Pos)
{
	m_system.startPoint = v3Pos;
}

float ETHParticleManager::GetBoundingRadius() const
{
	return m_system.boundingSphere / 2;
}

str_type::string ETHParticleManager::GetBitmapName() const
{
	return m_system.bitmapFile;
}

str_type::string ETHParticleManager::GetSoundName() const
{
	return m_system.soundFXFile;
}

void ETHParticleManager::Kill(const bool kill)
{
	m_killed = kill;
}

void ETHParticleManager::SetSystem(const ETHParticleSystem &partSystem)
{
	//partSystem.nParticles = m_system.nParticles;
	m_system = partSystem;
}

void ETHParticleManager::SetParticleBitmap(SpritePtr pBMP)
{
	m_pBMP = pBMP;
}

SpritePtr ETHParticleManager::GetParticleBitmap()
{
	return m_pBMP;
}

const ETHParticleSystem *ETHParticleManager::GetSystem() const
{
	return &m_system;
}

void ETHParticleManager::SetSoundEffect(AudioSamplePtr pSound)
{
	m_pSound = pSound;
}
  
AudioSamplePtr ETHParticleManager::GetSoundEffect()
{
	return m_pSound;
}

// Returns true if it has a SFX
bool ETHParticleManager::HasSoundEffect() const
{
	return (m_system.soundFXFile != GS_L(""));
}

bool ETHParticleManager::IsSoundLooping() const
{
	return m_isSoundLooping;
}

bool ETHParticleManager::UpdateParticleSystem(
	const Vector2& v2Pos,
	const Vector3& v3Pos,
	const float angle,
	const unsigned long lastFrameElapsedTime)
{
	bool anythingDrawn = false;
	const unsigned long cappedLastFrameElapsedTime = Min(lastFrameElapsedTime, static_cast<unsigned long>(250));
	const float frameSpeed = static_cast<float>((static_cast<double>(cappedLastFrameElapsedTime) / 1000.0) * 60.0);

	Matrix4x4 rot = RotateZ(DegreeToRadian(angle));
	m_nActiveParticles = 0;
	for (int t = 0; t < m_system.nParticles; t++)
	{
		PARTICLE& particle = m_particles[t];

		if (m_system.repeat > 0)
			if (particle.repeat >= m_system.repeat)
				continue;

		// check how many particles are active
		if (particle.size > 0.0f && particle.released)
		{
			if (!Killed() || (Killed() && particle.elapsed < particle.lifeTime))
				m_nActiveParticles++;
		}

		anythingDrawn = true;
		particle.elapsed += lastFrameElapsedTime;

		if (!particle.released)
		{
			// if we shouldn't release all particles at the same time, check if it's time to release this particle
			const float releaseTime = 
				((m_system.lifeTime + m_system.randomizeLifeTime) * (static_cast<float>(particle.id) / static_cast<float>(m_system.nParticles)));

			if (particle.elapsed > releaseTime || m_system.allAtOnce)
			{
				particle.elapsed = 0.0f;
				particle.released = true;
				PositionParticle(t, v2Pos, angle, rot, v3Pos);
			}
		}

		if (particle.released)
		{
			particle.dir += (m_system.gravityVector * frameSpeed);
			particle.pos += (particle.dir * frameSpeed);
			particle.angle += (particle.angleDir * frameSpeed);
			particle.size  += (m_system.growth * frameSpeed);
			const float w = particle.elapsed / particle.lifeTime;
			particle.color = m_system.color0 + (m_system.color1 - m_system.color0) * w;

			// update particle animation if there is any
			if (m_system.spriteCut.x > 1 || m_system.spriteCut.y > 1)
			{
				if (m_system.animationMode == ETHParticleSystem::PLAY_ANIMATION)
				{
					particle.currentFrame = static_cast<unsigned int>(
						Min(static_cast<int>(static_cast<float>(m_system.GetNumFrames()) * w),
							m_system.GetNumFrames() - 1));
				}
			}

			particle.size = Min(particle.size, m_system.maxSize);
			particle.size = Max(particle.size, m_system.minSize);

			if (particle.elapsed > particle.lifeTime)
			{
				particle.repeat++;
				if (!Killed())
					ResetParticle(t, v2Pos, v3Pos, angle, rot);
			}
		}
	}
	m_finished = !anythingDrawn;

	// manages the sound
	HandleSoundPlayback(v2Pos, frameSpeed);

	return true;
}

void ETHParticleManager::HandleSoundPlayback(const Vector2 &v2Pos, const float frameSpeed)
{
	if (!m_pSound)
		return;

	const Vector2 v2FinalPos = v2Pos+ETHGlobal::ToVector2(m_system.startPoint);
	if (m_nActiveParticles <= 0)
	{
		if (IsSoundLooping())
			m_pSound->Stop();
	}
	else
	{
		const VideoPtr& video = m_provider->GetVideo();
		const float move = Distance(v2FinalPos, m_v2Move);
		const float screenSize = Distance(Vector2(0,0), video->GetScreenSizeF());
		const float moveVolume = move/screenSize;
		m_soundVolume -= moveVolume;
		m_soundVolume = Max(m_soundVolume, 0.2f);
		m_soundVolume = Min(m_soundVolume, 1.0f);

		const float screenWidth = video->GetScreenSizeF().x;
		const float pan = ((Max(Min(v2FinalPos.x - video->GetCameraPos().x, screenWidth), 0.0f) / screenWidth) - 0.5f) * 2;
		m_pSound->SetPan(pan);

		float volume = 1.0f;
		if (m_isSoundLooping)
		{
			volume = (float)m_nActiveParticles / (float)m_system.nParticles;
		}
		m_pSound->SetVolume(m_soundVolume * volume * m_entityVolume * m_generalVolume);

		// if the particle system is supposed to repeat many times (nRepeat >= N), lets loop
		// it's sound effect. If it's not, then the application has to take care of it
		if ((!m_pSound->IsPlaying() || !m_pSound->GetLoop()) &&
			(m_system.repeat >= _ETH_MINIMUM_PARTICLE_REPEATS_TO_LOOP_SOUND || m_system.repeat <= 0))
		{
			m_isSoundLooping = true;

			// if it's not forced to remain quiet, play it
			if (!m_isSoundStopped)
			{
				m_pSound->Play();
				m_pSound->SetLoop(true);
			}
		}
		if (m_system.repeat < _ETH_MINIMUM_PARTICLE_REPEATS_TO_LOOP_SOUND && m_system.repeat > 0)
			m_isSoundLooping = false;

		m_soundVolume += 0.01f * frameSpeed;
	}
	m_v2Move = v2FinalPos;
}

bool ETHParticleManager::Finished() const
{
	return m_finished;
}

bool ETHParticleManager::Play(const Vector2 &v2Pos, const Vector3 &v3Pos, const float angle)
{
	if (m_pSound)
		m_pSound->Play();
	Matrix4x4 rot = RotateZ(DegreeToRadian(angle));
	m_finished = false;
	for (int t=0; t<m_system.nParticles; t++)
	{
		m_particles[t].repeat = 0;
		m_particles[t].released = false;
		ResetParticle(t, v2Pos, v3Pos, angle, rot);
	}
	return true;
}

void ETHParticleManager::BubbleSort(std::vector<PARTICLE> &v)
{
	const int len = v.size();
	for (int j = len - 1; j > 0; j--)
	{
		bool leave = true;
		for (int i = 0; i < j; i++)
		{
			if (v[i + 1] < v[i])
			{
				leave = false;
				std::swap(v[i + 1], v[i]);
			}
		}
		if (leave)
			return;
	}
}

bool ETHParticleManager::DrawParticleSystem(
	Vector3 v3Ambient,
	const float maxHeight,
	const float minHeight,
	const DEPTH_SORTING_MODE ownerType,
	const Vector2& zAxisDirection,
	const Vector2& parallaxOffset,
	const float ownerDepth)
{
	if (!m_pBMP)
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHParticleManager::DrawParticleSystem: Invalid particle system bitmap");
		m_provider->Log(ss.str(), Platform::FileLogger::WARNING);
		return false;
	}

	const VideoPtr& video = m_provider->GetVideo();
	Video::ALPHA_MODE alpha = video->GetAlphaMode();
	video->SetAlphaMode(m_system.alphaMode);

	// if the alpha blending is not additive, we'll have to sort it
	if (alpha == Video::AM_PIXEL)
	{
		BubbleSort(m_particles);
	}

	m_pBMP->SetOrigin(Sprite::EO_CENTER);
	for (int t = 0; t < m_system.nParticles; t++)
	{
		const PARTICLE& particle = m_particles[t];

		if (m_system.repeat > 0)
			if (particle.repeat >= m_system.repeat)
				continue;

		if (particle.size <= 0.0f || !particle.released)
			continue;

		if (Killed() && particle.elapsed > particle.lifeTime)
			continue;

		Vector3 v3FinalAmbient(1, 1, 1);
		if (m_system.alphaMode == Video::AM_PIXEL || m_system.alphaMode == Video::AM_ALPHA_TEST)
		{
			v3FinalAmbient.x = Min(m_system.emissive.x + v3Ambient.x, 1.0f);
			v3FinalAmbient.y = Min(m_system.emissive.y + v3Ambient.y, 1.0f);
			v3FinalAmbient.z = Min(m_system.emissive.z + v3Ambient.z, 1.0f);
		}

		Color dwColor;
		const Vector4& color(particle.color);
		dwColor.a = (GS_BYTE)(color.w * 255.0f);
		dwColor.r = (GS_BYTE)(color.x * v3FinalAmbient.x * 255.0f);
		dwColor.g = (GS_BYTE)(color.y * v3FinalAmbient.y * 255.0f);
		dwColor.b = (GS_BYTE)(color.z * v3FinalAmbient.z * 255.0f);

		// compute the right in-screen position
		const Vector2 v2Pos = ETHGlobal::ToScreenPos(Vector3(particle.pos, m_system.startPoint.z), zAxisDirection);

		// compute depth
		if (ownerType != LAYERABLE)
		{
			float offsetYZ = particle.startPoint.z;
			if (ownerType == INDIVIDUAL_OFFSET)
			{
				offsetYZ += particle.GetOffset() + _ETH_PARTICLE_DEPTH_SHIFT;
			}
			SetParticleDepth(ETHEntity::ComputeDepth(offsetYZ, maxHeight, minHeight));
		}
		else
		{
			SetParticleDepth(ownerDepth);
		}

		// draw
		if (m_system.spriteCut.x > 1 || m_system.spriteCut.y > 1)
		{
			if ((int)m_pBMP->GetNumColumns() != m_system.spriteCut.x || (int)m_pBMP->GetNumRows() != m_system.spriteCut.y)
				m_pBMP->SetupSpriteRects(m_system.spriteCut.x, m_system.spriteCut.y);
			m_pBMP->SetRect(particle.currentFrame);
		}
		else
		{
			m_pBMP->UnsetRect();
		}
		m_pBMP->DrawOptimal((v2Pos + parallaxOffset), dwColor, particle.angle, Vector2(particle.size, particle.size));
	}
	video->SetAlphaMode(alpha);
	return true;
}

void ETHParticleManager::SetTileZ(const float z)
{
	m_system.startPoint.z = z;
}

float ETHParticleManager::GetTileZ() const
{
	return m_system.startPoint.z;
}

int ETHParticleManager::GetNumActiveParticles() const
{
	return m_nActiveParticles;
}

int ETHParticleManager::GetNumParticles() const
{
	return m_system.nParticles;
}

bool ETHParticleManager::IsEndless() const
{
	return (m_system.repeat == 0);
}

void ETHParticleManager::ScaleParticleSystem(const float scale)
{
	m_system.Scale(scale);
	for (std::size_t t = 0; t < m_particles.size(); t++)
	{
		m_particles[t].Scale(scale);
	}
}

void ETHParticleManager::MirrorX(const bool mirrorGravity)
{
	m_system.MirrorX(mirrorGravity);
	for (int t = 0; t < m_system.nParticles; t++)
	{
		m_particles[t].dir.x *=-1;
		m_particles[t].pos.x *=-1;
	}
}

void ETHParticleManager::MirrorY(const bool mirrorGravity)
{
	m_system.MirrorY(mirrorGravity);
	for (int t = 0; t < m_system.nParticles; t++)
	{
		m_particles[t].dir.y *=-1;
		m_particles[t].pos.y *=-1;
	}
}

void ETHParticleManager::ResetParticle(
	const int t,
	const Vector2& v2Pos,
	const Vector3& v3Pos,
	const float angle,
	const Matrix4x4& rotMatrix)
{
	const Vector2 halfRandDir(m_system.randomizeDir / 2.0f);

	PARTICLE& particle = m_particles[t];
	particle.angleDir = m_system.angleDir + Randomizer::Float(-m_system.randAngle/2, m_system.randAngle/2);
	particle.elapsed = 0.0f;
	particle.lifeTime = m_system.lifeTime + Randomizer::Float(-m_system.randomizeLifeTime/2, m_system.randomizeLifeTime/2);
	particle.size = m_system.size + Randomizer::Float(-m_system.randomizeSize/2, m_system.randomizeSize/2);
	particle.dir.x = (m_system.directionVector.x + Randomizer::Float(-halfRandDir.x, halfRandDir.x));
	particle.dir.y = (m_system.directionVector.y + Randomizer::Float(-halfRandDir.y, halfRandDir.y));
	particle.dir = Multiply(m_particles[t].dir, rotMatrix);
	particle.color = m_system.color0;
	PositionParticle(t, v2Pos, angle, rotMatrix, v3Pos);

	// setup sprite frame
	if (m_system.spriteCut.x > 1 || m_system.spriteCut.y > 1)
	{
		if (m_system.animationMode == ETHParticleSystem::PLAY_ANIMATION)
		{
			particle.currentFrame = 0;
		} else
			if (m_system.animationMode == ETHParticleSystem::PICK_RANDOM_FRAME)
			{
				particle.currentFrame = Randomizer::Int(m_system.spriteCut.x * m_system.spriteCut.y - 1);
			}
	}
}

void ETHParticleManager::PositionParticle(
	const int t,
	const Vector2& v2Pos,
	const float angle,
	const Matrix4x4& rotMatrix,
	const Vector3& v3Pos)
{
	const Vector2 halfRandStartPoint(m_system.randStartPoint / 2.0f);

	PARTICLE& particle = m_particles[t];
	particle.angle = m_system.angleStart + Randomizer::Float(m_system.randAngleStart) + angle;
	particle.pos.x = m_system.startPoint.x + Randomizer::Float(-halfRandStartPoint.x, halfRandStartPoint.x);
	particle.pos.y = m_system.startPoint.y + Randomizer::Float(-halfRandStartPoint.y, halfRandStartPoint.y);
	particle.pos = Multiply(particle.pos, rotMatrix);	
	particle.pos = m_particles[t].pos + v2Pos;
	particle.startPoint = Vector3(v2Pos, v3Pos.z) + m_system.startPoint;
}

void ETHParticleManager::SetParticleDepth(const float depth)
{
	m_provider->GetVideo()->SetSpriteDepth(depth);
}

bool ETHParticleManager::Killed() const
{
	return m_killed;
}
