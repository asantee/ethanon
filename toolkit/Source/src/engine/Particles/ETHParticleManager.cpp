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
	const Vector3& v3Pos,
	const float angle) :
	m_provider(provider)
{
	ETHParticleSystem partSystem;
	if (partSystem.ReadFromFile(file, m_provider->GetFileManager()))
	{
		CreateParticleSystem(partSystem, v3Pos, angle, 1.0f);
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
	const Vector3& v3Pos,
	const float angle,
	const float scale) :
	m_provider(provider)
{
	CreateParticleSystem(partSystem, v3Pos, angle, scale);
}

bool ETHParticleManager::CreateParticleSystem(
	const ETHParticleSystem& partSystem,
	const Vector3& v3Pos,
	const float angle,
	const float scale)
{
	if (partSystem.nParticles <= 0)
	{
		ETH_STREAM_DECL(ss) << GS_L("ETHParticleManager::CreateParticleSystem: The number of particles must be greater than 0.");
		m_provider->Log(ss.str(), Platform::FileLogger::ERROR);
		return false;
	}

	m_finished = false;
	m_killed = false;
	m_nActiveParticles = 0;
	m_system = partSystem;

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

	m_pBMP = graphics->GetPointer(fileManager, m_provider->GetVideo(), m_system.bitmapFile, currentPath,
		ETHDirectories::GetParticlesDirectory(), (m_system.alphaMode == Video::AM_ADD));

	if (m_system.allAtOnce)
	{
		m_nActiveParticles = m_system.nParticles;
	}
	else
	{
		m_nActiveParticles = 0;
	}

	m_particles.resize(m_system.nParticles);

	Matrix4x4 rot = Matrix4x4::RotateZ(Util::DegreeToRadian(angle));
	for (int t = 0; t < m_system.nParticles; t++)
	{
		m_particles[t].id = t;
		m_particles[t].released = false;
		ResetParticle(m_system, m_particles[t], v3Pos, angle, rot);
	}
	return true;
}

Vector3 ETHParticleManager::GetStartPos() const
{
	return m_system.startPoint;
}

void ETHParticleManager::SetStartPos(const Vector3& v3Pos)
{
	m_system.startPoint = v3Pos;
}

str_type::string ETHParticleManager::GetBitmapName() const
{
	return m_system.bitmapFile;
}

void ETHParticleManager::Kill(const bool kill)
{
	m_killed = kill;
}

void ETHParticleManager::SetSystem(const ETHParticleSystem &partSystem)
{
	m_system = partSystem;
}

void ETHParticleManager::SetParticleBitmap(SpritePtr pBMP)
{
	m_pBMP = pBMP;
}

void ETHParticleManager::SetParticleBitmap(const gs2d::str_type::string& bitmap)
{
	m_system.bitmapFile = bitmap;
	SetParticleBitmap(m_provider->GetGraphicResourceManager()->GetPointer(
		m_provider->GetFileManager(),
		m_provider->GetVideo(),
		bitmap,
		m_provider->GetFileIOHub()->GetResourceDirectory(),
		ETHDirectories::GetParticlesDirectory(),
		false));
}

SpritePtr ETHParticleManager::GetParticleBitmap()
{
	return m_pBMP;
}

const ETHParticleSystem *ETHParticleManager::GetSystem() const
{
	return &m_system;
}

void ETHParticleManager::UpdateParticleSystem(
	const Vector3& v3Pos,
	const Vector2& zAxisDirection,
	const Vector2& parallaxOffset,
	const float angle,
	const float lastFrameElapsedTime)
{
	bool anythingDrawn = false;
	const float cappedLastFrameElapsedTime = Min(lastFrameElapsedTime, 250.0f);

	Matrix4x4 rot = Matrix4x4::RotateZ(Util::DegreeToRadian(angle));
	m_nActiveParticles = 0;

	m_worldSpaceBoundingMin = ETHGlobal::ToScreenPos(v3Pos + m_system.startPoint, zAxisDirection);
	m_worldSpaceBoundingMax = m_worldSpaceBoundingMin;

	for (int t = 0; t < m_system.nParticles; t++)
	{
		PARTICLE& particle = m_particles[t];

		UpdateParticle(
			m_system,
			particle,
			false, // hasJustBeenReset
			Killed(),
			v3Pos,
			angle,
			cappedLastFrameElapsedTime,
			rot,
			anythingDrawn,
			m_nActiveParticles);
		
		const Vector2 worldSpacePos =
			ETHGlobal::ToScreenPos(Vector3(particle.pos, m_system.startPoint.z), zAxisDirection) + parallaxOffset;
		m_worldSpaceBoundingMin = Vector2::Vector2Min(m_worldSpaceBoundingMin, worldSpacePos - (particle.size * 0.5f));
		m_worldSpaceBoundingMax = Vector2::Vector2Max(m_worldSpaceBoundingMax, worldSpacePos + (particle.size * 0.5f));
	}
	m_finished = !anythingDrawn;
}

Vector2 ETHParticleManager::GetWorldSpaceBoundingMin() const
{
	return m_worldSpaceBoundingMin;
}

Vector2 ETHParticleManager::GetWorldSpaceBoundingMax() const
{
	return m_worldSpaceBoundingMax;
}

bool ETHParticleManager::IsVisible(
	const Vector2& worldSpaceScreenMin,
	const Vector2& worldSpaceScreenMax) const
{
	if (   m_worldSpaceBoundingMin.x > worldSpaceScreenMax.x || m_worldSpaceBoundingMin.y > worldSpaceScreenMax.y
		|| m_worldSpaceBoundingMax.x < worldSpaceScreenMin.x || m_worldSpaceBoundingMax.y < worldSpaceScreenMin.y)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ETHParticleManager::UpdateParticle(
	const ETHParticleSystem& system,
	PARTICLE& particle,
	const bool hasJustBeenReset,
	const bool killed,
	const Vector3& v3Pos,
	const float angle,
	const float lastFrameElapsedTime,
	const Matrix4x4& rot,
	bool& anythingDrawn,
	int& activeParticles)
{
	if (system.repeat > 0)
		if (particle.repeat >= system.repeat)
			return;

	// check how many particles are active
	if (particle.size > 0.0f && particle.released)
	{
		if (!hasJustBeenReset)
			if (!killed || (killed && particle.elapsed < particle.lifeTime))
				activeParticles++;
	}

	anythingDrawn = true;

	particle.elapsed += lastFrameElapsedTime;

	if (!particle.released)
	{
		// if we shouldn't release all particles at the same time, check if it's time to release this particle
		const float releaseTime = 
			((system.lifeTime + system.randomizeLifeTime) * (static_cast<float>(particle.id) / static_cast<float>(system.nParticles)));

		if (particle.elapsed > releaseTime || system.allAtOnce)
		{
			particle.elapsed = 0.0f;
			particle.released = true;
			PositionParticle(system, particle, angle, rot, v3Pos);
		}
	}

	if (particle.released)
	{
		const float frameSpeed = static_cast<float>((static_cast<double>(lastFrameElapsedTime) / 1000.0) * 60.0);
		particle.dir += (system.gravityVector * frameSpeed);
		particle.pos += (particle.dir * frameSpeed);
		particle.angle += (particle.angleDir * frameSpeed);
		particle.size  += (system.growth * frameSpeed);
		const float w = particle.elapsed / particle.lifeTime;
		particle.color = system.color0 + (system.color1 - system.color0) * w;

		// update particle animation if there is any
		if (system.spriteCut.x > 1 || system.spriteCut.y > 1)
		{
			if (system.animationMode == ETHParticleSystem::PLAY_ANIMATION)
			{
				particle.currentFrame = static_cast<unsigned int>(
					Min(static_cast<int>(static_cast<float>(system.GetNumFrames()) * w),
						system.GetNumFrames() - 1));
			}
		}

		particle.size = Min(particle.size, system.maxSize);
		particle.size = Max(particle.size, system.minSize);

		if (particle.elapsed > particle.lifeTime)
		{
			particle.repeat++;
			if (!killed)
			{
				const float diff = particle.elapsed - particle.lifeTime;
				ResetParticle(system, particle, v3Pos, angle, rot);
				if (diff > 0.0f && particle.lifeTime > 0.0f)
				{
					UpdateParticle(
						system,
						particle,
						true, // hasJustBeenReset
						killed,
						v3Pos,
						angle,
						diff,
						rot,
						anythingDrawn,
						activeParticles);
				}
			}
		}
	}
}

Rect2D ETHParticleManager::ComputeBoundingRectangle(const float angle) const
{
	PARTICLE particle;
	particle.id = 0;
	particle.released = false;
	Matrix4x4 rot = Matrix4x4::RotateZ(Util::DegreeToRadian(angle));

	ETHParticleSystem staticSystem = m_system;
	staticSystem.size += (staticSystem.randomizeSize / 2.0f);
	staticSystem.lifeTime += (staticSystem.randomizeLifeTime / 2);
	staticSystem.randomizeLifeTime = 0;
	staticSystem.randomizeSize = 0.0f;
	staticSystem.randStartPoint = Vector2(0.0f, 0.0f);
	staticSystem.randomizeDir = Vector2(0.0f, 0.0f);

	ResetParticle(staticSystem, particle, Vector3(0, 0, 0), angle, rot);

	const Vector2 startMin(particle.pos - (particle.size / 2.0f) - (m_system.randStartPoint * 0.5f));
	const Vector2 startMax(particle.pos + (particle.size / 2.0f) + (m_system.randStartPoint * 0.5f));

	int activeParticles = 0; bool anythingDrawn = false;
	UpdateParticle(
		staticSystem,
		particle,
		false,
		false,
		Vector3(0,0,0),
		angle,
		staticSystem.lifeTime + (staticSystem.randomizeLifeTime / 2),
		rot,
		anythingDrawn,
		activeParticles);

	const Vector2 endMin(particle.pos - (particle.size / 2.0f) - (m_system.randStartPoint * 0.5f));
	const Vector2 endMax(particle.pos + (particle.size / 2.0f) + (m_system.randStartPoint * 0.5f));

	const Vector2 finalMin(Vector2::Vector2Min(startMin, endMin));
	const Vector2 finalMax(Vector2::Vector2Max(startMax, endMax));

	return Rect2D((finalMin + finalMax) / 2.0f, finalMax - finalMin);
}

bool ETHParticleManager::Finished() const
{
	return m_finished;
}

bool ETHParticleManager::Play(
	const Vector3 &v3Pos,
	const float angle)
{
	Matrix4x4 rot = Matrix4x4::RotateZ(Util::DegreeToRadian(angle));
	m_finished = false;
	for (int t = 0; t < m_system.nParticles; t++)
	{
		m_particles[t].repeat = 0;
		m_particles[t].released = false;
		ResetParticle(m_system, m_particles[t], v3Pos, angle, rot);
	}
	return true;
}

void ETHParticleManager::Sort(std::vector<PARTICLE> &v)
{
	const int len = static_cast<int>(v.size());
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

float ETHParticleManager::ComputeParticleDepth(
	const DEPTH_SORTING_MODE& ownerType,
	const float& ownerDepth,
	const PARTICLE& particle,
	const float& maxHeight,
	const float& minHeight)
{
	float r = 0.0f;

	// compute depth
	if (ownerType == LAYERABLE)
	{
		r = (ownerDepth);
	}
	else
	{
		float offsetYZ = particle.startPoint.z;
		if (ownerType == INDIVIDUAL_OFFSET)
		{
			offsetYZ += particle.GetOffset() + GetParticleDepthShift(ownerType);
		}
		r = (ETHEntity::ComputeDepth(offsetYZ, maxHeight, minHeight));
	}
	return r;
}

float ETHParticleManager::GetParticleDepthShift(const DEPTH_SORTING_MODE& ownerType)
{
	return (ownerType == INDIVIDUAL_OFFSET) ? _ETH_PARTICLE_DEPTH_SHIFT : 0.0f;
}

bool ETHParticleManager::DrawParticleSystem(
	Vector3 ambient,
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
		Sort(m_particles);
	}

	const bool shouldUseHighlightPS = m_system.ShouldUseHighlightPS();
	const ShaderPtr& currentShader = m_provider->GetVideo()->GetCurrentShader();

	m_pBMP->SetOrigin(Sprite::EO_CENTER);
	for (int t = 0; t < m_system.nParticles; t++)
	{
		const PARTICLE& particle = m_particles[t];

		if (m_system.repeat > 0)
			if (particle.repeat >= m_system.repeat)
				continue;

		if (particle.size <= 0.0f || !particle.released || particle.color.w <= 0.0f)
			continue;

		if (Killed() && particle.elapsed > particle.lifeTime)
			continue;

		Vector3 finalAmbient(1, 1, 1);
		if (m_system.alphaMode == Video::AM_PIXEL || m_system.alphaMode == Video::AM_ALPHA_TEST)
		{
			finalAmbient.x = Min(m_system.emissive.x + ambient.x, 1.0f);
			finalAmbient.y = Min(m_system.emissive.y + ambient.y, 1.0f);
			finalAmbient.z = Min(m_system.emissive.z + ambient.z, 1.0f);
		}

		Vector4 finalColor = particle.color * Vector4(finalAmbient, 1.0f);

		SetParticleDepth(ComputeParticleDepth(ownerType, ownerDepth, particle, maxHeight, minHeight));

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
		
		if (shouldUseHighlightPS)
		{
			currentShader->SetConstant(GS_L("highlight"), finalColor);
		}
		
		const Vector2 v2Pos =
			ETHGlobal::ToScreenPos(Vector3(particle.pos, particle.startPoint.z), zAxisDirection) + parallaxOffset;
		m_pBMP->DrawOptimal(v2Pos, finalColor, particle.angle, Vector2(particle.size, particle.size));
	}
	video->SetAlphaMode(alpha);
	return true;
}

void ETHParticleManager::SetZPosition(const float z)
{
	m_system.startPoint.z = z;
}

float ETHParticleManager::GetZPosition() const
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

void ETHParticleManager::SetParticleColorA(const Vector3 &color, const float alpha)
{
	m_system.color0 = Vector4(color, alpha);
}

void ETHParticleManager::SetParticleColorB(const Vector3 &color, const float alpha)
{
	m_system.color1 = Vector4(color, alpha);
}

void ETHParticleManager::ResetParticle(
	const ETHParticleSystem& system,
	PARTICLE& particle,
	const Vector3& v3Pos,
	const float angle,
	const Matrix4x4& rotMatrix)
{
	const Vector2 halfRandDir(system.randomizeDir / 2.0f);

	particle.angleDir = system.angleDir + Randomizer::Float(-system.randAngle / 2, system.randAngle / 2);
	particle.elapsed = 0.0f;
	particle.lifeTime = system.lifeTime + Randomizer::Float(-system.randomizeLifeTime / 2, system.randomizeLifeTime / 2);
	particle.size = system.size + Randomizer::Float(-system.randomizeSize / 2, system.randomizeSize/2);
	particle.dir.x = (system.directionVector.x + Randomizer::Float(-halfRandDir.x, halfRandDir.x));
	particle.dir.y = (system.directionVector.y + Randomizer::Float(-halfRandDir.y, halfRandDir.y));
	particle.dir = Matrix4x4::Multiply(particle.dir, rotMatrix);
	particle.color = system.color0;
	PositionParticle(system, particle, angle, rotMatrix, v3Pos);

	// setup sprite frame
	if (system.spriteCut.x > 1 || system.spriteCut.y > 1)
	{
		if (system.animationMode == ETHParticleSystem::PLAY_ANIMATION)
		{
			particle.currentFrame = 0;
		}
		else
		{
			if (system.animationMode == ETHParticleSystem::PICK_RANDOM_FRAME)
			{
				particle.currentFrame = Randomizer::Int(system.spriteCut.x * system.spriteCut.y - 1);
			}
		}
	}
}

void ETHParticleManager::PositionParticle(
	const ETHParticleSystem& system,
	PARTICLE& particle,
	const float angle,
	const Matrix4x4& rotMatrix,
	const Vector3& v3Pos)
{
	const Vector2 halfRandStartPoint(system.randStartPoint / 2.0f);

	particle.angle = system.angleStart + Randomizer::Float(system.randAngleStart) + angle;
	particle.pos.x = system.startPoint.x + Randomizer::Float(-halfRandStartPoint.x, halfRandStartPoint.x);
	particle.pos.y = system.startPoint.y + Randomizer::Float(-halfRandStartPoint.y, halfRandStartPoint.y);
	particle.pos = Matrix4x4::Multiply(particle.pos, rotMatrix);
	particle.pos = particle.pos + Vector2(v3Pos.x, v3Pos.y);
	particle.startPoint = v3Pos + system.startPoint;
}

void ETHParticleManager::SetParticleDepth(const float depth)
{
	m_provider->GetVideo()->SetSpriteDepth(depth);
}

bool ETHParticleManager::Killed() const
{
	return m_killed;
}
