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

#ifndef ETH_PARTICLE_MANAGER_H_
#define ETH_PARTICLE_MANAGER_H_

#include "ETHParticleSystem.h"

#include <Audio.h>

#define _ETH_PARTICLE_DEPTH_SHIFT (10.0f)

class ETHParticleManager
{
public:
	enum DEPTH_SORTING_MODE
	{
		INDIVIDUAL_OFFSET = 0,
		LAYERABLE = 1,
		SAME_DEPTH_AS_OWNER
	};

	/// Load a particle system from a file in memory
	ETHParticleManager(
		ETHResourceProviderPtr provider,
		const str_type::string& file,
		const Vector2& v2Pos,
		const Vector3& v3Pos,
		const float angle);

	ETHParticleManager(
		ETHResourceProviderPtr provider,
		const ETHParticleSystem& partSystem,
		const Vector2& v2Pos,
		const Vector3& v3Pos,
		const float angle,
		const float scale);

	/// Update the position, size and angle of all particles in the system (if they are active)
	/// Must be called once every frame (only once). The new particles are positioned according
	/// to v2Pos and it's starting position
	void UpdateParticleSystem(
		const Vector2& v2Pos,
		const Vector3& v3Pos,
		const float angle,
		const float lastFrameElapsedTime);

	void UpdateParticle(
		const int t,
		const bool hasJustBeenReset,
		const Vector2& v2Pos,
		const Vector3& v3Pos,
		const float angle,
		const float lastFrameElapsedTime,
		const Matrix4x4& rot,
		bool& anythingDrawn);

	/// Draw all particles also considering it's ambient light color
	bool DrawParticleSystem(
		Vector3 v3Ambient,
		const float maxHeight,
		const float minHeight,
		const DEPTH_SORTING_MODE ownerType,
		const Vector2& zAxisDirection,
		const Vector2& parallaxOffset,
		const float ownerDepth);

	/// Return true if the particle system has finished it's execution
	/// A particle system is finished when its repeat count reaches the end
	bool Finished() const;

	/// Restart the system execution by setting all particles repeat count to zero
	bool Play(const Vector2 &v2Pos, const Vector3 &v3Pos, const float angle);

	/// Set another system configuration (it can be used during the animation)
	void SetSystem(const ETHParticleSystem &partSystem);

	/// Set another particle bitmap  (it can be used during the animation)
	void SetParticleBitmap(SpritePtr pBMP);
	void SetParticleBitmap(const gs2d::str_type::string& bitmap);

	/// Return a handle to the current particle bitmap
	SpritePtr GetParticleBitmap();

	/// Kill the particle system by avoiding new particles to be launched
	void Kill(const bool kill);

	/// Return true if the system was killed by Kill(bool) function
	bool Killed() const;

	/// Set the particles starting position
	void SetStartPos(const Vector3& v3Pos);

	/// Return the particles starting position
	Vector3 GetStartPos() const;

	/// Return the bounding sphere radius
	float GetBoundingRadius() const;

	/// Return all the data from the system
	const ETHParticleSystem *GetSystem() const;
	
	/// Return particle bitmap name
	str_type::string GetBitmapName() const;

	/// Set a tilebased Z position for the tile
	void SetZPosition(const float z);
	float GetZPosition() const;

	/// Return the maximum number of particles
	int GetNumParticles() const;

	/// Return the number of active particles
	int GetNumActiveParticles() const;

	/// Return true if the particle system is endless (nRepeats == 0)
	bool IsEndless() const;

	/// Scale the particle system
	void ScaleParticleSystem(const float scale);

	/// Mirror the entire system along the X-axis
	void MirrorX(const bool mirrorGravity);

	/// Mirror the entire system along the Y-axis
	void MirrorY(const bool mirrorGravity);

private:

	struct PARTICLE
	{
		PARTICLE() :
			released(false),
			repeat(0),
			id(-1),
			currentFrame(0),
			elapsed(0.0f)
		{
		}

		inline bool operator < (const PARTICLE& other) const
		{
			return (GetOffset() < other.GetOffset());
		}

		inline float GetOffset() const
		{
			return startPoint.y - pos.y;
		}

		inline void Scale(const float& scale)
		{
			size *= scale;
			dir *= scale;
		}

		Vector2 pos;
		Vector2 dir;
		Vector4 color;
		Vector3 startPoint;
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

	ETHParticleSystem m_system;
	std::vector<PARTICLE> m_particles;
	ETHResourceProviderPtr m_provider;
	SpritePtr m_pBMP;
	bool m_finished, m_killed;
	int m_nActiveParticles;
	Vector2 m_v2Move;

	static void Sort(std::vector<PARTICLE> &v);

	/// Create a particle system
	bool CreateParticleSystem(
		const ETHParticleSystem& partSystem,
		const Vector2& v2Pos,
		const Vector3& v3Pos,
		const float angle,
		const float scale);

	void ResetParticle(const int t, const Vector2& v2Pos, const Vector3& v3Pos, const float angle, const Matrix4x4& rotMatrix);
	void PositionParticle(const int t, const Vector2& v2Pos, const float angle, const Matrix4x4& rotMatrix, const Vector3& v3Pos);
	void SetParticleDepth(const float depth);

public:
	static float ComputeParticleDepth(
		const DEPTH_SORTING_MODE& ownerType,
		const float& ownerDepth,
		const PARTICLE& particle,
		const float& maxHeight,
		const float& minHeight);

	static float GetParticleDepthShift(const DEPTH_SORTING_MODE& ownerType);
};

typedef boost::shared_ptr<ETHParticleManager> ETHParticleManagerPtr;

#endif
