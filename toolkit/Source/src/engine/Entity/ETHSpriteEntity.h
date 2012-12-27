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

#ifndef ETH_SPRITE_ENTITY_H_
#define ETH_SPRITE_ENTITY_H_

#include "ETHEntity.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Scene/ETHSceneProperties.h"

class ETHSpriteEntity : public ETHEntity
{
public:
	ETHSpriteEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId =-1);
	ETHSpriteEntity(TiXmlElement *pElement, ETHResourceProviderPtr provider);
	ETHSpriteEntity(ETHResourceProviderPtr provider, const ETHEntityProperties& properties, const float angle, const float scale);
	ETHSpriteEntity(ETHResourceProviderPtr provider);

	void Refresh(const ETHEntityProperties& properties);

	void AddRef();
	void Release();

	bool SetDepth(const float maxHeight, const float minHeight);
	void SetOrigin();

	SpritePtr GetSprite();
	SpritePtr GetGloss();
	SpritePtr GetNormal();
	SpritePtr GetLightmap();
	SpritePtr GetHalo();
	SpritePtr GetParticleBMP(const unsigned int n);
	AudioSamplePtr GetParticleSFX(const unsigned int n);

	float GetMaxHeight();
	float GetMinHeight();

	void DestroyParticleSystem(const unsigned int n);
	void SetParticleBitmap(const int unsigned n, SpritePtr bitmap);
	void SetParticleBitmap(const unsigned int n, const str_type::string& bitmap);
	void SetParticleSFX(const int unsigned n, AudioSamplePtr pSample);
	void SetParticlePosition(const unsigned int n, const Vector3 &v3Pos);
	void ScaleParticleSystem(const unsigned int n, const float scale);
	bool MirrorParticleSystemX(const unsigned int n, const bool mirrorGravity);
	bool MirrorParticleSystemY(const unsigned int n, const bool mirrorGravity);
	bool HasParticleSystem(const unsigned int n) const;

	ETHParticleManagerPtr GetParticleManager(const std::size_t n);
	bool AreParticlesOver() const;
	void SetStopSFXWhenDestroyed(const bool enable);
	void SilenceParticleSystems(const bool silence);
	bool PlayParticleSystem(const unsigned int n, const Vector2& zAxisDirection);
	void KillParticleSystem(const unsigned int n);
	bool ParticlesKilled(const unsigned int n) const;
	void SetSoundVolume(const float volume);
	bool SetSpriteCut(const unsigned int col, const unsigned int row);

	void ValidateSpriteCut(const SpritePtr& sprite) const;
	Vector2 GetCurrentSize() const;
	Vector2 ComputeParallaxOffset() const;
	float ComputeDepth(const float maxHeight, const float minHeight) const;
	void SetScale(const Vector2& scale);

	ETHEntityProperties::VIEW_RECT GetScreenRect(const ETHSceneProperties& sceneProps) const;
	Vector2 GetScreenRectMin(const ETHSceneProperties& sceneProps) const;
	Vector2 GetScreenRectMax(const ETHSceneProperties& sceneProps) const;
	Vector2 ComputeInScreenPosition(const ETHSceneProperties& sceneProps) const;
	Vector2 ComputePositionWithZAxisApplied(const ETHSceneProperties& sceneProps) const;
	Vector2 ComputeInScreenSpriteCenter(const ETHSceneProperties& sceneProps) const;
	bool IsPointOnSprite(const ETHSceneProperties& sceneProps, const Vector2& absolutePointPos, const Vector2& size) const;

	bool SetSprite(const str_type::string &fileName);
	bool SetNormal(const str_type::string &fileName);
	bool SetGloss(const str_type::string &fileName);
	bool SetHalo(const str_type::string &fileName);

	void Update(const unsigned long lastFrameElapsedTime, const Vector2& zAxisDir, ETHBucketManager& buckets);
	void UpdateParticleSystems(const Vector2& zAxisDirection, const unsigned long lastFrameElapsedTime);

	float ComputeLightIntensity();

	ETHPhysicsController* GetPhysicsController();

	void ForceSFXStop();
	void StartSFX();

	void LoadParticleSystem();

	void RecoverResources();
	void ReleaseLightmap();

	Vector2 GetSpriteCut() const;

protected:
	std::vector<ETHParticleManagerPtr> m_particles;

	bool m_stopSFXWhenDestroyed;
	ETHResourceProviderPtr m_provider;
	mutable SpritePtr m_pSprite;
	SpritePtr m_pGloss;
	SpritePtr m_pNormal;
	SpritePtr m_pHalo;
	SpritePtr m_pLightmap;

	static const float m_layrableMinimumDepth;

private:
	void Create();
	void Zero();
};

#endif
