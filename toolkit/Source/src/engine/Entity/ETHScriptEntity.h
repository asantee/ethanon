#ifndef ETH_SCRIPT_ENTITY_H_
#define ETH_SCRIPT_ENTITY_H_

#include "../Physics/ETHCollisionBox.h"

#include "../Scene/ETHSceneProperties.h"
#include "../Scene/ETHBucketManager.h"

#include "ETHCustomDataManager.h"

#include "../Entity/ETHEntityProperties.h"

#include "../../angelscript/include/angelscript.h"

class ETHPhysicsController;

class ETHScriptEntity
{
	bool m_isAlive;

protected:
	ETHScriptEntity();
	mutable int32_t m_ref;

public:
	virtual ETHEntityProperties::ENTITY_TYPE GetType() const = 0;
	virtual std::string GetEntityName() const = 0;
	virtual Vector3 GetPosition() const = 0;
	virtual Vector2 GetPositionXY() const = 0;
	virtual float GetAngle() const = 0;
	virtual void SetAngle(const float angle) = 0;
	virtual void SetPosition(const Vector3& pos, ETHBucketManager& buckets) = 0;
	virtual void SetPositionXY(const Vector2& pos, ETHBucketManager& buckets) = 0;
	virtual bool SetFrame(const uint32_t frame) = 0;
	virtual bool SetFrame(const uint32_t column, const uint32_t row) = 0;
	virtual uint32_t GetFrame() const = 0;
	virtual Vector2 GetSize() const = 0;
	virtual bool HasLightSource() const = 0;
	virtual bool HasHalo() const = 0;
	virtual bool IsCollidable() const = 0;
	virtual uint32_t GetNumFrames() const = 0;
	virtual bool IsStatic() const = 0;
	virtual bool IsTemporary() const = 0;
	virtual bool IsInvisible() const = 0;
	virtual void Hide(const bool hide) = 0;
	virtual bool IsHidden() const = 0;
	virtual ETHEntityProperties::VIEW_RECT GetScreenRect(const ETHSceneProperties& sceneProps) const = 0;
	virtual Vector2 GetScreenRectMin(const ETHSceneProperties& sceneProps) const = 0;
	virtual Vector2 GetScreenRectMax(const ETHSceneProperties& sceneProps) const = 0;
	virtual int32_t GetID() const = 0;
	virtual void TurnDynamic() = 0;
	virtual void TurnStatic() = 0;
	virtual void KillParticleSystem(const uint32_t n) = 0;
	virtual bool ParticlesKilled(const uint32_t n) const = 0;
	virtual bool PlayParticleSystem(const uint32_t n, const Vector2& zAxisDirection) = 0;
	virtual bool AreParticlesOver() const = 0;
	virtual ETHCollisionBox GetCollisionBox() const = 0;
	virtual bool HasParticleSystems() const = 0;
	virtual bool HasParticleSystem(const uint32_t n) const = 0;
	virtual void AddToPosition(const Vector3& pos, ETHBucketManager& buckets) = 0;
	virtual void AddToPositionXY(const Vector2& pos, ETHBucketManager& buckets) = 0;
	virtual void AddToAngle(const float angle) = 0;
	virtual bool SetSpriteCut(const uint32_t columns, const uint32_t rows) = 0;
	virtual void SetParticlePosition(const uint32_t n, const Vector3 &v3Pos) = 0;
	virtual void SetParticleBitmap(const uint32_t n, const std::string& bitmap) = 0;
	virtual void ScaleParticleSystem(const uint32_t n, const float scale) = 0;
	virtual bool MirrorParticleSystemX(const uint32_t n, const bool mirrorGravity) = 0;
	virtual bool MirrorParticleSystemY(const uint32_t n, const bool mirrorGravity) = 0;
	virtual void SetParticleColorA(const uint32_t n, const Vector3 &color, const float alpha) = 0;
	virtual void SetParticleColorB(const uint32_t n, const Vector3 &color, const float alpha) = 0;
	virtual Vector2 ComputeParallaxOffset(const float sceneParallaxIntensity) const = 0;
	virtual float GetParallaxIntensity() const = 0;
	virtual void SetParallaxIntensity(const float individualIntensity) = 0;

	virtual void SetLightPosition(const Vector3& pos) = 0;
	virtual Vector3 GetLightPosition() const = 0;
	virtual void SetLightRange(const float range) = 0;
	virtual float GetLightRange() const = 0;
	virtual void SetLightColor(const Vector3 &color) = 0;
	virtual Vector3 GetLightColor() const = 0;
	virtual void SetEmissiveColor(const Vector3 &color) = 0;
	virtual Vector3 GetEmissiveColor() const = 0;

	virtual void SetColor(const Vector3 &color) = 0;
	virtual void SetAlpha(const float alpha) = 0;
	virtual Vector3 GetColor() const = 0;
	virtual Vector4 GetColorARGB() const = 0;
	virtual float GetAlpha() const = 0;

	virtual Vector3 GetSolidColor() const = 0;
	virtual void SetSolidColor(const Vector3& color) = 0;
	virtual void SetSolidColorAlpha(const float alpha) = 0;
	virtual float GetSolidColorAlpha() const = 0;
	virtual Vector4 GetSolidColorARGB() const = 0;

	virtual void SetLayerDepth(const float depth) = 0;
	virtual float GetLayerDepth() const = 0;
	virtual void SetPivotAdjust(const Vector2& p) = 0;
	virtual Vector2 GetPivotAdjust() const = 0;
	virtual void RecoverResources(const Platform::FileManagerPtr& expansionFileManager) = 0;
	virtual void DisableLightSource() = 0;

	virtual void SetFlipX(const bool flipX) = 0;
	virtual void SetFlipY(const bool flipY) = 0;
	virtual bool GetFlipX() const = 0;
 	virtual bool GetFlipY() const = 0;

	virtual void Scale(const Vector2& scale) = 0;
	virtual void Scale(const float scale) = 0;
	virtual void SetScale(const Vector2& scale) = 0;
	virtual Vector2 GetScale() const = 0;

	virtual bool SetSprite(const std::string &fileName) = 0;
	virtual bool SetHalo(const std::string &fileName) = 0;

	virtual std::string GetSpriteName() const = 0;
	virtual std::string GetHaloName() const = 0;

	virtual void SetFloat(const std::string &name, const float &value) = 0;
	virtual void SetInt(const std::string &name, const int32_t &value) = 0;
	virtual void SetUInt(const std::string &name, const uint32_t &value) = 0;
	virtual void SetString(const std::string &name, const std::string &value) = 0;
	virtual void SetVector2(const std::string &name, const Vector2 &value) = 0;
	virtual void SetVector3(const std::string &name, const Vector3 &value) = 0;

	virtual float GetFloat(const std::string &name, const float defaultValue) const = 0;
	virtual int32_t GetInt(const std::string &name, const int32_t defaultValue) const = 0;
	virtual uint32_t GetUInt(const std::string &name, const uint32_t defaultValue) const = 0;
	virtual std::string GetString(const std::string &name, const std::string& defaultValue) const = 0;
	virtual Vector2 GetVector2(const std::string &name, const Vector2& defaultValue) const = 0;
	virtual Vector3 GetVector3(const std::string &name, const Vector3& defaultValue) const = 0;

	virtual float GetFloat(const std::string &name) const = 0;
	virtual int32_t GetInt(const std::string &name) const = 0;
	virtual uint32_t GetUInt(const std::string &name) const = 0;
	virtual std::string GetString(const std::string &name) const = 0;
	virtual Vector2 GetVector2(const std::string &name) const = 0;
	virtual Vector3 GetVector3(const std::string &name) const = 0;

	virtual void SetAngelScriptObject(const std::string &name, void *value, int32_t typeId) = 0;
	virtual bool GetAngelScriptObject(const std::string &name, void *value, int32_t typeId) = 0;

	virtual float AddToFloat(const std::string &name, const float &value) = 0;
	virtual int32_t AddToInt(const std::string &name, const int32_t &value) = 0;
	virtual uint32_t AddToUInt(const std::string &name, const uint32_t &value) = 0;
	virtual Vector2 AddToVector2(const std::string &name, const Vector2 &v) = 0;
	virtual Vector3 AddToVector3(const std::string &name, const Vector3 &v) = 0;

	virtual float MultiplyFloat(const std::string &name, const float &value) = 0;
	virtual int32_t MultiplyInt(const std::string &name, const int32_t &value) = 0;
	virtual uint32_t MultiplyUInt(const std::string &name, const uint32_t &value) = 0;
	virtual Vector2 MultiplyVector2(const std::string &name, const float &value) = 0;
	virtual Vector3 MultiplyVector3(const std::string &name, const float &value) = 0;

	virtual bool EraseData(const std::string &name) = 0;
	virtual ETHCustomData::DATA_TYPE CheckCustomData(const std::string &name) const = 0;
	virtual void DebugPrintCustomData() const = 0;
	virtual bool HasCustomData() const = 0;
	virtual void ClearCustomData() = 0;

	virtual ETHPhysicsController* GetPhysicsController() = 0;

	virtual bool IsFixedRotation() const = 0;
	virtual bool IsSensor() const = 0;
	virtual bool HasSimulatedBody() const = 0;

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	void Kill();
	bool IsAlive() const;

	virtual void SetPositionX(const float v, ETHBucketManager& buckets) = 0;
	virtual void SetPositionY(const float v, ETHBucketManager& buckets) = 0;
	virtual void SetPositionZ(const float v) = 0;
	virtual float GetPositionX() const = 0;
	virtual float GetPositionY() const = 0;
	virtual float GetPositionZ() const = 0;
	virtual void AddToPositionX(const float v, ETHBucketManager& buckets) = 0;
	virtual void AddToPositionY(const float v, ETHBucketManager& buckets) = 0;
	virtual void AddToPositionZ(const float v) = 0;

	virtual Vector2 GetSpriteCut() const = 0;
};

#endif
