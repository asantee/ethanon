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

#include "ETHEntity.h"

#include "../Physics/ETHPhysicsSimulator.h"

Sprite::ENTITY_ORIGIN ETHEntity::ConvertToGSSO(const ETHEntityProperties::ENTITY_TYPE type)
{
	switch (type)
	{
	case ETHEntityProperties::ET_GROUND_DECAL:
	case ETHEntityProperties::ET_OPAQUE_DECAL:
	case ETHEntityProperties::ET_OVERALL:
	case ETHEntityProperties::ET_LAYERABLE:
	case ETHEntityProperties::ET_HORIZONTAL:
		return Sprite::EO_CENTER;
		break;
	case ETHEntityProperties::ET_VERTICAL:
		return Sprite::EO_CENTER_BOTTOM;
		break;
	default:
		return Sprite::EO_DEFAULT;
	};
}

float ETHEntity::ComputeDepth(const float height, const float maxHeight, const float minHeight)
{
	return ((height - minHeight) / (maxHeight - minHeight));
}

ETHEntity::ETHEntity(const str_type::string& filePath, const int nId, const Platform::FileManagerPtr& fileManager) :
	ETHScriptEntity(),
	m_properties(filePath, fileManager),
	m_id(nId),
	m_controller(new ETHRawEntityController(Vector3(0, 0, 0), 0.0f))
{
	Zero();
}

ETHEntity::ETHEntity(
	TiXmlElement *pElement,
	ETHEntityCache& entityCache,
	const str_type::string &entityPath,
	Platform::FileManagerPtr fileManager) :
	ETHScriptEntity(),
	m_id(-1),
	m_controller(new ETHRawEntityController(Vector3(0, 0, 0), 0.0f))
{
	Zero();
	ReadFromXMLFile(pElement, entityCache, entityPath, fileManager);
}

ETHEntity::ETHEntity() : 
	ETHScriptEntity(),
	m_id(-1),
	m_controller(new ETHRawEntityController(Vector3(0, 0, 0), 0.0f))
{
	Zero();
}

ETHEntity::~ETHEntity()
{
	if (m_gcDict)
	{
		m_gcDict->Release();
	}
	m_controller->Destroy();
}

void ETHEntity::InstantiateDictionary()
{
	asIScriptEngine *engine = asGetActiveContext()->GetEngine();
	m_gcDict = new CScriptDictionary(engine);
}

#define ETH_DEFAULT_COLOR Vector4(1, 1, 1, 1)

void ETHEntity::Zero()
{
	m_v4Color = ETH_DEFAULT_COLOR;
	m_spriteFrame = (0);
	m_shadowZ = (0.0f);
	m_hide = m_flipX = m_flipY = (ETH_FALSE);
	m_gcDict = 0;
}

void ETHEntity::SetAngelScriptObject(const str_type::string &name, void *value, int typeId)
{
	if (!m_gcDict)
		InstantiateDictionary();
	m_gcDict->Set(name, value, typeId);
}

bool ETHEntity::GetAngelScriptObject(const str_type::string &name, void *value, int typeId)
{
	if (!m_gcDict)
		return false;
	return m_gcDict->Get(name, value, typeId);
}

bool ETHEntity::WriteToXMLFile(
	TiXmlElement *pHeadRoot,
	ETHEntityCache& entityCache,
	const str_type::string &entityPath,
	Platform::FileManagerPtr fileManager) const
{
	TiXmlElement *pEntity = new TiXmlElement(GS_L("Entity"));
	pHeadRoot->LinkEndChild(pEntity);

	TiXmlElement *pElement;

	// if it has a specific name, save it
	if (m_properties.entityName != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("EntityName"));
		pElement->LinkEndChild(new TiXmlText(m_properties.entityName));
		pEntity->LinkEndChild(pElement);
	}

	if (m_v4Color != ETH_DEFAULT_COLOR)
		ETHEntityProperties::SetColorPropertyToXmlElement(pEntity, GS_L("Color"), m_v4Color);

	pElement = new TiXmlElement(GS_L("Position"));
	pEntity->LinkEndChild(pElement);
	{
		const Vector3 pos(m_controller->GetPos());
		pElement->SetDoubleAttribute(GS_L("x"), pos.x);
		pElement->SetDoubleAttribute(GS_L("y"), pos.y);
		pElement->SetDoubleAttribute(GS_L("z"), pos.z);
		pElement->SetDoubleAttribute(GS_L("angle"), m_controller->GetAngle());
		
	}
	pEntity->SetAttribute(GS_L("id"), m_id);
	pEntity->SetAttribute(GS_L("spriteFrame"), m_spriteFrame);

	if (m_spriteFrame > 0)
		pEntity->SetDoubleAttribute(GS_L("spriteFrame"), m_spriteFrame);

	if (m_properties.castShadow)
		pEntity->SetDoubleAttribute(GS_L("shadowZ"), m_shadowZ);

	ETHEntityProperties::SetBooleanPropertyToXmlElement(pEntity, GS_L("hide"),  m_hide,  ETH_FALSE);
	ETHEntityProperties::SetBooleanPropertyToXmlElement(pEntity, GS_L("flipX"), m_flipX, ETH_FALSE);
	ETHEntityProperties::SetBooleanPropertyToXmlElement(pEntity, GS_L("flipY"), m_flipY, ETH_FALSE);

	// write entity data as file reference or inline data (if the entity source file doesn't exist)
	if (entityCache.Get(m_properties.entityName, entityPath, fileManager))
	{
		m_properties.WriteEntityNameToXMLFile(pEntity);
	}
	else
	{
		m_properties.WriteContentToXMLFile(pEntity);
	}
	return true;
}

bool ETHEntity::ReadFromXMLFile(TiXmlElement *pElement)
{
	ReadInSceneDataFromXMLFile(pElement);
	TiXmlNode *pNode = pElement->FirstChild(GS_L("Entity"));
	if (pNode)
	{
		return m_properties.ReadFromXMLFile(pNode->ToElement());
	}
	else
	{
		return false;
	}
}

bool ETHEntity::ReadFromXMLFile(
	TiXmlElement *pElement,
	ETHEntityCache& entityCache,
	const str_type::string &entityPath,
	Platform::FileManagerPtr fileManager)
{
	ReadInSceneDataFromXMLFile(pElement);
	TiXmlNode *pNode = pElement->FirstChild(GS_L("Entity"));
	if (pNode)
	{
		return m_properties.ReadFromXMLFile(pNode->ToElement(), entityCache, entityPath, fileManager);
	}
	else
	{
		return false;
	}
}

void ETHEntity::ReadInSceneDataFromXMLFile(TiXmlElement *pElement)
{
	pElement->QueryIntAttribute(GS_L("id"), &m_id);
	pElement->QueryFloatAttribute(GS_L("shadowZ"), &m_shadowZ);

	m_hide = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("hide"), m_hide);
	m_flipX = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("flipX"), m_flipX);
	m_flipY = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("flipY"), m_flipY);

	int signedSpriteFrame = 0;
	pElement->QueryIntAttribute(GS_L("spriteFrame"), &signedSpriteFrame);
	m_spriteFrame = static_cast<unsigned int>(signedSpriteFrame);

	TiXmlNode *pNode;
	TiXmlElement *pStringElement;

	pNode = pElement->FirstChild(GS_L("EntityName"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			m_properties.entityName = pStringElement->GetText();
		}
	}

	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("Color"), m_v4Color);

	pNode = pElement->FirstChild(GS_L("Position"));
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			Vector3 pos; float angle = 0.0f;
			pIter->QueryFloatAttribute(GS_L("x"), &pos.x);
			pIter->QueryFloatAttribute(GS_L("y"), &pos.y);
			pIter->QueryFloatAttribute(GS_L("z"), &pos.z);
			pIter->QueryFloatAttribute(GS_L("angle"), &angle);
			m_controller->SetPos(pos);
			m_controller->SetAngle(angle);
		}
	}
}

int ETHEntity::GetID() const
{
	return m_id;
}

ETHEntityControllerPtr ETHEntity::GetController()
{
	return m_controller;
}

void ETHEntity::SetController(const ETHEntityControllerPtr& controller)
{
	m_controller = controller;
}

Vector2 ETHEntity::ComputeAbsoluteOrigin(const Vector2 &v2Size) const
{
	Vector2 v2Center;
	switch (ETHEntity::ConvertToGSSO(GetType()))
	{
	case Sprite::EO_RECT_CENTER:
	case Sprite::EO_CENTER:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = v2Size.y / 2.0f;
		break;
	case Sprite::EO_RECT_CENTER_BOTTOM:
	case Sprite::EO_CENTER_BOTTOM:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = v2Size.y;
		break;
	case Sprite::EO_RECT_CENTER_TOP:
	case Sprite::EO_CENTER_TOP:
		v2Center.x = v2Size.x / 2.0f;
		v2Center.y = 0.0f;
		break;
	default:
	case Sprite::EO_DEFAULT:
		v2Center.x = 0.0f;
		v2Center.y = 0.0f;
		break;
	};	
	return (v2Center + (m_properties.pivotAdjust * m_properties.scale));
}

Vector2 ETHEntity::ComputeOrigin(const Vector2 &v2Size) const
{
	return (ComputeAbsoluteOrigin(v2Size) / v2Size);
}

void ETHEntity::ChangeEntityName(const str_type::string& name)
{
	m_properties.entityName = name;
}

str_type::string ETHEntity::GetEntityName() const
{
	return m_properties.entityName;
}

ETHCollisionBox ETHEntity::GetCollisionBox() const
{
	if (IsCollidable())
	{
		return *m_properties.collision.get();
	}
	return ETHCollisionBox(Vector3(0,0,0), Vector3(0,0,0));
}

std::size_t ETHEntity::GetNumParticleSystems() const
{
	return m_properties.particleSystems.size();
}

ETHEntityProperties::ENTITY_TYPE ETHEntity::GetType() const
{
	return m_properties.type;
}

ETHEntityProperties::BODY_SHAPE ETHEntity::GetShape() const
{
	return m_properties.shape;
}

Vector3 ETHEntity::GetPosition() const
{
	return m_controller->GetPos();
}

Vector2 ETHEntity::GetPositionXY() const
{
	return m_controller->GetPosXY();
}

float ETHEntity::GetPositionX() const
{
	return m_controller->GetPosX();
}

float ETHEntity::GetPositionY() const
{
	return m_controller->GetPosY();
}

float ETHEntity::GetPositionZ() const
{
	return m_controller->GetPosZ();
}

void ETHEntity::SetPosition(const Vector3& pos, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->SetPos(pos);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::SetPositionXY(const Vector2& pos, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->SetPosXY(pos);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::SetPositionX(const float v, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->SetPosX(v);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::SetPositionY(const float v, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->SetPosY(v);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::SetPositionZ(const float v)
{
	m_controller->SetPosZ(v);
}

void ETHEntity::AddToPosition(const Vector3& pos, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->AddToPos(pos);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::AddToPositionXY(const Vector2& pos, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->AddToPosXY(pos);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::AddToPositionX(const float v, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->AddToPosX(v);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::AddToPositionY(const float v, ETHBucketManager& buckets)
{
	const Vector2& oldPos(GetPositionXY());
	m_controller->AddToPosY(v);
	buckets.RequestBucketMove(this, oldPos, GetPositionXY());
}

void ETHEntity::AddToPositionZ(const float v)
{
	m_controller->AddToPosZ(v);
}

void ETHEntity::SetOrphanPosition(const Vector3& pos)
{
	m_controller->SetPos(pos);
}

void ETHEntity::SetOrphanPositionXY(const Vector2& pos)
{
	m_controller->SetPos(Vector3(pos, m_controller->GetPos().z));
}

void ETHEntity::SetAngle(const float angle)
{
	m_controller->SetAngle(angle);
}

void ETHEntity::AddToAngle(const float angle)
{
	m_controller->AddToAngle(angle);
}

float ETHEntity::GetAngle() const
{
	return m_controller->GetAngle();
}

void ETHEntity::SetLightRange(const float range)
{
	if (HasLightSource())
		m_properties.light->range = range;
}

float ETHEntity::GetLightRange() const
{
	if (HasLightSource())
		return m_properties.light->range;
	else
		return 0.0f;
}

void ETHEntity::SetLightColor(const Vector3 &color)
{
	if (HasLightSource())
		m_properties.light->color = color;
}

Vector3 ETHEntity::GetLightColor() const
{
	if (HasLightSource())
		return m_properties.light->color;
	else
		return Vector3(0, 0, 0);
}

void ETHEntity::SetEmissiveColor(const Vector3 &color)
{
	m_properties.emissiveColor = Vector4(color, 1);
}

Vector3 ETHEntity::GetEmissiveColor() const
{
	const Vector4 *pColor = &m_properties.emissiveColor;
	return Vector3(pColor->x, pColor->y, pColor->z);
}

void ETHEntity::SetColor(const Vector3& color)
{
	m_v4Color.x = color.x;
	m_v4Color.y = color.y;
	m_v4Color.z = color.z;
}

void ETHEntity::SetColor(const Vector4& color)
{
	m_v4Color = color;
}

void ETHEntity::SetAlpha(const float alpha)
{
	m_v4Color.w = alpha;
}

Vector3 ETHEntity::GetColor() const
{
	return Vector3(m_v4Color.x, m_v4Color.y, m_v4Color.z);
}

Vector4 ETHEntity::GetColorARGB() const
{
	return m_v4Color;
}

float ETHEntity::GetAlpha() const
{
	return m_v4Color.w;
}

void ETHEntity::SetShadowZ(const float z)
{
	m_shadowZ = z;
}

float ETHEntity::GetShadowZ() const
{
	return m_shadowZ;
}

bool ETHEntity::HasShadow() const
{
	return (m_properties.castShadow == ETH_TRUE);
}

void ETHEntity::SetLayerDepth(const float depth)
{
	m_properties.type = ETHEntityProperties::ET_LAYERABLE;
	m_properties.layerDepth = Max(0.0f, Min(depth, 1.0f));
}

float ETHEntity::GetLayerDepth() const
{
	return (m_properties.type == ETHEntityProperties::ET_LAYERABLE) ? m_properties.layerDepth : -1.0f;
}

float ETHEntity::GetDensity(const unsigned int fixtureIdx) const
{
	if (m_properties.shape == ETHEntityProperties::BS_COMPOUND)
	{
		if (m_properties.compoundShape)
			return m_properties.compoundShape->GetIndividualDensity(fixtureIdx, m_properties.density);
	}
	return m_properties.density;
}

float ETHEntity::GetFriction(const unsigned int fixtureIdx) const
{
	if (m_properties.shape == ETHEntityProperties::BS_COMPOUND)
	{
		if (m_properties.compoundShape)
			return m_properties.compoundShape->GetIndividualFriction(fixtureIdx, m_properties.friction);
	}
	return m_properties.friction;
}

float ETHEntity::GetRestitution(const unsigned int fixtureIdx) const
{
	if (m_properties.shape == ETHEntityProperties::BS_COMPOUND)
	{
		if (m_properties.compoundShape)
			return m_properties.compoundShape->GetIndividualRestitution(fixtureIdx, m_properties.restitution);
	}
	return m_properties.restitution;
}

void ETHEntity::Scale(const Vector2& scale)
{
	m_properties.scale = m_properties.scale * scale;
	for (std::size_t t = 0; t < GetNumParticleSystems(); t++)
	{
		ScaleParticleSystem(static_cast<unsigned int>(t), (scale.x + scale.y) / 2.0f);
	}
	m_controller->Scale(scale, this);
}

void ETHEntity::Scale(const float scale)
{
	m_properties.scale *= scale;
	for (std::size_t t = 0; t < GetNumParticleSystems(); t++)
	{
		ScaleParticleSystem(static_cast<unsigned int>(t), scale);
	}
	m_controller->Scale(Vector2(scale, scale), this);
}

void ETHEntity::SetScale(const Vector2& scale)
{
	m_properties.scale = scale;
	/*for (std::size_t t = 0; t < GetNumParticleSystems(); t++)
	{
		ScaleParticleSystem(t, (scale.x + scale.y) / 2.0f);
	}*/
}

Vector2 ETHEntity::GetScale() const
{
	return m_properties.scale;
}

void ETHEntity::ScaleParticleSystemOrigin(const unsigned int n, const float scale)
{
	if (m_properties.particleSystems[n])
		m_properties.particleSystems[n]->Scale(scale);
}

bool ETHEntity::IsRotatable() const
{
	return GetType() != ETHEntityProperties::ET_VERTICAL;
}

bool ETHEntity::HasLightSource() const
{
	return static_cast<bool>(m_properties.light);
}

bool ETHEntity::HasParticleSystems() const
{
	return (m_properties.particleSystems.size() > 0);
}

Vector3 ETHEntity::GetLightPosition() const
{
	if (HasLightSource())
		return GetPosition() + m_properties.light->pos;
	else
		return Vector3(0,0,0);
}

void ETHEntity::SetLightPosition(const Vector3& pos)
{
	if (HasLightSource())
	{
		m_properties.light->pos = pos;
	}
}

Vector3 ETHEntity::GetLightRelativePosition() const
{
	if (HasLightSource())
		return m_properties.light->pos;
	else
		return Vector3(0,0,0);
}

bool ETHEntity::HasHalo() const
{
	return (!HasLightSource()) ? false : (!m_properties.light->haloBitmap.empty());
}

bool ETHEntity::IsCollidable() const
{
	return static_cast<bool>(m_properties.collision);
}

Video::ALPHA_MODE ETHEntity::GetBlendMode() const
{
	return m_properties.blendMode;
}

unsigned int ETHEntity::GetNumFrames() const
{
	return static_cast<unsigned int>(m_properties.spriteCut.x * m_properties.spriteCut.y);
}

bool ETHEntity::HasAnyCallbackFunction() const
{
	return m_controller->HasAnyCallbackFunction();
}

bool ETHEntity::IsStatic() const
{
	return ETHGlobal::ToBool(m_properties.staticEntity);
}

bool ETHEntity::IsApplyLight() const
{
	return ETHGlobal::ToBool(m_properties.applyLight);
}

void ETHEntity::Hide(const bool hide)
{
	m_hide = hide;
}

bool ETHEntity::IsHidden() const
{
	return !(m_hide == ETH_FALSE);
}

bool ETHEntity::IsTemporary() const
{
	unsigned int temporary = 0, existent = 0;
	for (std::size_t t=0; t<m_properties.particleSystems.size(); t++)
	{
		if (m_properties.particleSystems[t]->nParticles > 0)
		{
			if (m_properties.particleSystems[t]->repeat > 0)
				temporary++;
			existent++;
		}
	}
	if (existent && temporary == existent && m_properties.spriteFile == GS_L(""))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHEntity::IsCastShadow() const
{
	return !(m_properties.castShadow == ETH_FALSE);
}

bool ETHEntity::IsInvisible() const
{
	return (m_properties.spriteFile == GS_L("") && !HasParticleSystems());
}

bool ETHEntity::IsBody() const
{
	return (m_properties.shape != ETHEntityProperties::BS_NONE
			&& IsCollidable());
}

Vector2 ETHEntity::GetCurrentBucket(const ETHBucketManager& buckets) const
{
	return ETHBucketManager::GetBucket(GetPositionXY(), buckets.GetBucketSize());
}

const ETHLight* ETHEntity::GetLight() const
{
	return m_properties.light.get();
}

bool ETHEntity::SetFrame(const unsigned int frame)
{
	const Vector2i *pv2Cut = &m_properties.spriteCut;
	if (frame > static_cast<unsigned int>(pv2Cut->x * pv2Cut->y))
	{
		m_spriteFrame = (0);
		return false;
	}
	else
	{
		m_spriteFrame = (frame);
		return true;
	}
}

unsigned int ETHEntity::GetFrame() const
{
	return m_spriteFrame;
}

bool ETHEntity::SetFrame(const unsigned int column, const unsigned int row)
{
	const Vector2i *pv2Cut = &m_properties.spriteCut;
	const unsigned int cutX = static_cast<unsigned int>(pv2Cut->x);
	const unsigned int cutY = static_cast<unsigned int>(pv2Cut->y);
	if (column >= cutX || row >= cutY)
	{
		m_spriteFrame = (0);
		return false;
	}
	else
	{
		m_spriteFrame = ((row*cutX)+column);
		return true;
	}
}

bool ETHEntity::RunCallbackScript()
{
	return m_controller->RunCallback(this);
}

float ETHEntity::GetSpecularPower() const
{
	return m_properties.specularPower;
}

float ETHEntity::GetSpecularBrightness() const
{
	return m_properties.specularBrightness;
}

void ETHEntity::TurnDynamic()
{
	m_properties.staticEntity = false;
	if (m_properties.light)
		m_properties.light->staticLight = false;
}

void ETHEntity::TurnStatic()
{
	m_properties.staticEntity = true;
	if (m_properties.light)
		m_properties.light->staticLight = true;
}

const ETHCustomDataManager *ETHEntity::GetCustomDataManager() const
{
	return &m_properties;
}

void ETHEntity::AddData(const str_type::string &name, const ETHCustomDataConstPtr &dataIn)
{
	m_properties.AddData(name, dataIn);
}

void ETHEntity::SetFloat(const str_type::string &name, const float &value)
{
	m_properties.SetFloat(name, value);
}

void ETHEntity::SetInt(const str_type::string &name, const int &value)
{
	m_properties.SetInt(name, value);
}

void ETHEntity::SetUInt(const str_type::string &name, const unsigned int &value)
{
	m_properties.SetUInt(name, value);
}

void ETHEntity::SetString(const str_type::string &name, const str_type::string &value)
{
	m_properties.SetString(name, value);
}

void ETHEntity::SetVector2(const str_type::string &name, const Vector2 &value)
{
	m_properties.SetVector2(name, value);
}

void ETHEntity::SetVector3(const str_type::string &name, const Vector3 &value)
{
	m_properties.SetVector3(name, value);
}

float ETHEntity::GetFloat(const str_type::string &name) const
{
	float fOut = 0.0f;
	m_properties.GetFloat(name, fOut);
	return fOut;
}

int ETHEntity::GetInt(const str_type::string &name) const
{
	int nOut = 0;
	m_properties.GetInt(name, nOut);
	return nOut;
}

unsigned int ETHEntity::GetUInt(const str_type::string &name) const
{
	unsigned int nOut = 0;
	m_properties.GetUInt(name, nOut);
	return nOut;
}

str_type::string ETHEntity::GetString(const str_type::string &name) const
{
	str_type::string sOut = GS_L("");
	m_properties.GetString(name, sOut);
	return sOut;
}

Vector2 ETHEntity::GetVector2(const str_type::string &name) const
{
	Vector2 vOut(0,0);
	m_properties.GetVector2(name, vOut);
	return vOut;
}

Vector3 ETHEntity::GetVector3(const str_type::string &name) const
{
	Vector3 vOut(0,0,0);
	m_properties.GetVector3(name, vOut);
	return vOut;
}

void ETHEntity::AddToFloat(const str_type::string &name, const float &value)
{
	m_properties.AddToFloat(name, value);
}

void ETHEntity::AddToInt(const str_type::string &name, const int &value)
{
	m_properties.AddToInt(name, value);
}

void ETHEntity::AddToUInt(const str_type::string &name, const unsigned int &value)
{
	m_properties.AddToUInt(name, value);
}

void ETHEntity::AddToVector2(const str_type::string &name, const Vector2 &v)
{
	m_properties.AddToVector2(name, v);
}

void ETHEntity::AddToVector3(const str_type::string &name, const Vector3 &v)
{
	m_properties.AddToVector3(name, v);
}

void ETHEntity::MultiplyFloat(const str_type::string &name, const float &value)
{
	m_properties.MultiplyFloat(name, value);
}

void ETHEntity::MultiplyInt(const str_type::string &name, const int &value)
{
	m_properties.MultiplyInt(name, value);
}

void ETHEntity::MultiplyUInt(const str_type::string &name, const unsigned int &value)
{
	m_properties.MultiplyUInt(name, value);
}

void ETHEntity::MultiplyVector2(const str_type::string &name, const float &value)
{
	m_properties.MultiplyVector2(name, value);
}

void ETHEntity::MultiplyVector3(const str_type::string &name, const float &value)
{
	m_properties.MultiplyVector3(name, value);
}

void ETHEntity::InsertData(const ETHCustomDataManager &dataIn)
{
	m_properties.InsertData(dataIn);
}

void ETHEntity::MoveData(ETHCustomDataManager &dataOut) const
{
	m_properties.MoveData(dataOut);
}

bool ETHEntity::EraseData(const str_type::string &name)
{
	return m_properties.EraseData(name);
}

ETHCustomData::DATA_TYPE ETHEntity::CheckCustomData(const str_type::string &name) const
{
	return m_properties.Check(name);
}

bool ETHEntity::HasCustomData() const
{
	return m_properties.HasData();
}

void ETHEntity::DebugPrintCustomData() const
{
	GS2D_COUT << m_properties.GetDebugStringData();
}

void ETHEntity::ClearCustomData()
{
	m_properties.Clear();
}

void ETHEntity::Kill()
{
	ETHScriptEntity::Kill();
	m_controller->Destroy();
}

bool ETHEntity::IsFixedRotation() const
{
	return (m_properties.fixedRotation == ETH_TRUE);
}

bool ETHEntity::IsSensor() const
{
	return (m_properties.sensor == ETH_TRUE);
}

bool ETHEntity::HasSimulatedBody() const
{
	return static_cast<bool>(boost::dynamic_pointer_cast<ETHPhysicsEntityController>(m_controller));
}

bool ETHEntity::IsBullet()
{
	return (m_properties.bullet == ETH_TRUE);
}

float ETHEntity::GetParallaxIntensity() const
{
	return m_properties.parallaxIntensity;
}

void ETHEntity::SetParallaxIntensity(const float individualIntensity)
{
	m_properties.parallaxIntensity = individualIntensity;
}

ETHPolygonPtr ETHEntity::GetPolygon() const
{
	return m_properties.polygon;
}

void ETHEntity::SetPivotAdjust(const Vector2& p)
{
	m_properties.pivotAdjust = p / GetScale();
}

gs2d::math::Vector2 ETHEntity::GetPivotAdjust() const
{
	return m_properties.pivotAdjust * GetScale();
}

ETHCompoundShapePtr ETHEntity::GetCompoundShape() const
{
	return m_properties.compoundShape;
}

void ETHEntity::DisableLightSource()
{
	m_properties.light.reset();
}

void ETHEntity::SetFlipX(const bool flipX)
{
	m_flipX = (flipX) ? ETH_TRUE : ETH_FALSE;
}

void ETHEntity::SetFlipY(const bool flipY)
{
	m_flipY = (flipY) ? ETH_TRUE : ETH_FALSE;
}

bool ETHEntity::GetFlipX() const
{
	return (m_flipX == ETH_TRUE);
}

bool ETHEntity::GetFlipY() const
{
	return (m_flipY == ETH_TRUE);
}
