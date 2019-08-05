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
	mutable int m_ref;

public:
	virtual ETHEntityProperties::ENTITY_TYPE GetType() const = 0;
	virtual str_type::string GetEntityName() const = 0;
	virtual Vector3 GetPosition() const = 0;
	virtual Vector2 GetPositionXY() const = 0;
	virtual float GetAngle() const = 0;
	virtual void SetAngle(const float angle) = 0;
	virtual void SetPosition(const Vector3& pos, ETHBucketManager& buckets) = 0;
	virtual void SetPositionXY(const Vector2& pos, ETHBucketManager& buckets) = 0;
	virtual bool SetFrame(const unsigned int frame) = 0;
	virtual bool SetFrame(const unsigned int column, const unsigned int row) = 0;
	virtual unsigned int GetFrame() const = 0;
	virtual Vector2 GetSize() const = 0;
	virtual bool HasLightSource() const = 0;
	virtual bool HasHalo() const = 0;
	virtual bool IsCollidable() const = 0;
	virtual unsigned int GetNumFrames() const = 0;
	virtual bool IsStatic() const = 0;
	virtual bool IsTemporary() const = 0;
	virtual bool IsInvisible() const = 0;
	virtual void Hide(const bool hide) = 0;
	virtual bool IsHidden() const = 0;
	virtual ETHEntityProperties::VIEW_RECT GetScreenRect(const ETHSceneProperties& sceneProps) const = 0;
	virtual Vector2 GetScreenRectMin(const ETHSceneProperties& sceneProps) const = 0;
	virtual Vector2 GetScreenRectMax(const ETHSceneProperties& sceneProps) const = 0;
	virtual int GetID() const = 0;
	virtual void TurnDynamic() = 0;
	virtual void TurnStatic() = 0;
	virtual void KillParticleSystem(const unsigned int n) = 0;
	virtual bool ParticlesKilled(const unsigned int n) const = 0;
	virtual bool PlayParticleSystem(const unsigned int n, const Vector2& zAxisDirection) = 0;
	virtual bool AreParticlesOver() const = 0;
	virtual ETHCollisionBox GetCollisionBox() const = 0;
	virtual bool HasParticleSystems() const = 0;
	virtual bool HasParticleSystem(const unsigned int n) const = 0;
	virtual void AddToPosition(const Vector3& pos, ETHBucketManager& buckets) = 0;
	virtual void AddToPositionXY(const Vector2& pos, ETHBucketManager& buckets) = 0;
	virtual void AddToAngle(const float angle) = 0;
	virtual bool SetSpriteCut(const unsigned int columns, const unsigned int rows) = 0;
	virtual void SetParticlePosition(const unsigned int n, const Vector3 &v3Pos) = 0;
	virtual void SetParticleBitmap(const unsigned int n, const str_type::string& bitmap) = 0;
	virtual void ScaleParticleSystem(const unsigned int n, const float scale) = 0;
	virtual bool MirrorParticleSystemX(const unsigned int n, const bool mirrorGravity) = 0;
	virtual bool MirrorParticleSystemY(const unsigned int n, const bool mirrorGravity) = 0;
	virtual void SetParticleColorA(const unsigned int n, const Vector3 &color, const float alpha) = 0;
	virtual void SetParticleColorB(const unsigned int n, const Vector3 &color, const float alpha) = 0;
	virtual Vector2 ComputeParallaxOffset() const = 0;
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

	virtual bool SetSprite(const str_type::string &fileName) = 0;
	virtual bool SetHalo(const str_type::string &fileName) = 0;

	virtual str_type::string GetSpriteName() const = 0;
	virtual str_type::string GetHaloName() const = 0;

	virtual void SetFloat(const str_type::string &name, const float &value) = 0;
	virtual void SetInt(const str_type::string &name, const int &value) = 0;
	virtual void SetUInt(const str_type::string &name, const unsigned int &value) = 0;
	virtual void SetString(const str_type::string &name, const str_type::string &value) = 0;
	virtual void SetVector2(const str_type::string &name, const Vector2 &value) = 0;
	virtual void SetVector3(const str_type::string &name, const Vector3 &value) = 0;

	virtual float GetFloat(const str_type::string &name, const float defaultValue) const = 0;
	virtual int GetInt(const str_type::string &name, const int defaultValue) const = 0;
	virtual unsigned int GetUInt(const str_type::string &name, const unsigned int defaultValue) const = 0;
	virtual str_type::string GetString(const str_type::string &name, const str_type::string& defaultValue) const = 0;
	virtual Vector2 GetVector2(const str_type::string &name, const Vector2& defaultValue) const = 0;
	virtual Vector3 GetVector3(const str_type::string &name, const Vector3& defaultValue) const = 0;

	virtual float GetFloat(const str_type::string &name) const = 0;
	virtual int GetInt(const str_type::string &name) const = 0;
	virtual unsigned int GetUInt(const str_type::string &name) const = 0;
	virtual str_type::string GetString(const str_type::string &name) const = 0;
	virtual Vector2 GetVector2(const str_type::string &name) const = 0;
	virtual Vector3 GetVector3(const str_type::string &name) const = 0;

	virtual void SetAngelScriptObject(const str_type::string &name, void *value, int typeId) = 0;
	virtual bool GetAngelScriptObject(const str_type::string &name, void *value, int typeId) = 0;

	virtual float AddToFloat(const str_type::string &name, const float &value) = 0;
	virtual int AddToInt(const str_type::string &name, const int &value) = 0;
	virtual unsigned int AddToUInt(const str_type::string &name, const unsigned int &value) = 0;
	virtual Vector2 AddToVector2(const str_type::string &name, const Vector2 &v) = 0;
	virtual Vector3 AddToVector3(const str_type::string &name, const Vector3 &v) = 0;

	virtual float MultiplyFloat(const str_type::string &name, const float &value) = 0;
	virtual int MultiplyInt(const str_type::string &name, const int &value) = 0;
	virtual unsigned int MultiplyUInt(const str_type::string &name, const unsigned int &value) = 0;
	virtual Vector2 MultiplyVector2(const str_type::string &name, const float &value) = 0;
	virtual Vector3 MultiplyVector3(const str_type::string &name, const float &value) = 0;

	virtual bool EraseData(const str_type::string &name) = 0;
	virtual ETHCustomData::DATA_TYPE CheckCustomData(const str_type::string &name) const = 0;
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
