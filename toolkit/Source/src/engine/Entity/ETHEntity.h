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

#ifndef ETH_ENTITY_H_
#define ETH_ENTITY_H_

#include "ETHEntityProperties.h"
#include "ETHScriptEntity.h"
#include "ETHEntityController.h"
#include "../Physics/ETHCompoundShape.h"
#include "../addons/scriptdictionary.h"

class ETHEntity : public ETHScriptEntity
{
	friend class ETHScene;
public:
	static GS_ENTITY_ORIGIN ConvertToGSSO(const ETHEntityProperties::ENTITY_TYPE type);
	static float ComputeDepth(const float height, const float maxHeight, const float minHeight);

	ETHEntity(const str_type::string& filePath, const int nId, const Platform::FileManagerPtr& fileManager);
	ETHEntity(TiXmlElement *pElement);
	ETHEntity();
	~ETHEntity();

	virtual void Refresh(const ETHEntityProperties& properties) = 0;

	bool WriteToXMLFile(TiXmlElement *pHeadRoot) const;

	ETH_INLINE const ETHEntityProperties* GetProperties() const { return &m_properties; };
	ETHEntityControllerPtr GetController();
	void SetController(const ETHEntityControllerPtr& controller);

	void Kill();

	// General methods
	int GetID() const;
	Vector2 ComputeOrigin(const Vector2 &v2Size) const;
	Vector2 ComputeAbsoluteOrigin(const Vector2 &v2Size) const;
	Vector3 GetLightPosition() const;
	Vector3 GetLightRelativePosition() const;
	ETHCollisionBox GetCollisionBox() const;
	ETHCompoundShapePtr GetCompoundShape() const;
	void ChangeEntityName(const str_type::string& name);
	str_type::string GetEntityName() const;
	std::size_t GetNumParticleSystems() const;
	ETHEntityProperties::ENTITY_TYPE GetType() const;
	ETHEntityProperties::BODY_SHAPE GetShape() const;
	Vector3 GetPosition() const;
	Vector2 GetPositionXY() const;
	void SetPosition(const Vector3& pos, ETHBucketManager& buckets);
	void SetPositionXY(const Vector2& pos, ETHBucketManager& buckets);
	void AddToPosition(const Vector3& pos, ETHBucketManager& buckets);
	void AddToPositionXY(const Vector2& pos, ETHBucketManager& buckets);
	void AddToAngle(const float angle);
	void SetOrphanPosition(const Vector3& pos);
	void SetOrphanPositionXY(const Vector2& pos);
	float GetAngle() const;
	void SetAngle(const float angle);
	Vector4 GetColorF() const;
	bool SetFrame(const unsigned int frame);
	bool SetFrame(const unsigned int column, const unsigned int row);
	unsigned int GetFrame() const;
	bool IsCollidable() const;
	GS_ALPHA_MODE GetBlendMode() const;
	unsigned int GetNumFrames() const;
	bool HasAnyCallbackFunction() const;
	const ETHLight* GetLight() const;
	void TurnDynamic();
	void TurnStatic();
	bool HasSoundEffect() const;
	void Hide(const bool hide);
	Vector2 GetCurrentBucket(const ETHBucketManager& buckets) const;

	void SetEmissiveColor(const Vector3 &color);
	Vector3 GetEmissiveColor() const;
	void SetColor(const Vector3& color);
	void SetColor(const Vector4& color);
	void SetAlpha(const float alpha);
	Vector3 GetColor() const;
	Vector4 GetColorARGB() const;
	float GetAlpha() const;
	void SetShadowZ(const float z);
	float GetShadowZ() const;
	bool HasShadow() const;
	void SetLayerDepth(const float depth);
	float GetLayerDepth() const;
	float GetDensity(const unsigned int fixtureIdx) const;
	float GetFriction(const unsigned int fixtureIdx) const;
	float GetRestitution(const unsigned int fixtureIdx) const;

	void Scale(const Vector2& scale);
	void Scale(const float scale);
	void SetScale(const Vector2& scale);
	Vector2 GetScale() const;

	/// Scale the particle systems property files
	void ScaleParticleSystemOrigin(const unsigned int n, const float scale);

	bool IsRotatable() const;
	bool IsStatic() const;
	bool IsApplyLight() const;
	bool IsHidden() const;
	bool IsTemporary() const;
	bool IsCastShadow() const;
	bool IsInvisible() const;
	bool IsBody() const;

	bool HasLightSource() const;
	bool HasParticleSystems() const;
	bool HasHalo() const;

	bool RunCallbackScript();

	float GetSpecularPower() const;
	float GetSpecularBrightness() const;
	void SetLightRange(const float range);
	float GetLightRange() const;
	void SetLightColor(const Vector3 &color);
	Vector3 GetLightColor() const;
	void DisableLightSource();

	// Custom data related methods:
	const ETHCustomDataManager *GetCustomDataManager() const;
	void SetFloat(const str_type::string &name, const float &value);
	void SetInt(const str_type::string &name, const int &value);
	void SetUInt(const str_type::string &name, const unsigned int &value);
	void SetString(const str_type::string &name, const str_type::string &value);
	void SetVector2(const str_type::string &name, const Vector2 &value);
	void SetVector3(const str_type::string &name, const Vector3 &value);
	void AddData(const str_type::string &name, const ETHCustomDataConstPtr &dataIn);

	float GetFloat(const str_type::string &name) const;
	int GetInt(const str_type::string &name) const;
	unsigned int GetUInt(const str_type::string &name) const;
	str_type::string GetString(const str_type::string &name) const;
	Vector2 GetVector2(const str_type::string &name) const;
	Vector3 GetVector3(const str_type::string &name) const;

	void SetObject(const str_type::string &name, void *value, int typeId);
	void GetObject(const str_type::string &name, void *value, int typeId);

	void AddToFloat(const str_type::string &name, const float &value);
	void AddToInt(const str_type::string &name, const int &value);
	void AddToUInt(const str_type::string &name, const unsigned int &value);
	void AddToVector2(const str_type::string &name, const Vector2 &v);
	void AddToVector3(const str_type::string &name, const Vector3 &v);

	void MultiplyFloat(const str_type::string &name, const float &value);
	void MultiplyInt(const str_type::string &name, const int &value);
	void MultiplyUInt(const str_type::string &name, const unsigned int &value);
	void MultiplyVector2(const str_type::string &name, const float &value);
	void MultiplyVector3(const str_type::string &name, const float &value);

	bool EraseData(const str_type::string &name);
	ETHCustomData::DATA_TYPE CheckCustomData(const str_type::string &name) const;
	bool HasCustomData() const;
	void DebugPrintCustomData() const;
	void ClearCustomData();
	void InsertData(const ETHCustomDataManager &dataIn);
	void MoveData(ETHCustomDataManager &dataOut) const;

	void SetPivotAdjust(const Vector2& p);
	Vector2 GetPivotAdjust() const;
	bool IsFixedRotation() const;
	bool IsSensor() const;
	bool HasSimulatedBody() const;
	bool IsBullet();

	void SetAngelScriptObject(const str_type::string &name, void *value, int typeId);
	bool GetAngelScriptObject(const str_type::string &name, void *value, int typeId);

	#ifdef _ETHANON_EDITOR
	inline ETHEntityProperties* GetEditableProperties() { return &m_properties; }
	#endif

	virtual void ReleaseLightmap() = 0;
	
	float GetParallaxIndividualIntensity() const;
	void SetParallaxIndividualIntensity(const float& individualIntensity);
	ETHPolygonPtr GetPolygon() const;

	void SetPositionX(const float v, ETHBucketManager& buckets);
	void SetPositionY(const float v, ETHBucketManager& buckets);
	void SetPositionZ(const float v);
	float GetPositionX() const;
	float GetPositionY() const;
	float GetPositionZ() const;
	void AddToPositionX(const float v, ETHBucketManager& buckets);
	void AddToPositionY(const float v, ETHBucketManager& buckets);
	void AddToPositionZ(const float v);

private:
	void Zero();
	void SetID(const int id) { m_id = id; }
	bool ReadFromXMLFile(TiXmlElement *pElement);
	int m_id;

	CScriptDictionary *m_gcDict;	// will be used to store script objects since we don't want to 
									// handle AngelScript's garbage collection stuff on our code

	void InstantiateDictionary();
protected:
	ETHEntityControllerPtr m_controller;
	ETHEntityProperties m_properties;
	unsigned int m_spriteFrame;
	Vector4 m_v4Color;
	ETH_BOOL m_hide;
	float m_shadowZ;
};

typedef ETHEntity* ETHEntityRawPtr;

#endif