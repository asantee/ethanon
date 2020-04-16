#ifndef ETH_ENTITY_H_
#define ETH_ENTITY_H_

#include "ETHEntityProperties.h"
#include "ETHScriptEntity.h"
#include "ETHEntityController.h"

#include "../Physics/ETHCompoundShape.h"

#include "../Entity/ETHEntityCache.h"

#include "../addons/scriptdictionary.h"

class ETHEntity : public ETHScriptEntity
{
	friend class ETHScene;
public:
	static float ComputeDepth(const float height, const float maxHeight, const float minHeight);

	ETHEntity(const std::string& filePath, const int nId, const Platform::FileManagerPtr& fileManager);
	ETHEntity(TiXmlElement *pElement,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		Platform::FileManagerPtr fileManager,
		const bool shouldGenerateNewID);
	ETHEntity();
	~ETHEntity();

	virtual void Refresh(const ETHEntityProperties& properties) = 0;

	bool WriteToXMLFile(
		TiXmlElement *pHeadRoot,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		Platform::FileManagerPtr fileManager) const;

	inline const ETHEntityProperties* GetProperties() const { return &m_properties; };
	ETHEntityControllerPtr GetController();
	void SetController(const ETHEntityControllerPtr& controller);

	void Kill();

	// General methods
	int GetID() const override;
	virtual Vector2 ComputeOrigin(const Vector2 &v2Size) const = 0;
	virtual Vector2 ComputeAbsoluteOrigin(const Vector2 &v2Size) const = 0;
	virtual Rect2D GetFrameRect() const = 0;
	Vector3 GetLightRelativePosition() const;
	ETHCollisionBox GetCollisionBox() const override;
	ETHCompoundShapePtr GetCompoundShape() const;
	void ChangeEntityName(const std::string& name);
	std::string GetEntityName() const override;
	std::size_t GetNumParticleSystems() const;
	ETHEntityProperties::ENTITY_TYPE GetType() const override;
	ETHEntityProperties::BODY_SHAPE GetShape() const;
	Vector3 GetPosition() const override;
	Vector2 GetPositionXY() const override;
	void SetPosition(const Vector3& pos, ETHBucketManager& buckets) override;
	void SetPositionXY(const Vector2& pos, ETHBucketManager& buckets) override;
	void AddToPosition(const Vector3& pos, ETHBucketManager& buckets) override;
	void AddToPositionXY(const Vector2& pos, ETHBucketManager& buckets) override;
	void AddToAngle(const float angle) override;
	void SetOrphanPosition(const Vector3& pos);
	void SetOrphanPositionXY(const Vector2& pos);
	float GetAngle() const override;
	void SetAngle(const float angle) override;
	bool IsCollidable() const override;
	Video::ALPHA_MODE GetBlendMode() const;
	bool HasAnyCallbackFunction() const;
	const ETHLight* GetLight() const;
	void TurnDynamic() override;
	void TurnStatic() override;
	void Hide(const bool hide) override;
	Vector2 GetCurrentBucket(const ETHBucketManager& buckets) const;

	void SetEmissiveColor(const Vector3 &color) override;
	Vector3 GetEmissiveColor() const override;
	void SetColor(const Vector3& color) override;
	void SetColor(const Vector4& color);
	void SetAlpha(const float alpha) override;
	Vector3 GetColor() const override;
	Vector4 GetColorARGB() const override;
	float GetAlpha() const override;
	void SetLayerDepth(const float depth) override;
	float GetLayerDepth() const override;
	float GetDensity(const unsigned int fixtureIdx) const;
	float GetFriction(const unsigned int fixtureIdx) const;
	float GetRestitution(const unsigned int fixtureIdx) const;

	Vector3 GetSolidColor() const override;
	void SetSolidColor(const Vector3& color) override;
	void SetSolidColorAlpha(const float alpha) override;
	float GetSolidColorAlpha() const override;
	Vector4 GetSolidColorARGB() const override;

	void Scale(const Vector2& scale) override;
	void Scale(const float scale) override;
	void SetScale(const Vector2& scale) override;
	Vector2 GetScale() const override;

	/// Scale the particle systems property files
	void ScaleParticleSystemOrigin(const unsigned int n, const float scale);

	bool IsRotatable() const;
	bool IsStatic() const override;
	bool IsHidden() const override;
	bool IsTemporary() const override;
	bool IsInvisible() const override;
	bool IsBody() const;

	bool HasLightSource() const override;
	bool HasParticleSystems() const override;
	bool HasHalo() const override;

	bool RunCallbackScript();

	void SetLightRange(const float range) override;
	float GetLightRange() const override;
	void SetLightColor(const Vector3 &color) override;
	Vector3 GetLightColor() const override;
	void SetLightPosition(const Vector3& pos) override;
	Vector3 GetLightPosition() const override;
	void DisableLightSource() override;

	void SetFlipX(const bool flipX) override;
	void SetFlipY(const bool flipY) override;
	bool GetFlipX() const override;
	bool GetFlipY() const override;

	// Custom data related methods:
	const ETHCustomDataManager *GetCustomDataManager() const;
	void SetFloat(const std::string &name, const float &value) override;
	void SetInt(const std::string &name, const int &value) override;
	void SetUInt(const std::string &name, const unsigned int &value) override;
	void SetString(const std::string &name, const std::string &value) override;
	void SetVector2(const std::string &name, const Vector2 &value) override;
	void SetVector3(const std::string &name, const Vector3 &value) override;
	void AddData(const std::string &name, const ETHCustomDataConstPtr &dataIn);

	float GetFloat(const std::string &name, const float defaultValue) const override;
	int GetInt(const std::string &name, const int defaultValue) const override;
	unsigned int GetUInt(const std::string &name, const unsigned int defaultValue) const override;
	std::string GetString(const std::string &name, const std::string& defaultValue) const override;
	Vector2 GetVector2(const std::string &name, const Vector2& defaultValue) const override;
	Vector3 GetVector3(const std::string &name, const Vector3& defaultValue) const override;

	float GetFloat(const std::string &name) const override;
	int GetInt(const std::string &name) const override;
	unsigned int GetUInt(const std::string &name) const override;
	std::string GetString(const std::string &name) const override;
	Vector2 GetVector2(const std::string &name) const override;
	Vector3 GetVector3(const std::string &name) const override;

	void SetObject(const std::string &name, void *value, int typeId);
	void GetObject(const std::string &name, void *value, int typeId);

	float AddToFloat(const std::string &name, const float &value) override;
	int AddToInt(const std::string &name, const int &value) override;
	unsigned int AddToUInt(const std::string &name, const unsigned int &value) override;
	Vector2 AddToVector2(const std::string &name, const Vector2 &v) override;
	Vector3 AddToVector3(const std::string &name, const Vector3 &v) override;

	float MultiplyFloat(const std::string &name, const float &value) override;
	int MultiplyInt(const std::string &name, const int &value) override;
	unsigned int MultiplyUInt(const std::string &name, const unsigned int &value) override;
	Vector2 MultiplyVector2(const std::string &name, const float &value) override;
	Vector3 MultiplyVector3(const std::string &name, const float &value) override;

	bool EraseData(const std::string &name) override;
	ETHCustomData::DATA_TYPE CheckCustomData(const std::string &name) const override;
	bool HasCustomData() const override;
	void DebugPrintCustomData() const override;
	void ClearCustomData() override;
	void InsertData(const ETHCustomDataManager &dataIn);
	void MoveData(ETHCustomDataManager &dataOut) const;

	void SetPivotAdjust(const Vector2& p) override;
	Vector2 GetPivotAdjust() const override;
	bool IsFixedRotation() const override;
	bool IsSensor() const override;
	bool HasSimulatedBody() const override;
	bool IsBullet();

	void SetAngelScriptObject(const std::string &name, void *value, int typeId) override;
	bool GetAngelScriptObject(const std::string &name, void *value, int typeId) override;

	#ifdef _ETHANON_EDITOR
	inline ETHEntityProperties* GetEditableProperties() { return &m_properties; }
	#endif

	virtual void ReleaseLightmap() = 0;
	
	float GetParallaxIntensity() const override;
	void SetParallaxIntensity(const float individualIntensity) override;
	ETHPolygonPtr GetPolygon() const;

	void SetPositionX(const float v, ETHBucketManager& buckets) override;
	void SetPositionY(const float v, ETHBucketManager& buckets) override;
	void SetPositionZ(const float v) override;
	float GetPositionX() const override;
	float GetPositionY() const override;
	float GetPositionZ() const override;
	void AddToPositionX(const float v, ETHBucketManager& buckets) override;
	void AddToPositionY(const float v, ETHBucketManager& buckets) override;
	void AddToPositionZ(const float v) override;

private:
	void Zero();

	void SetID(const int id) { m_id = id; }

	bool ReadFromXMLFile(
		TiXmlElement *pElement,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		Platform::FileManagerPtr fileManager,
		const bool shouldGenerateNewID);
	bool ReadFromXMLFile(TiXmlElement *pElement, const bool shouldGenerateNewID);
	void ReadInSceneDataFromXMLFile(TiXmlElement *pElement, const bool shouldGenerateNewID);

	int m_id;

	CScriptDictionary *m_gcDict;	// will be used to store script objects since we don't want to 
									// handle AngelScript's garbage collection stuff on our code

	void InstantiateDictionary();

protected:
	ETHEntityControllerPtr m_controller;
	ETHEntityProperties m_properties;
	unsigned int m_spriteFrame;
	Vector4 m_v4Color;
	Vector4 m_v4SolidColor;
	ETH_BOOL m_hide, m_flipX, m_flipY;
};

typedef ETHEntity* ETHEntityRawPtr;

#endif
