#ifndef ETH_ENTITY_PROPERTIES_H_
#define ETH_ENTITY_PROPERTIES_H_

#include "ETHLight.h"

#include "../Particles/ETHParticleManager.h"

#include "ETHCustomDataManager.h"

#include "../Physics/ETHPolygon.h"
#include "../Physics/ETHCompoundShape.h"
#include "../Physics/ETHCollisionBox.h"

class ETHEntityMaterial
{
public:
	void Reset();
	Vector4 emissiveColor;
	Vector4 diffuseColor;
	std::string spriteFile;
	float restitution;
	boost::shared_ptr<ETHLight> light;
	Video::ALPHA_MODE blendMode;
	ETH_BOOL sensor;
	ETH_BOOL fixedRotation;
	ETH_BOOL bullet;
	float density, friction, gravityScale;
	ETHPolygonPtr polygon;
	ETHCompoundShapePtr compoundShape;
	std::string enmlJointDefinitions;
};

class ETHEntitySpriteProperties : public ETHEntityMaterial
{
public:
	Vector2 pivotAdjust;
	mutable Vector2i spriteCut;
	mutable Vector2i originalSpriteCut;
	unsigned int GetNumFrames() const;
};

class ETHEntityCache;

class ETHEntityProperties : public ETHEntitySpriteProperties, public ETHCustomDataManager
{
	friend class ETHEntity;
	friend class ETHSpriteEntity;
	friend class ETHRenderEntity;
	friend class ETHScene;
	friend class ETHLightmapGen;

public:
	enum ENTITY_TYPE
	{
		ET_HORIZONTAL = 0,
		ET_LAYERABLE = 5
	};

	enum BODY_SHAPE
	{
		BS_NONE = 0,
		BS_BOX = 1,
		BS_CIRCLE = 2,
		BS_POLYGON = 3,
		BS_COMPOUND = 4
	};

	struct VIEW_RECT
	{
		Vector2 min, max;
	};

	ETHEntityProperties(const std::string& filePath, const Platform::FileManagerPtr& fileManager);
	ETHEntityProperties(TiXmlElement *pElement);
	ETHEntityProperties();
	bool IsCollidable() const;
	void Reset();
	bool SaveToFile(const std::string& filePath);

	bool ReadFromXMLFile(
		TiXmlElement *pElement,
		ETHEntityCache& entityCache,
		const std::string &entityPath,
		Platform::FileManagerPtr fileManager);
	bool ReadFromXMLFile(TiXmlElement *pElement);
	bool IsSuccessfullyLoaded() const;

	bool WriteContentToXMLFile(TiXmlElement *pHeadRoot) const;
	bool WriteEntityNameToXMLFile(TiXmlElement *pHeadRoot) const;

	static ETHParticleManager::DEPTH_SORTING_MODE ResolveDepthSortingMode(const ENTITY_TYPE type);

	static void SetBooleanPropertyToXmlElement(
		TiXmlElement *pEntity,
		const std::string& name,
		const ETH_BOOL value);

	static void SetBooleanPropertyToXmlElement(
		TiXmlElement *pEntity,
		const std::string& name,
		const ETH_BOOL value,
		const ETH_BOOL defaultValue);

	static ETH_BOOL ReadBooleanPropertyFromXmlElement(
		TiXmlElement *pEntity,
		const std::string& name,
		const ETH_BOOL defaultValue);

	static void ReadVector2PropertyFromXmlElement(
		TiXmlElement *pElement,
		const std::string& name,
		Vector2& value);

	static void ReadVector2iPropertyFromXmlElement(
		TiXmlElement *pElement,
		const std::string& name,
		Vector2i& value);

	static void ReadVector3PropertyFromXmlElement(
		TiXmlElement *pElement,
		const std::string& name,
		Vector3& value);

	static void ReadColorPropertyFromXmlElement(
		TiXmlElement *pElement,
		const std::string& name,
		Vector4& value);

	static void ReadColorPropertyFromXmlElement(
		TiXmlElement *pElement,
		const std::string& name,
		Vector3& value);

	static void SetVector2PropertyToXmlElement(
		TiXmlElement *pRoot,
		const std::string& name,
		const Vector2& value);

	static void SetVector2iPropertyToXmlElement(
		TiXmlElement *pRoot,
		const std::string& name,
		const Vector2i& value);

	static void SetVector3PropertyToXmlElement(
		TiXmlElement *pRoot,
		const std::string& name,
		const Vector3& value);

	static void SetColorPropertyToXmlElement(
		TiXmlElement *pRoot,
		const std::string& name,
		const Vector3& value);

	static void SetColorPropertyToXmlElement(
		TiXmlElement *pRoot,
		const std::string& name,
		const Vector4& value);

	BODY_SHAPE shape;
	std::string entityName;
	ETH_BOOL staticEntity;
	ETH_BOOL hideFromSceneEditor;
	ENTITY_TYPE type;
	float parallaxIntensity;
	float layerDepth;
	Vector2 scale;
	boost::shared_ptr<ETHCollisionBox> collision;
	std::vector<boost::shared_ptr<ETHParticleSystem> > particleSystems;
	bool successfullyLoaded;
	
	bool operator < (const ETHEntityProperties& other) const;
};

#endif
