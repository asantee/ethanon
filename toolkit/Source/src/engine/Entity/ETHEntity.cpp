#include "ETHEntity.h"

#include "../Physics/ETHPhysicsSimulator.h"

#include "../Scene/ETHScene.h"

float ETHEntity::ComputeDepth(const float height, const float maxHeight, const float minHeight)
{
	return ((height - minHeight) / (maxHeight - minHeight));
}

ETHEntity::ETHEntity(const std::string& filePath, const int nId, const Platform::FileManagerPtr& fileManager) :
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
	const std::string &entityPath,
	Platform::FileManagerPtr fileManager,
	const bool shouldGenerateNewID) :
	ETHScriptEntity(),
	m_id(-1),
	m_controller(new ETHRawEntityController(Vector3(0, 0, 0), 0.0f))
{
	Zero();
	ReadFromXMLFile(pElement, entityCache, entityPath, fileManager, shouldGenerateNewID);
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
	m_gcDict = CScriptDictionary::Create(engine);
}

#define ETH_DEFAULT_COLOR Vector4(1, 1, 1, 1)

void ETHEntity::Zero()
{
	m_v4Color = ETH_DEFAULT_COLOR;
	m_v4SolidColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	m_spriteFrame = (0);
	m_hide = m_flipX = m_flipY = (ETH_FALSE);
	m_gcDict = 0;
}

void ETHEntity::SetAngelScriptObject(const std::string &name, void *value, int typeId)
{
	if (!m_gcDict)
		InstantiateDictionary();
	m_gcDict->Set(name, value, typeId);
}

bool ETHEntity::GetAngelScriptObject(const std::string &name, void *value, int typeId)
{
	if (!m_gcDict)
		return false;
	return m_gcDict->Get(name, value, typeId);
}

bool ETHEntity::WriteToXMLFile(
	TiXmlElement *pHeadRoot,
	ETHEntityCache& entityCache,
	const std::string &entityPath,
	Platform::FileManagerPtr fileManager) const
{
	TiXmlElement *pEntity = new TiXmlElement(("Entity"));
	pHeadRoot->LinkEndChild(pEntity);

	TiXmlElement *pElement;

	// if it has a specific name, save it
	if (m_properties.entityName != (""))
	{
		pElement = new TiXmlElement(("EntityName"));
		pElement->LinkEndChild(new TiXmlText(m_properties.entityName));
		pEntity->LinkEndChild(pElement);
	}

	if (m_v4Color != ETH_DEFAULT_COLOR)
		ETHEntityProperties::SetColorPropertyToXmlElement(pEntity, ("Color"), m_v4Color);

	pElement = new TiXmlElement(("Position"));
	pEntity->LinkEndChild(pElement);
	{
		const Vector3 pos(m_controller->GetPos());
		pElement->SetDoubleAttribute(("x"), pos.x);
		pElement->SetDoubleAttribute(("y"), pos.y);
		pElement->SetDoubleAttribute(("z"), pos.z);
		pElement->SetDoubleAttribute(("angle"), m_controller->GetAngle());
		
	}
	pEntity->SetAttribute(("id"), m_id);
	pEntity->SetAttribute(("spriteFrame"), m_spriteFrame);

	if (m_spriteFrame > 0)
		pEntity->SetDoubleAttribute(("spriteFrame"), m_spriteFrame);

	ETHEntityProperties::SetBooleanPropertyToXmlElement(pEntity, ("hide"),  m_hide,  ETH_FALSE);
	ETHEntityProperties::SetBooleanPropertyToXmlElement(pEntity, ("flipX"), m_flipX, ETH_FALSE);
	ETHEntityProperties::SetBooleanPropertyToXmlElement(pEntity, ("flipY"), m_flipY, ETH_FALSE);

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

bool ETHEntity::ReadFromXMLFile(TiXmlElement *pElement, const bool shouldGenerateNewID)
{
	ReadInSceneDataFromXMLFile(pElement, shouldGenerateNewID);
	TiXmlNode *pNode = pElement->FirstChild(("Entity"));
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
	const std::string &entityPath,
	Platform::FileManagerPtr fileManager,
	const bool shouldGenerateNewID)
{
	ReadInSceneDataFromXMLFile(pElement, shouldGenerateNewID);
	TiXmlNode *pNode = pElement->FirstChild(("Entity"));
	if (pNode)
	{
		return m_properties.ReadFromXMLFile(pNode->ToElement(), entityCache, entityPath, fileManager);
	}
	else
	{
		return false;
	}
}

void ETHEntity::ReadInSceneDataFromXMLFile(TiXmlElement *pElement, const bool shouldGenerateNewID)
{
	if (shouldGenerateNewID)
	{
		m_id = ETHScene::UpdateIDCounter(this);
	}
	else
	{
		pElement->QueryIntAttribute(("id"), &m_id);
	}

	m_hide = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("hide"), m_hide);
	m_flipX = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("flipX"), m_flipX);
	m_flipY = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("flipY"), m_flipY);

	int signedSpriteFrame = 0;
	pElement->QueryIntAttribute(("spriteFrame"), &signedSpriteFrame);
	m_spriteFrame = static_cast<unsigned int>(signedSpriteFrame);

	TiXmlNode *pNode;
	TiXmlElement *pStringElement;

	pNode = pElement->FirstChild(("EntityName"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			m_properties.entityName = pStringElement->GetText();
		}
	}

	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("Color"), m_v4Color);

	pNode = pElement->FirstChild(("Position"));
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			Vector3 pos; float angle = 0.0f;
			pIter->QueryFloatAttribute(("x"), &pos.x);
			pIter->QueryFloatAttribute(("y"), &pos.y);
			pIter->QueryFloatAttribute(("z"), &pos.z);
			pIter->QueryFloatAttribute(("angle"), &angle);
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

void ETHEntity::ChangeEntityName(const std::string& name)
{
	m_properties.entityName = name;
}

std::string ETHEntity::GetEntityName() const
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

Vector3 ETHEntity::GetSolidColor() const
{
	return Vector3(m_v4SolidColor.x, m_v4SolidColor.y, m_v4SolidColor.z);
}

void ETHEntity::SetSolidColor(const Vector3& color)
{
	m_v4SolidColor.x = color.x;
	m_v4SolidColor.y = color.y;
	m_v4SolidColor.z = color.z;
}

void ETHEntity::SetSolidColorAlpha(const float alpha)
{
	m_v4SolidColor.w = alpha;
}

float ETHEntity::GetSolidColorAlpha() const
{
	return m_v4SolidColor.w;
}

Vector4 ETHEntity::GetSolidColorARGB() const
{
	return m_v4SolidColor;
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
	return true;
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
		return m_properties.light->pos;
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

bool ETHEntity::HasAnyCallbackFunction() const
{
	return m_controller->HasAnyCallbackFunction();
}

bool ETHEntity::IsStatic() const
{
	return ETHGlobal::ToBool(m_properties.staticEntity);
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
	if (existent && temporary == existent && m_properties.spriteFile == (""))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ETHEntity::IsInvisible() const
{
	return (m_properties.spriteFile == ("") && !HasParticleSystems());
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

bool ETHEntity::RunCallbackScript()
{
	return m_controller->RunCallback(this);
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

void ETHEntity::AddData(const std::string &name, const ETHCustomDataConstPtr &dataIn)
{
	m_properties.AddData(name, dataIn);
}

void ETHEntity::SetFloat(const std::string &name, const float &value)
{
	m_properties.SetFloat(name, value);
}

void ETHEntity::SetInt(const std::string &name, const int &value)
{
	m_properties.SetInt(name, value);
}

void ETHEntity::SetUInt(const std::string &name, const unsigned int &value)
{
	m_properties.SetUInt(name, value);
}

void ETHEntity::SetString(const std::string &name, const std::string &value)
{
	m_properties.SetString(name, value);
}

void ETHEntity::SetVector2(const std::string &name, const Vector2 &value)
{
	m_properties.SetVector2(name, value);
}

void ETHEntity::SetVector3(const std::string &name, const Vector3 &value)
{
	m_properties.SetVector3(name, value);
}

float ETHEntity::GetFloat(const std::string &name, const float defaultValue) const
{
	float fOut = defaultValue;
	m_properties.GetFloat(name, fOut);
	return fOut;
}

int ETHEntity::GetInt(const std::string &name, const int defaultValue) const
{
	int nOut = defaultValue;
	m_properties.GetInt(name, nOut);
	return nOut;
}

unsigned int ETHEntity::GetUInt(const std::string &name, const unsigned int defaultValue) const
{
	unsigned int nOut = defaultValue;
	m_properties.GetUInt(name, nOut);
	return nOut;
}

std::string ETHEntity::GetString(const std::string &name, const std::string& defaultValue) const
{
	std::string sOut = defaultValue;
	m_properties.GetString(name, sOut);
	return sOut;
}

Vector2 ETHEntity::GetVector2(const std::string &name, const Vector2& defaultValue) const
{
	Vector2 vOut(defaultValue);
	m_properties.GetVector2(name, vOut);
	return vOut;
}

Vector3 ETHEntity::GetVector3(const std::string &name, const Vector3& defaultValue) const
{
	Vector3 vOut(defaultValue);
	m_properties.GetVector3(name, vOut);
	return vOut;
}

float ETHEntity::GetFloat(const std::string &name) const
{
	return GetFloat(name, 0.0f);
}

int ETHEntity::GetInt(const std::string &name) const
{
	return GetInt(name, 0);
}

unsigned int ETHEntity::GetUInt(const std::string &name) const
{
	return GetUInt(name, 0);
}

std::string ETHEntity::GetString(const std::string &name) const
{
	return GetString(name, (""));
}

Vector2 ETHEntity::GetVector2(const std::string &name) const
{
	return GetVector2(name, Vector2());
}

Vector3 ETHEntity::GetVector3(const std::string &name) const
{
	return GetVector3(name, Vector3());
}

float ETHEntity::AddToFloat(const std::string &name, const float &value)
{
	return m_properties.AddToFloat(name, value);
}

int ETHEntity::AddToInt(const std::string &name, const int &value)
{
	return m_properties.AddToInt(name, value);
}

unsigned int ETHEntity::AddToUInt(const std::string &name, const unsigned int &value)
{
	return m_properties.AddToUInt(name, value);
}

Vector2 ETHEntity::AddToVector2(const std::string &name, const Vector2 &v)
{
	return m_properties.AddToVector2(name, v);
}

Vector3 ETHEntity::AddToVector3(const std::string &name, const Vector3 &v)
{
	return m_properties.AddToVector3(name, v);
}

float ETHEntity::MultiplyFloat(const std::string &name, const float &value)
{
	return m_properties.MultiplyFloat(name, value);
}

int ETHEntity::MultiplyInt(const std::string &name, const int &value)
{
	return m_properties.MultiplyInt(name, value);
}

unsigned int ETHEntity::MultiplyUInt(const std::string &name, const unsigned int &value)
{
	return m_properties.MultiplyUInt(name, value);
}

Vector2 ETHEntity::MultiplyVector2(const std::string &name, const float &value)
{
	return m_properties.MultiplyVector2(name, value);
}

Vector3 ETHEntity::MultiplyVector3(const std::string &name, const float &value)
{
	return m_properties.MultiplyVector3(name, value);
}

void ETHEntity::InsertData(const ETHCustomDataManager &dataIn)
{
	m_properties.InsertData(dataIn);
}

void ETHEntity::MoveData(ETHCustomDataManager &dataOut) const
{
	m_properties.MoveData(dataOut);
}

bool ETHEntity::EraseData(const std::string &name)
{
	return m_properties.EraseData(name);
}

ETHCustomData::DATA_TYPE ETHEntity::CheckCustomData(const std::string &name) const
{
	return m_properties.Check(name);
}

bool ETHEntity::HasCustomData() const
{
	return m_properties.HasData();
}

void ETHEntity::DebugPrintCustomData() const
{
	std::cout << m_properties.GetDebugStringData();
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
