#ifndef ETH_SPRITE_ENTITY_H_
#define ETH_SPRITE_ENTITY_H_

#include "ETHEntity.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Scene/ETHSceneProperties.h"

class ETHSpriteEntity : public ETHEntity
{
public:
	ETHSpriteEntity(const str_type::string& filePath, ETHResourceProviderPtr provider, const int nId =-1);
	ETHSpriteEntity(
		TiXmlElement *pElement,
		ETHResourceProviderPtr provider,
		ETHEntityCache& entityCache,
		const str_type::string& entityPath,
		const bool shouldGenerateNewID);
	ETHSpriteEntity(
		ETHResourceProviderPtr provider,
		const ETHEntityProperties& properties,
		const float angle,
		const float scale);
	ETHSpriteEntity(ETHResourceProviderPtr provider);

	void Refresh(const ETHEntityProperties& properties);

	void AddRef();
	void Release();

	bool SetDepth(const float maxHeight, const float minHeight);
	void SetOrigin();

	Rect2D GetFrameRect() const;

	SpritePtr GetSprite();
	SpritePtr GetGloss();
	SpritePtr GetNormal();
	SpritePtr GetLightmap();
	SpritePtr GetHalo();
	SpritePtr GetParticleBMP(const unsigned int n);

	str_type::string AssembleLightmapFileName(const str_type::string& directory, const str_type::string& extension) const;
	bool SaveLightmapToFile(const str_type::string& directory);

	float GetMaxHeight();
	float GetMinHeight();

	bool LoadLightmapFromFile(const str_type::string& filePath);

	void DestroyParticleSystem(const unsigned int n);
	void SetParticleBitmap(const int unsigned n, SpritePtr bitmap);
	void SetParticleBitmap(const unsigned int n, const str_type::string& bitmap);
	void SetParticlePosition(const unsigned int n, const Vector3 &v3Pos);
	void ScaleParticleSystem(const unsigned int n, const float scale);
	bool MirrorParticleSystemX(const unsigned int n, const bool mirrorGravity);
	bool MirrorParticleSystemY(const unsigned int n, const bool mirrorGravity);
	void SetParticleColorA(const unsigned int n, const Vector3 &color, const float alpha);
	void SetParticleColorB(const unsigned int n, const Vector3 &color, const float alpha);
	bool HasParticleSystem(const unsigned int n) const;

	ETHParticleManagerPtr GetParticleManager(const std::size_t n);
	ETHParticleManagerConstPtr GetConstParticleManager(const std::size_t n) const;
	bool AreParticlesOver() const;
	bool PlayParticleSystem(const unsigned int n, const Vector2& zAxisDirection);
	void KillParticleSystem(const unsigned int n);
	bool ParticlesKilled(const unsigned int n) const;
	void SetSoundVolume(const float volume);
	bool SetSpriteCut(const unsigned int col, const unsigned int row);

	void ValidateSpriteCut(const SpritePtr& sprite) const;
	Vector2 GetSize() const;
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

	str_type::string GetSpriteName() const;
	str_type::string GetNormalName() const;
	str_type::string GetGlossName() const;
	str_type::string GetHaloName() const;

	void Update(const float lastFrameElapsedTime, const Vector2& zAxisDir, ETHBucketManager& buckets);
	void UpdateParticleSystems(const Vector2& zAxisDirection, const float lastFrameElapsedTime);

	float ComputeLightIntensity();

	ETHPhysicsController* GetPhysicsController();

	void LoadParticleSystem();

	void RecoverResources(const Platform::FileManagerPtr& expansionFileManager);
	void ReleaseLightmap();

	bool ShouldUseHighlightPixelShader() const;
	bool ShouldUseSolidColorPixelShader() const;
	bool ShouldUsePass1AddPixelShader() const;

	Vector2 GetSpriteCut() const;

protected:
	std::vector<ETHParticleManagerPtr> m_particles;

	ETHResourceProviderPtr m_provider;
	mutable SpritePtr m_pSprite;
	SpritePtr m_pGloss;
	SpritePtr m_pNormal;
	SpritePtr m_pHalo;
	SpritePtr m_pLightmap;
	str_type::string m_preRenderedLightmapFilePath;

	Sprite::RectsPtr m_packedFrames;

	static const float m_layrableMinimumDepth;

private:
	void Create();
	void Zero();
};

#endif
