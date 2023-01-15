#ifndef ETH_SPRITE_ENTITY_H_
#define ETH_SPRITE_ENTITY_H_

#include "ETHEntity.h"
#include "../Resource/ETHResourceProvider.h"
#include "../Scene/ETHSceneProperties.h"

class ETHSpriteEntity : public ETHEntity
{
public:
	ETHSpriteEntity(const std::string& filePath, ETHResourceProviderPtr provider, const int nId =-1);
	ETHSpriteEntity(
		TiXmlElement *pElement,
		ETHResourceProviderPtr provider,
		ETHEntityCache& entityCache,
		const std::string& entityPath,
		const bool shouldGenerateNewID);
	ETHSpriteEntity(
		ETHResourceProviderPtr provider,
		const ETHEntityProperties& properties,
		const float angle,
		const float scale);
	ETHSpriteEntity(ETHResourceProviderPtr provider);

	void AddRef() override;
	void Release() override;

	bool SetDepth(const float maxHeight, const float minHeight);
	void SetOrigin();

	Rect2D GetFrameRect() const override;

	SpritePtr GetSprite();
	SpritePtr GetLightmap();
	SpritePtr GetHalo();
	SpritePtr GetParticleBMP(const unsigned int n);

	std::string AssembleLightmapFileName(const std::string& directory, const std::string& extension) const;

	float GetMaxHeight();
	float GetMinHeight();

	bool LoadLightmapFromFile(const std::string& filePath);

	void DestroyParticleSystem(const unsigned int n);
	void SetParticleBitmap(const int unsigned n, SpritePtr bitmap);
	void SetParticleBitmap(const unsigned int n, const std::string& bitmap) override;
	void SetParticlePosition(const unsigned int n, const Vector3 &v3Pos) override;
	void ScaleParticleSystem(const unsigned int n, const float scale) override;
	bool MirrorParticleSystemX(const unsigned int n, const bool mirrorGravity) override;
	bool MirrorParticleSystemY(const unsigned int n, const bool mirrorGravity) override;
	void SetParticleColorA(const unsigned int n, const Vector3 &color, const float alpha) override;
	void SetParticleColorB(const unsigned int n, const Vector3 &color, const float alpha) override;
	bool HasParticleSystem(const unsigned int n) const override;

	ETHParticleManagerPtr GetParticleManager(const std::size_t n);
	ETHParticleManagerConstPtr GetConstParticleManager(const std::size_t n) const;
	bool AreParticlesOver() const override;
	bool PlayParticleSystem(const unsigned int n, const Vector2& zAxisDirection) override;
	void KillParticleSystem(const unsigned int n) override;
	bool ParticlesKilled(const unsigned int n) const override;
	void SetSoundVolume(const float volume);
	bool SetSpriteCut(const unsigned int col, const unsigned int row) override;

	Vector2 GetSize() const override;
	Vector2 ComputeParallaxOffset(const float sceneParallaxIntensity) const override;
	float ComputeDepth(const float maxHeight, const float minHeight) const;
	void SetScale(const Vector2& scale) override;

	ETHEntityProperties::VIEW_RECT GetScreenRect(const ETHSceneProperties& sceneProps) const override;
	Vector2 GetScreenRectMin(const ETHSceneProperties& sceneProps) const override;
	Vector2 GetScreenRectMax(const ETHSceneProperties& sceneProps) const override;
	Vector2 ComputeInScreenPosition(const ETHSceneProperties& sceneProps) const;
	Vector2 ComputePositionWithZAxisApplied(const ETHSceneProperties& sceneProps) const;
	Vector2 ComputeInScreenSpriteCenter(const ETHSceneProperties& sceneProps) const;
	bool IsPointOnSprite(const ETHSceneProperties& sceneProps, const Vector2& absolutePointPos, const Vector2& size) const;

	bool SetSprite(const std::string &fileName) override;
	bool SetHalo(const std::string &fileName) override;

	std::string GetSpriteName() const override;
	std::string GetHaloName() const override;

	void Update(const float lastFrameElapsedTime, const Vector2& zAxisDir, const float sceneParallaxIntensity, ETHBucketManager& buckets);
	void UpdateParticleSystems(const Vector2& zAxisDirection, const float sceneParallaxIntensity, const float lastFrameElapsedTime);

	float ComputeLightIntensity();

	ETHPhysicsController* GetPhysicsController() override;

	void CreateParticleSystem();

	unsigned int GetNumFrames() const override;
	unsigned int GetFrame() const override;
	bool SetFrame(const unsigned int frame) override;
	bool SetFrame(const unsigned int column, const unsigned int row) override;
	Vector2 ComputeOrigin(const Vector2 &v2Size) const override;
	Vector2 ComputeAbsoluteOrigin(const Vector2 &v2Size) const override;

	void RecoverResources(const Platform::FileManagerPtr& expansionFileManager) override;
	void ReleaseLightmap() override;

	bool ShouldUseHighlightPixelShader() const;
	bool ShouldUseSolidColorPixelShader() const;
	bool ShouldUsePass1AddPixelShader() const;

	Vector2 GetSpriteCut() const override;

	std::size_t GetNumParticleSystems() const override;
	
	void LoadResources();
	
protected:
	std::vector<ETHParticleManagerPtr> m_particles;

	ETHResourceProviderPtr m_provider;
	mutable SpritePtr m_pSprite;
	SpritePtr m_pHalo;
	SpritePtr m_pLightmap;
	std::string m_preRenderedLightmapFilePath;

	SpriteRectsPtr m_packedFrames;

	static const float m_layrableMinimumDepth;

private:
	void Create();
	void Zero();
};

#endif
