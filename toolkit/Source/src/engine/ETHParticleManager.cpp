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
#include "Resource/ETHDirectories.h"
#include "Util/ETHASUtil.h"
#include "Entity/ETHEntity.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

ETH_PARTICLE::ETH_PARTICLE()
{
	released = false;
	repeat = 0;
	id =-1;
	currentFrame = 0;
}

void ETH_PARTICLE_SYSTEM::Scale(const float scale)
{
	boundingSphere *= scale;
	v2GravityVector *= scale;
	v2DirectionVector *= scale;
	v2RandomizeDir *= scale;
	v3StartPoint *= scale;
	v2RandStartPoint *= scale;
	size *= scale;
	randomizeSize *= scale;
	growth *= scale;
	minSize *= scale;
	maxSize *= scale;
}

void ETH_PARTICLE_SYSTEM::MirrorX(const bool mirrorGravity)
{
	if (mirrorGravity)
		v2GravityVector.x *=-1;
	v3StartPoint.x *=-1;
	v2DirectionVector.x *=-1;
	v2RandomizeDir.x *=-1;
	v2RandStartPoint.x *=-1;
}

void ETH_PARTICLE_SYSTEM::MirrorY(const bool mirrorGravity)
{
	if (mirrorGravity)
		v2GravityVector.y *=-1;
	v3StartPoint.y *=-1;
	v2DirectionVector.y *=-1;
	v2RandomizeDir.y *=-1;
	v2RandStartPoint.y *=-1;
}

ETH_PARTICLE_SYSTEM::ETH_PARTICLE_SYSTEM()
{
	Reset();
}

void ETH_PARTICLE_SYSTEM::Reset()
{
	alphaMode = GSAM_PIXEL;
	nParticles = 0;
	lifeTime = 0.0f;
	randomizeLifeTime = 0.0f;
	size = 1.0f;
	growth = 0.0f;
	minSize = 0.0f;
	maxSize = 99999.0f;
	repeat = 0;
	randomizeSize = 0.0f;
	randAngleStart = 0.0f;
	angleStart = 0.0f;
	emissive = Vector3(1,1,1);
	allAtOnce = false;
	boundingSphere = 512.0f;
	soundFXFile = GS_L("");
	v2SpriteCut = Vector2i(1,1);
	animationMode = _ETH_PLAY_ANIMATION;
}

bool ETH_PARTICLE_SYSTEM::ReadFromXMLFile(TiXmlElement *pElement)
{
	pElement->QueryIntAttribute(GS_L("particles"), &nParticles);

	int tempAllAtOnce;
	pElement->QueryIntAttribute(GS_L("allAtOnce"), &tempAllAtOnce);
	allAtOnce = static_cast<ETH_BOOL>(tempAllAtOnce);

	pElement->QueryIntAttribute(GS_L("alphaMode"), (int*)&alphaMode);
	pElement->QueryIntAttribute(GS_L("repeat"), &repeat);
	pElement->QueryFloatAttribute(GS_L("boundingSphere"), &boundingSphere);
	pElement->QueryFloatAttribute(GS_L("lifeTime"), &lifeTime);
	pElement->QueryFloatAttribute(GS_L("randomLifeTime"), &randomizeLifeTime);
	pElement->QueryFloatAttribute(GS_L("angleDir"), &angleDir);
	pElement->QueryFloatAttribute(GS_L("randAngle"), &randAngle);
	pElement->QueryFloatAttribute(GS_L("size"), &size);
	pElement->QueryFloatAttribute(GS_L("randomizeSize"), &randomizeSize);
	pElement->QueryFloatAttribute(GS_L("growth"), &growth);
	pElement->QueryFloatAttribute(GS_L("minSize"), &minSize);
	pElement->QueryFloatAttribute(GS_L("maxSize"), &maxSize);
	pElement->QueryFloatAttribute(GS_L("angleStart"), &angleStart);
	pElement->QueryFloatAttribute(GS_L("randAngleStart"), &randAngleStart);
	pElement->QueryIntAttribute(GS_L("animationMode"), &animationMode);

	TiXmlNode *pNode;
	TiXmlElement *pStringElement;

	pNode = pElement->FirstChild(GS_L("Bitmap"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			bitmapFile = pStringElement->GetText();
		}
	}

	pNode = pElement->FirstChild(GS_L("SoundEffect"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			soundFXFile = pStringElement->GetText();
		}
	}

	TiXmlElement *pIter;
	pNode = pElement->FirstChild(GS_L("Gravity"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &v2GravityVector.x);
			pIter->QueryFloatAttribute(GS_L("y"), &v2GravityVector.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("Direction"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &v2DirectionVector.x);
			pIter->QueryFloatAttribute(GS_L("y"), &v2DirectionVector.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("RandomizeDir"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &v2RandomizeDir.x);
			pIter->QueryFloatAttribute(GS_L("y"), &v2RandomizeDir.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("StartPoint"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &v3StartPoint.x);
			pIter->QueryFloatAttribute(GS_L("y"), &v3StartPoint.y);
			pIter->QueryFloatAttribute(GS_L("z"), &v3StartPoint.z);
		}
	}

	pNode = pElement->FirstChild(GS_L("RandStartPoint"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &v2RandStartPoint.x);
			pIter->QueryFloatAttribute(GS_L("y"), &v2RandStartPoint.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("SpriteCut"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryIntAttribute(GS_L("x"), &v2SpriteCut.x);
			pIter->QueryIntAttribute(GS_L("y"), &v2SpriteCut.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("Color0"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &v4Color0.x);
			pIter->QueryFloatAttribute(GS_L("g"), &v4Color0.y);
			pIter->QueryFloatAttribute(GS_L("b"), &v4Color0.z);
			pIter->QueryFloatAttribute(GS_L("a"), &v4Color0.w);
		}
	}

	pNode = pElement->FirstChild(GS_L("Color1"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &v4Color1.x);
			pIter->QueryFloatAttribute(GS_L("g"), &v4Color1.y);
			pIter->QueryFloatAttribute(GS_L("b"), &v4Color1.z);
			pIter->QueryFloatAttribute(GS_L("a"), &v4Color1.w);
		}
	}

	pNode = pElement->FirstChild(GS_L("Luminance"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &emissive.x);
			pIter->QueryFloatAttribute(GS_L("g"), &emissive.y);
			pIter->QueryFloatAttribute(GS_L("b"), &emissive.z);
		}
	}
	return true;
}

bool ETH_PARTICLE_SYSTEM::ReadFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager)
{
	TiXmlDocument doc(fileName);
	str_type::string content;
	fileManager->GetUTF16FileString(fileName, content);
	if (!doc.LoadFile(content, TIXML_ENCODING_LEGACY))
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	TiXmlElement *pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
		return false;

	hRoot = TiXmlHandle(pElem);

	return ReadFromXMLFile(hRoot.FirstChildElement().Element());
}

bool ETH_PARTICLE_SYSTEM::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pParticleRoot = new TiXmlElement(GS_L("ParticleSystem"));
	pRoot->LinkEndChild(pParticleRoot); 

	TiXmlElement *pElement;

	if (soundFXFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("SoundEffect"));
		pElement->LinkEndChild(
			new TiXmlText( Platform::GetFileName(soundFXFile) )
		);
		pParticleRoot->LinkEndChild(pElement);
	}

	if (bitmapFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("Bitmap"));
		pElement->LinkEndChild(new TiXmlText( Platform::GetFileName(bitmapFile)));
		pParticleRoot->LinkEndChild(pElement);
	}

	pElement = new TiXmlElement(GS_L("Gravity"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), v2GravityVector.x);
	pElement->SetDoubleAttribute(GS_L("y"), v2GravityVector.y);

	pElement = new TiXmlElement(GS_L("Direction"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), v2DirectionVector.x);
	pElement->SetDoubleAttribute(GS_L("y"), v2DirectionVector.y);

	pElement = new TiXmlElement(GS_L("RandomizeDir"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), v2RandomizeDir.x);
	pElement->SetDoubleAttribute(GS_L("y"), v2RandomizeDir.y);

	pElement = new TiXmlElement(GS_L("SpriteCut"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), v2SpriteCut.x);
	pElement->SetDoubleAttribute(GS_L("y"), v2SpriteCut.y);

	pElement = new TiXmlElement(GS_L("StartPoint"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), v3StartPoint.x);
	pElement->SetDoubleAttribute(GS_L("y"), v3StartPoint.y);
	pElement->SetDoubleAttribute(GS_L("z"), v3StartPoint.z);

	pElement = new TiXmlElement(GS_L("RandStartPoint"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), v2RandStartPoint.x);
	pElement->SetDoubleAttribute(GS_L("y"), v2RandStartPoint.y);

	pElement = new TiXmlElement(GS_L("Color0"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), v4Color0.x);
	pElement->SetDoubleAttribute(GS_L("g"), v4Color0.y);
	pElement->SetDoubleAttribute(GS_L("b"), v4Color0.z);
	pElement->SetDoubleAttribute(GS_L("a"), v4Color0.w);

	pElement = new TiXmlElement(GS_L("Color1"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), v4Color1.x);
	pElement->SetDoubleAttribute(GS_L("g"), v4Color1.y);
	pElement->SetDoubleAttribute(GS_L("b"), v4Color1.z);
	pElement->SetDoubleAttribute(GS_L("a"), v4Color1.w);

	pElement = new TiXmlElement(GS_L("Luminance"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), emissive.x);
	pElement->SetDoubleAttribute(GS_L("g"), emissive.y);
	pElement->SetDoubleAttribute(GS_L("b"), emissive.z);

	pParticleRoot->SetAttribute(GS_L("particles"), nParticles);
	pParticleRoot->SetAttribute(GS_L("allAtOnce"), allAtOnce);
	pParticleRoot->SetAttribute(GS_L("alphaMode"), alphaMode);
	pParticleRoot->SetAttribute(GS_L("repeat"), repeat);
	pParticleRoot->SetAttribute(GS_L("animationMode"), animationMode);
	pParticleRoot->SetDoubleAttribute(GS_L("boundingSphere"), boundingSphere);
	pParticleRoot->SetDoubleAttribute(GS_L("lifeTime"), lifeTime);
	pParticleRoot->SetDoubleAttribute(GS_L("randomLifeTime"), randomizeLifeTime);
	pParticleRoot->SetDoubleAttribute(GS_L("angleDir"), angleDir);
	pParticleRoot->SetDoubleAttribute(GS_L("randAngle"), randAngle);
	pParticleRoot->SetDoubleAttribute(GS_L("size"), size);
	pParticleRoot->SetDoubleAttribute(GS_L("randomizeSize"), randomizeSize);
	pParticleRoot->SetDoubleAttribute(GS_L("growth"), growth);
	pParticleRoot->SetDoubleAttribute(GS_L("minSize"), minSize);
	pParticleRoot->SetDoubleAttribute(GS_L("maxSize"), maxSize);
	pParticleRoot->SetDoubleAttribute(GS_L("angleStart"), angleStart);
	pParticleRoot->SetDoubleAttribute(GS_L("randAngleStart"), randAngleStart);	

	return true;
}

int ETH_PARTICLE_SYSTEM::GetNumFrames() const
{
	return v2SpriteCut.x * v2SpriteCut.y;
}

str_type::string ETH_PARTICLE_SYSTEM::GetActualBitmapFile() const
{
	return (bitmapFile == GS_L("")) ? ETH_DEFAULT_PARTICLE_BITMAP : bitmapFile;
}

ETHParticleManager::ETHParticleManager(ETHResourceProviderPtr provider, const str_type::string& file, const Vector2 &v2Pos,
	const Vector3 &v3Pos, const float angle, const float entityVolume) :
	m_provider(provider)
{
	ETH_PARTICLE_SYSTEM partSystem;
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

ETHParticleManager::ETHParticleManager(ETHResourceProviderPtr provider, const ETH_PARTICLE_SYSTEM &partSystem, const Vector2 &v2Pos,
									   const Vector3 &v3Pos, const float angle, const float entityVolume, const float scale) :
	m_provider(provider)
{
	CreateParticleSystem(partSystem, v2Pos, v3Pos, angle, entityVolume, scale);
}

bool ETHParticleManager::CreateParticleSystem(const ETH_PARTICLE_SYSTEM &partSystem, const Vector2 &v2Pos,
											  const Vector3 &v3Pos, const float angle, const float entityVolume,
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
		ETHDirectories::GetParticlesDirectory(), (m_system.alphaMode == GSAM_ADD));

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
	for (int t=0; t<m_system.nParticles; t++)
	{
		m_particles[t].id = t;
		//if (m_system.allAtOnce)
		//	m_particles[t].released = true;
		//else
		m_particles[t].released = false;
		ResetParticle(t, v2Pos, Vector3(v2Pos,0), angle, rot);
	}
	//m_particles[0].released = true;
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
	return m_system.v3StartPoint;
}

void ETHParticleManager::SetStartPos(const Vector3& v3Pos)
{
	m_system.v3StartPoint = v3Pos;
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

void ETHParticleManager::SetSystem(const ETH_PARTICLE_SYSTEM &partSystem)
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

const ETH_PARTICLE_SYSTEM *ETHParticleManager::GetSystem() const
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

bool ETHParticleManager::UpdateParticleSystem(const Vector2 &v2Pos, const Vector3 &v3Pos, const float angle, const unsigned long lastFrameElapsedTime)
{
	bool anythingDrawn = false;
	const unsigned long cappedLastFrameElapsedTime = Min(lastFrameElapsedTime, static_cast<unsigned long>(250));
	const float frameSpeed = static_cast<float>((static_cast<double>(cappedLastFrameElapsedTime) / 1000.0) * 60.0);

	Matrix4x4 rot = RotateZ(DegreeToRadian(angle));
	m_nActiveParticles = 0;
	for (int t = 0; t < m_system.nParticles; t++)
	{
		ETH_PARTICLE& particle = m_particles[t];

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
			particle.v2Dir += (m_system.v2GravityVector * frameSpeed);
			particle.v2Pos += (particle.v2Dir * frameSpeed);
			particle.angle += (particle.angleDir * frameSpeed);
			particle.size  += (m_system.growth * frameSpeed);
			const float w = particle.elapsed / particle.lifeTime;
			particle.v4Color = m_system.v4Color0 + (m_system.v4Color1 - m_system.v4Color0) * w;

			// update particle animation if there is any
			if (m_system.v2SpriteCut.x > 1 || m_system.v2SpriteCut.y > 1)
			{
				if (m_system.animationMode == _ETH_PLAY_ANIMATION)
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

	const Vector2 v2FinalPos = v2Pos+ETHGlobal::ToVector2(m_system.v3StartPoint);
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

void ETHParticleManager::BubbleSort(std::vector<ETH_PARTICLE> &v)
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

bool ETHParticleManager::DrawParticleSystem(Vector3 v3Ambient, const float maxHeight, const float minHeight, const ETH_ENTITY_TYPE ownerType,
											const Vector2 &zAxisDirection, const Vector2 &parallaxOffset, const float ownerDepth)
{
	if (!m_pBMP)
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHParticleManager::DrawParticleSystem: Invalid particle system bitmap");
		m_provider->Log(ss.str(), Platform::FileLogger::WARNING);
		return false;
	}

	const VideoPtr& video = m_provider->GetVideo();
	GS_ALPHA_MODE alpha = video->GetAlphaMode();
	video->SetAlphaMode(m_system.alphaMode);

	// if the alpha blending is not additive, we'll have to sort it
	if (alpha == GSAM_PIXEL)
	{
		BubbleSort(m_particles);
	}

	m_pBMP->SetOrigin(GSEO_CENTER);
	for (int t = 0; t < m_system.nParticles; t++)
	{
		const ETH_PARTICLE& particle = m_particles[t];

		if (m_system.repeat > 0)
			if (particle.repeat >= m_system.repeat)
				continue;

		if (particle.size <= 0.0f || !particle.released)
			continue;

		if (Killed() && particle.elapsed > particle.lifeTime)
			continue;

		Vector3 v3FinalAmbient(1, 1, 1);
		if (m_system.alphaMode == GSAM_PIXEL || m_system.alphaMode == GSAM_ALPHA_TEST)
		{
			v3FinalAmbient.x = Min(m_system.emissive.x + v3Ambient.x, 1.0f);
			v3FinalAmbient.y = Min(m_system.emissive.y + v3Ambient.y, 1.0f);
			v3FinalAmbient.z = Min(m_system.emissive.z + v3Ambient.z, 1.0f);
		}

		GS_COLOR dwColor;
		const Vector4& color(particle.v4Color);
		dwColor.a = (GS_BYTE)(color.w * 255.0f);
		dwColor.r = (GS_BYTE)(color.x * v3FinalAmbient.x * 255.0f);
		dwColor.g = (GS_BYTE)(color.y * v3FinalAmbient.y * 255.0f);
		dwColor.b = (GS_BYTE)(color.z * v3FinalAmbient.z * 255.0f);

		// compute the right in-screen position
		const Vector2 v2Pos = ETHGlobal::ToScreenPos(Vector3(particle.v2Pos, m_system.v3StartPoint.z), zAxisDirection);

		// compute depth
		if (ownerType != ETH_LAYERABLE)
		{
			float offsetYZ = particle.v3StartPoint.z;
			if (ownerType == ETH_VERTICAL)
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
		if (m_system.v2SpriteCut.x > 1 || m_system.v2SpriteCut.y > 1)
		{
			if ((int)m_pBMP->GetNumColumns() != m_system.v2SpriteCut.x || (int)m_pBMP->GetNumRows() != m_system.v2SpriteCut.y)
				m_pBMP->SetupSpriteRects(m_system.v2SpriteCut.x, m_system.v2SpriteCut.y);
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
	m_system.v3StartPoint.z = z;
}

float ETHParticleManager::GetTileZ() const
{
	return m_system.v3StartPoint.z;
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
		m_particles[t].v2Dir.x *=-1;
		m_particles[t].v2Pos.x *=-1;
	}
}

void ETHParticleManager::MirrorY(const bool mirrorGravity)
{
	m_system.MirrorY(mirrorGravity);
	for (int t = 0; t < m_system.nParticles; t++)
	{
		m_particles[t].v2Dir.y *=-1;
		m_particles[t].v2Pos.y *=-1;
	}
}
