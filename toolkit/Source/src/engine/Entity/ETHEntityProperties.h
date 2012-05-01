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

#ifndef ETH_ENTITY_PROPERTIES_H_
#define ETH_ENTITY_PROPERTIES_H_

#include "ETHLight.h"
#include "../ETHParticleManager.h"
#include "ETHCustomDataManager.h"
#include "../Physics/ETHPolygon.h"
#include "../Physics/ETHCompoundShape.h"
#include "../Physics/ETHCollisionBox.h"

class ETHEntityMaterial
{
public:
	void Reset();
	Vector4 emissiveColor;
	ETH_BOOL castShadow;
	ETH_BOOL applyLight;
	ETH_BOOL sensor;
	ETH_BOOL fixedRotation;
	ETH_BOOL bullet;
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
	GS_ALPHA_MODE blendMode;
	ETH_BODY_SHAPE shape;
	float density, friction, gravityScale;
	ETHPolygonPtr polygon;
	ETHCompoundShapePtr compoundShape;
	str_type::string enmlJointDefinitions;
};

class ETHEntitySpriteProperties : public ETHEntityMaterial
{
public:
	int startFrame;
	Vector2 pivotAdjust;
	Vector2i spriteCut;
};

class ETHEntityProperties : public ETHEntitySpriteProperties, public ETHCustomDataManager
{
	friend class ETHEntity;
	friend class ETHSpriteEntity;
	friend class ETHRenderEntity;
	friend class ETHScene;
	friend class ETHLightmapGen;
public:
	ETHEntityProperties(const str_type::string& filePath, const Platform::FileManagerPtr& fileManager);
	ETHEntityProperties(TiXmlElement *pElement);
	ETHEntityProperties();
	bool IsCollidable() const;
	void Reset();
	bool SaveToFile(const str_type::string& filePath);
	bool ReadFromXMLFile(TiXmlElement *pElement);
	bool WriteToXMLFile(TiXmlElement *pHeadRoot) const;
	bool IsSuccessfullyLoaded() const;

#if !defined(_ETHANON_EDITOR) && !defined(_ETH_MAKE_PROPS_ALL_PUBLIC)
protected:
#endif
	str_type::string entityName;
	ETH_BOOL staticEntity;
	ETH_ENTITY_TYPE type;
	float parallaxIntensity;
	float layerDepth;
	float soundVolume;
	Vector2 scale;
	boost::shared_ptr<ETHCollisionBox> collision;
	std::vector<boost::shared_ptr<ETH_PARTICLE_SYSTEM> > particleSystems;
	bool successfullyLoaded;
};

#endif