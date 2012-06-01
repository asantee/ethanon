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

#ifndef ETH_PARTICLE_MANAGER_H_
#define ETH_PARTICLE_MANAGER_H_

#include "Resource/ETHResourceProvider.h"
#include "ETHCommon.h"

#define _ETH_MINIMUM_PARTICLE_REPEATS_TO_LOOP_SOUND (4)
#define _ETH_PARTICLE_DEPTH_SHIFT (10.0f)

#define _ETH_PLAY_ANIMATION 1
#define _ETH_PICK_RANDOM_FRAME 2

struct ETH_PARTICLE
{
	ETH_PARTICLE();
	inline bool operator < (const ETH_PARTICLE &other) const
	{
		return (GetOffset() < other.GetOffset());
	}
	inline float GetOffset() const
	{
		return v3StartPoint.y-v2Pos.y;
	}
	inline void Scale(const float& scale)
	{
		size *= scale;
	}
	Vector2 v2Pos;
	Vector2 v2Dir;
	Vector4 v4Color;
	Vector3 v3StartPoint;
	float angle;
	float angleDir;
	float size;
	float lifeTime;
	float elapsed;
	int repeat;
	bool released;
	int id;
	unsigned int currentFrame;
};

struct ETH_PARTICLE_SYSTEM
{
	ETH_PARTICLE_SYSTEM();
	void Reset();

	void Scale(const float scale);
	void MirrorX(const bool mirrorGravity);
	void MirrorY(const bool mirrorGravity);

	bool ReadFromXMLFile(TiXmlElement *pElement);

	bool ReadFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager);
	bool WriteToXMLFile(TiXmlElement *pRoot) const;
	int GetNumFrames() const;

	str_type::string GetActualBitmapFile() const;

	str_type::string bitmapFile;
	str_type::string soundFXFile;
	ETH_BOOL allAtOnce;
	float boundingSphere;
	GS_ALPHA_MODE alphaMode;
	int nParticles;
	Vector2 v2GravityVector;
	Vector2 v2DirectionVector;
	Vector2 v2RandomizeDir;
	Vector3 v3StartPoint;
	Vector2 v2RandStartPoint;

	Vector2i v2SpriteCut;

	Vector4 v4Color0;
	Vector4 v4Color1;

	float lifeTime;
	float randomizeLifeTime;

	float angleDir;
	float randAngle;
	float size;
	float randomizeSize;
	float growth;
	float minSize;
	float maxSize;

	int repeat;
	int animationMode;
	Vector3 v3Luminance;
	float angleStart;
	float randAngleStart;
};

class ETHParticleManager
{
public:
	/// Load a particle system from a file in memory
	ETHParticleManager(ETHResourceProviderPtr provider, const str_type::string& file, const Vector2 &v2Pos,
					   const Vector3 &v3Pos, const float angle, const float entityVolume);

	ETHParticleManager(ETHResourceProviderPtr provider, const ETH_PARTICLE_SYSTEM &partSystem, const Vector2 &v2Pos,
					   const Vector3 &v3Pos, const float angle, const float entityVolume);

	/// Update the position, size and angle of all particles in the system (if they are active)
	/// Must be called once every frame (only once). The new particles are positioned according
	/// to v2Pos and it's starting position
	bool UpdateParticleSystem(const Vector2 &v2Pos, const Vector3 &v3Pos, const float angle, const unsigned long lastFrameElapsedTime);

	/// Draw all particles also considering it's ambient light color
	bool DrawParticleSystem(Vector3 v3Ambient, const float maxHeight, const float minHeight,
							const ETH_ENTITY_TYPE ownerType, const Vector2 &zAxisDirection,
							const Vector2 &parallaxOffset, const float ownerDepth);

	/// Return true if the particle system has finished it's execution
	/// A particle system is finished when its repeat count reaches the end
	bool Finished() const;

	/// Restart the system execution by setting all particles repeat count to zero
	bool Play(const Vector2 &v2Pos, const Vector3 &v3Pos, const float angle);

	/// Set a sound effect
	void SetSoundEffect(AudioSamplePtr pSound);

	/// Get a sound effect handle
	AudioSamplePtr GetSoundEffect();

	/// Return true if it has a SFX
	bool HasSoundEffect() const;

	/// Set another system configuration (it can be used during the animation)
	void SetSystem(const ETH_PARTICLE_SYSTEM &partSystem);

	/// Set another particle bitmap  (it can be used during the animation)
	void SetParticleBitmap(SpritePtr pBMP);

	/// Return a handle to the current particle bitmap
	SpritePtr GetParticleBitmap();

	/// Kill the particle system by avoiding new particles to be launched
	void Kill(const bool kill);

	/// Return true if the system was killed by Kill(bool) function
	inline bool Killed() const
	{
		return m_killed;
	}

	/// Set the particles starting position
	void SetStartPos(const Vector3 v3Pos);

	/// Return the particles starting position
	Vector3 GetStartPos() const;

	/// Return the bounding sphere radius
	float GetBoundingRadius() const;

	/// Return all the data from the system
	const ETH_PARTICLE_SYSTEM *GetSystem() const;
	
	/// Return particle bitmap name
	str_type::string GetBitmapName() const;
	
	/// Return particle sound FX name (if there's one)
	str_type::string GetSoundName() const;

	/// Set a tilebased Z position for the tile
	void SetTileZ(const float z);
	float GetTileZ() const;

	/// Return the maximum number of particles
	int GetNumParticles() const;

	/// Return the number of active particles
	int GetNumActiveParticles() const;

	/// Return true if this sound effect loops
	bool IsSoundLooping() const;

	/// If true, forces the sound effect to be stopped
	void StopSFX(const bool stopped);

	/// Set an arbitrary sound volume for this one
	void SetSoundVolume(const float volume);

	/// Return the sound volume
	float GetSoundVolume() const;

	/// Return true if the particle system is endless (nRepeats == 0)
	bool IsEndless() const;

	/// Scale the particle system
	void ScaleParticleSystem(const float scale);

	/// Mirror the entire system along the X-axis
	void MirrorX(const bool mirrorGravity);

	/// Mirror the entire system along the Y-axis
	void MirrorY(const bool mirrorGravity);

private:
	ETH_PARTICLE_SYSTEM m_system;
	std::vector<ETH_PARTICLE> m_particles;
	ETHResourceProviderPtr m_provider;
	SpritePtr m_pBMP;
	AudioSamplePtr m_pSound;
	bool m_finished, m_killed;
	int m_nActiveParticles;
	Vector2 m_v2Move;
	float m_soundVolume;
	bool m_isSoundLooping;
	bool m_isSoundStopped;
	float m_entityVolume, m_generalVolume;

	static void BubbleSort(std::vector<ETH_PARTICLE> &v);
	void HandleSoundPlayback(const Vector2 &v2Pos, const float frameSpeed);

	/// Create a particle system
	bool CreateParticleSystem(const ETH_PARTICLE_SYSTEM &partSystem, const Vector2 &v2Pos,
							  const Vector3 &v3Pos, const float angle, const float entityVolume);

	inline void ResetParticle(const int t, const Vector2 &v2Pos, const Vector3 &v3Pos, const float angle, const Matrix4x4 &rotMatrix)
	{
		m_particles[t].angleDir = m_system.angleDir + Randomizer::Float(-m_system.randAngle/2, m_system.randAngle/2);
		m_particles[t].elapsed = 0.0f;
		m_particles[t].lifeTime = m_system.lifeTime + Randomizer::Float(-m_system.randomizeLifeTime/2, m_system.randomizeLifeTime/2);
		m_particles[t].size = m_system.size + Randomizer::Float(-m_system.randomizeSize/2, m_system.randomizeSize/2);
		m_particles[t].v2Dir.x = (m_system.v2DirectionVector.x + Randomizer::Float(-m_system.v2RandomizeDir.x/2, m_system.v2RandomizeDir.x/2));
		m_particles[t].v2Dir.y = (m_system.v2DirectionVector.y + Randomizer::Float(-m_system.v2RandomizeDir.y/2, m_system.v2RandomizeDir.y/2));
		m_particles[t].v2Dir = Multiply(m_particles[t].v2Dir, rotMatrix);
		m_particles[t].v4Color = m_system.v4Color0;
		PositionParticle(t, v2Pos, angle, rotMatrix, v3Pos);

		// setup sprite frame
		if (m_system.v2SpriteCut.x > 1 || m_system.v2SpriteCut.y > 1)
		{
			if (m_system.animationMode == _ETH_PLAY_ANIMATION)
			{
				m_particles[t].currentFrame = 0;
			} else
			if (m_system.animationMode == _ETH_PICK_RANDOM_FRAME)
			{
				m_particles[t].currentFrame = Randomizer::Int(m_system.v2SpriteCut.x*m_system.v2SpriteCut.y-1);
			}
		}
	}

	inline void PositionParticle(const int t, const Vector2 &v2Pos, const float angle, const Matrix4x4 &rotMatrix, const Vector3 &v3Pos)
	{
		m_particles[t].angle = m_system.angleStart+Randomizer::Float(m_system.randAngleStart)+angle;
		m_particles[t].v2Pos.x = m_system.v3StartPoint.x + Randomizer::Float(-m_system.v2RandStartPoint.x/2, m_system.v2RandStartPoint.x/2);
		m_particles[t].v2Pos.y = m_system.v3StartPoint.y + Randomizer::Float(-m_system.v2RandStartPoint.y/2, m_system.v2RandStartPoint.y/2);
		m_particles[t].v2Pos = Multiply(m_particles[t].v2Pos, rotMatrix);	
		m_particles[t].v2Pos = m_particles[t].v2Pos + v2Pos;
		m_particles[t].v3StartPoint = Vector3(v2Pos, v3Pos.z)+m_system.v3StartPoint;
	}

	inline void SetParticleDepth(const float depth)
	{
		m_provider->GetVideo()->SetSpriteDepth(depth);
	}
};

typedef boost::shared_ptr<ETHParticleManager> ETHParticleManagerPtr;

#endif
