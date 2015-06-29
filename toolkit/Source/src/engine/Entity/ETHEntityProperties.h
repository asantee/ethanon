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
	ETH_BOOL castShadow;
	ETH_BOOL applyLight;
	str_type::string glossFile;
	str_type::string normalFile;
	str_type::string spriteFile;
	float shadowOpacity;
	float shadowScale;
	float specularPower;
	float specularBrightness;
	float restitution;
	boost::shared_ptr<ETHLight> light;
	float shadowLengthScale;
	Video::ALPHA_MODE blendMode;
	ETH_BOOL sensor;
	ETH_BOOL fixedRotation;
	ETH_BOOL bullet;
	float density, friction, gravityScale;
	ETHPolygonPtr polygon;
	ETHCompoundShapePtr compoundShape;
	str_type::string enmlJointDefinitions;
};

class ETHEntitySpriteProperties : public ETHEntityMaterial
{
public:
	Vector2 pivotAdjust;
	Vector2i spriteCut;
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
		ET_GROUND_DECAL = 1, // deprecated
		ET_VERTICAL,
		ET_OVERALL, // depracated
		ET_OPAQUE_DECAL, // deprecated
		ET_LAYERABLE
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

	ETHEntityProperties(const str_type::string& filePath, const Platform::FileManagerPtr& fileManager);
	ETHEntityProperties(TiXmlElement *pElement);
	ETHEntityProperties();
	bool IsCollidable() const;
	void Reset();
	bool SaveToFile(const str_type::string& filePath);

	bool ReadFromXMLFile(
		TiXmlElement *pElement,
		ETHEntityCache& entityCache,
		const str_type::string &entityPath,
		Platform::FileManagerPtr fileManager);
	bool ReadFromXMLFile(TiXmlElement *pElement);
	bool IsSuccessfullyLoaded() const;

	bool WriteContentToXMLFile(TiXmlElement *pHeadRoot) const;
	bool WriteEntityNameToXMLFile(TiXmlElement *pHeadRoot) const;

	static ETHParticleManager::DEPTH_SORTING_MODE ResolveDepthSortingMode(const ENTITY_TYPE type);

	static void SetBooleanPropertyToXmlElement(
		TiXmlElement *pEntity,
		const str_type::string& name,
		const ETH_BOOL value);

	static void SetBooleanPropertyToXmlElement(
		TiXmlElement *pEntity,
		const str_type::string& name,
		const ETH_BOOL value,
		const ETH_BOOL defaultValue);

	static ETH_BOOL ReadBooleanPropertyFromXmlElement(
		TiXmlElement *pEntity,
		const str_type::string& name,
		const ETH_BOOL defaultValue);

	static void ReadVector2PropertyFromXmlElement(
		TiXmlElement *pElement,
		const str_type::string& name,
		Vector2& value);

	static void ReadVector2iPropertyFromXmlElement(
		TiXmlElement *pElement,
		const str_type::string& name,
		Vector2i& value);

	static void ReadVector3PropertyFromXmlElement(
		TiXmlElement *pElement,
		const str_type::string& name,
		Vector3& value);

	static void ReadColorPropertyFromXmlElement(
		TiXmlElement *pElement,
		const str_type::string& name,
		Vector4& value);

	static void ReadColorPropertyFromXmlElement(
		TiXmlElement *pElement,
		const str_type::string& name,
		Vector3& value);

	static void SetVector2PropertyToXmlElement(
		TiXmlElement *pRoot,
		const str_type::string& name,
		const Vector2& value);

	static void SetVector2iPropertyToXmlElement(
		TiXmlElement *pRoot,
		const str_type::string& name,
		const Vector2i& value);

	static void SetVector3PropertyToXmlElement(
		TiXmlElement *pRoot,
		const str_type::string& name,
		const Vector3& value);

	static void SetColorPropertyToXmlElement(
		TiXmlElement *pRoot,
		const str_type::string& name,
		const Vector3& value);

	static void SetColorPropertyToXmlElement(
		TiXmlElement *pRoot,
		const str_type::string& name,
		const Vector4& value);

	BODY_SHAPE shape;
	str_type::string entityName;
	ETH_BOOL staticEntity;
	ETH_BOOL hideFromSceneEditor;
	ENTITY_TYPE type;
	float parallaxIntensity;
	float layerDepth;
	Vector2 scale;
	boost::shared_ptr<ETHCollisionBox> collision;
	std::vector<boost::shared_ptr<ETHParticleSystem> > particleSystems;
	bool successfullyLoaded;
};

#endif
