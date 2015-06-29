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

#include "ETHEntityProperties.h"

#include "ETHEntityCache.h"

#include "../Resource/ETHResourceProvider.h"

#include <iostream>

static const str_type::string COMPOUND_SHAPE_ENML_SAMPLE(
	GS_L("\n\n// this is a compound shape sample\n")
	GS_L("// shape sizes and pos must be normalized (-1 to 1)\n")
	GS_L("// the final shape actual size is defined by our collision box\n")
	GS_L("\n// Lollipop sample\n")
	GS_L("\ncandy\n")
	GS_L("{\n")
	GS_L("	shape = circle;\n")
	GS_L("	posX = 0.0;\n")
	GS_L("	posY =-0.5;\n")
	GS_L("	radius = 1.0;\n")
	GS_L("	angle = 0;\n")
	GS_L("}\n\n")
	GS_L("stick\n")
	GS_L("{\n")
	GS_L("	shape = box;\n")
	GS_L("	sizeX = 0.1;\n")
	GS_L("	sizeY = 2.0;\n")
	GS_L("	posX = 0.0;\n")
	GS_L("	posY = 0.0;\n")
	GS_L("	angle = 0;\n")
	GS_L("}\n\n"));


unsigned int ETHEntitySpriteProperties::GetNumFrames() const
{
	return static_cast<unsigned int>(spriteCut.x * spriteCut.y);
}

static const str_type::string POLYGON_ENML_SAMPLE(
	GS_L("\n\n// this is a basic triangle sample\n")
	GS_L("// polygons must be convex and ordered clockwise")
	GS_L("\nv0\n")
	GS_L("{\n")
	GS_L("	x=1;\n")
	GS_L("	y=-1;\n")
	GS_L("}\n")
	GS_L("v1\n")
	GS_L("{\n")
	GS_L("	x=1;\n")
	GS_L("	y=1;\n")
	GS_L("}\n")
	GS_L("v2\n")
	GS_L("{\n")
	GS_L("	x=-1;\n")
	GS_L("	y=1;\n")
	GS_L("}\n"));

#define ETH_DEFAULT_EMISSIVE_COLOR	Vector4(0, 0, 0, 0)
#define ETH_DEFAULT_DIFFUSE_COLOR	Vector4(1, 1, 1, 1)
#define ETH_DEFAULT_SPRITE_CUT		Vector2i(1, 1)
#define ETH_DEFAULT_PIVOT_ADJUST	Vector2(0, 0)
#define ETH_DEFAULT_SCALE			Vector2(1, 1)
#define ETH_DEFAULT_PARALLAX_INTENS	1.0f

void ETHEntityMaterial::Reset()
{
	emissiveColor = ETH_DEFAULT_EMISSIVE_COLOR;
	diffuseColor = ETH_DEFAULT_DIFFUSE_COLOR;
	castShadow = ETH_FALSE;
	applyLight = ETH_FALSE;
	sensor = ETH_FALSE;
	fixedRotation = ETH_FALSE;
	bullet = ETH_FALSE;
	spriteFile = GS_L("");
	normalFile = GS_L("");
	glossFile = GS_L("");
	shadowScale = 0.0f;
	shadowOpacity = 1.0f;
	specularPower = 50.0f;
	specularBrightness = 1.0f;
	shadowLengthScale = 1.0f;
	blendMode = Video::AM_PIXEL;
	density = 1.0f;
	friction = 1.0f;
	restitution = 0.0f;
	gravityScale = 1.0f;
}

ETHParticleManager::DEPTH_SORTING_MODE ETHEntityProperties::ResolveDepthSortingMode( const ENTITY_TYPE type )
{
	switch (type)
	{
	case ET_VERTICAL:
		return ETHParticleManager::INDIVIDUAL_OFFSET;
		break;
	case ET_LAYERABLE:
		return ETHParticleManager::LAYERABLE;
		break;
	default:
		return ETHParticleManager::SAME_DEPTH_AS_OWNER;
	}
}

ETHEntityProperties::ETHEntityProperties()
{
	ETHEntityMaterial::Reset();
	Reset();
}

ETHEntityProperties::ETHEntityProperties(TiXmlElement *pElement)
{
	ETHEntityMaterial::Reset();
	Reset();
	ReadFromXMLFile(pElement);
}

ETHEntityProperties::ETHEntityProperties(const str_type::string& filePath, const Platform::FileManagerPtr& fileManager)
{
	ETHEntityMaterial::Reset();
	Reset();

	TiXmlDocument doc(filePath);
	str_type::string content;
	fileManager->GetUTFFileString(filePath, content);
	if (!doc.LoadFile(content, TIXML_ENCODING_LEGACY))
	{
		ETH_STREAM_DECL(ss) << GS_L("Couldn't load file: ") << filePath;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	TiXmlElement *pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		ETH_STREAM_DECL(ss) << GS_L("The current file seems to be invalid: ") << filePath;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		return;
	}

	hRoot = TiXmlHandle(pElem);
	entityName = Platform::GetFileName(filePath);
	ReadFromXMLFile(hRoot.FirstChildElement().Element());
}

void ETHEntityProperties::Reset()
{
	ETHEntityMaterial::Reset();
	entityName = GS_L("");
	spriteCut = ETH_DEFAULT_SPRITE_CUT;
	pivotAdjust = ETH_DEFAULT_PIVOT_ADJUST;
	scale = ETH_DEFAULT_SCALE;
	staticEntity = ETH_FALSE;
	hideFromSceneEditor = ETH_FALSE;
	type = ET_HORIZONTAL;
	layerDepth = 0.0f;
	successfullyLoaded = false;
	parallaxIntensity = ETH_DEFAULT_PARALLAX_INTENS;
	shape = BS_NONE;
}

bool ETHEntityProperties::IsSuccessfullyLoaded() const
{
	return successfullyLoaded;
}

bool ETHEntityProperties::SaveToFile(const str_type::string& filePath)
{
	TiXmlDocument doc;
	TiXmlDeclaration *pDecl = new TiXmlDeclaration(GS_L("1.0"), GS_L(""), GS_L(""));
	doc.LinkEndChild(pDecl);

	TiXmlElement *pElement = new TiXmlElement(GS_L("Ethanon"));
	doc.LinkEndChild(pElement);

	WriteContentToXMLFile(doc.RootElement());
	doc.SaveFile(filePath);
	return true;
}

bool ETHEntityProperties::ReadFromXMLFile(
	TiXmlElement *pElement,
	ETHEntityCache& entityCache,
	const str_type::string &entityPath,
	Platform::FileManagerPtr fileManager)
{
	TiXmlNode *pNode = pElement->FirstChild(GS_L("FileName"));
	if (pNode)
	{
		const ETHEntityProperties* props = entityCache.Get(entityName, entityPath, fileManager);
		if (props)
		{
			*this = *props;
			ReadDataFromXMLFile(pElement);
			return (successfullyLoaded = true);
		}
	}
	else
	{
		return ReadFromXMLFile(pElement);
	}
	return false;
}

bool ETHEntityProperties::ReadFromXMLFile(TiXmlElement *pElement)
{
	pElement->QueryIntAttribute(GS_L("type"), (int*)&type);

	staticEntity = ReadBooleanPropertyFromXmlElement(pElement, GS_L("static"), staticEntity);
	applyLight = ReadBooleanPropertyFromXmlElement(pElement, GS_L("applyLight"), applyLight);
	castShadow = ReadBooleanPropertyFromXmlElement(pElement, GS_L("castShadow"), castShadow);
	hideFromSceneEditor = ReadBooleanPropertyFromXmlElement(pElement, GS_L("hideFromSceneEditor"), hideFromSceneEditor);

	pElement->QueryIntAttribute(GS_L("shape"), (int*)&shape);
	pElement->QueryIntAttribute(GS_L("blendMode"), (int*)&blendMode);
	pElement->QueryFloatAttribute(GS_L("layerDepth"), &layerDepth);
	pElement->QueryFloatAttribute(GS_L("parallaxIntensity"), &parallaxIntensity);

	if (applyLight)
	{
		pElement->QueryFloatAttribute(GS_L("specularPower"), &specularPower);
		pElement->QueryFloatAttribute(GS_L("specularBrightness"), &specularBrightness);
	}

	if (castShadow)
	{
		pElement->QueryFloatAttribute(GS_L("shadowScale"), &shadowScale);
		pElement->QueryFloatAttribute(GS_L("shadowLengthScale"), &shadowLengthScale);
		pElement->QueryFloatAttribute(GS_L("shadowOpacity"), &shadowOpacity);
	}

	if (shape != BS_NONE)
	{
		sensor = ReadBooleanPropertyFromXmlElement(pElement, GS_L("sensor"), sensor);
		fixedRotation = ReadBooleanPropertyFromXmlElement(pElement, GS_L("fixedRotation"), fixedRotation);
		bullet = ReadBooleanPropertyFromXmlElement(pElement, GS_L("bullet"), bullet);

		pElement->QueryFloatAttribute(GS_L("friction"), &friction);
		pElement->QueryFloatAttribute(GS_L("density"), &density);
		pElement->QueryFloatAttribute(GS_L("gravityScale"), &gravityScale);
		pElement->QueryFloatAttribute(GS_L("restitution"), &restitution);
	}

	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("EmissiveColor"), emissiveColor);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("DiffuseColor"), diffuseColor);

	ReadVector2iPropertyFromXmlElement(pElement, GS_L("SpriteCut"), spriteCut);
	ReadVector2PropertyFromXmlElement(pElement, GS_L("Scale"), scale);
	ReadVector2PropertyFromXmlElement(pElement, GS_L("PivotAdjust"), pivotAdjust);

	TiXmlElement *pIter;
	TiXmlNode *pNode;

	pNode = pElement->FirstChild(GS_L("Sprite"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			spriteFile = pIter->GetText();
		}
	}

	pNode = pElement->FirstChild(GS_L("Normal"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			normalFile = pIter->GetText();
		}
	}

	pNode = pElement->FirstChild(GS_L("Gloss"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			glossFile = pIter->GetText();
		}
	}

	pNode = pElement->FirstChild(GS_L("Particles"));
	if (pNode)
	{
		TiXmlElement *pParticles = pNode->ToElement();
		if (pParticles)
		{
			pNode = pParticles->FirstChild(GS_L("ParticleSystem"));
			if (pNode)
			{
				TiXmlElement *pParticleIter = pNode->ToElement();
				if (pParticleIter)
				{
					// TODO: use unlimited amount of systems 
					for (std::size_t t = 0; t < ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
					{
						if (pParticleIter)
						{
							boost::shared_ptr<ETHParticleSystem> newSystem = boost::shared_ptr<ETHParticleSystem>(new ETHParticleSystem);
							newSystem->ReadFromXMLFile(pParticleIter);
							if (newSystem->nParticles > 0)
							{
								particleSystems.push_back(newSystem);
							}
							pParticleIter = pParticleIter->NextSiblingElement();
						}
					}
				}
			}
		}
	}

	pNode = pElement->FirstChild(GS_L("Light"));
	if (pNode)
	{
		TiXmlElement *pLight = pNode->ToElement();
		if (pLight)
		{
			boost::shared_ptr<ETHLight> newLight = boost::shared_ptr<ETHLight>(new ETHLight(true));
			newLight->ReadFromXMLFile(pLight);
			if (newLight->IsActive() && newLight->range > 0.0f)
			{
				light = newLight;
				// forces the light 'static' flag to be the same as its owner's
				light->staticLight = staticEntity;
			}
		}
	}

	{
		int nCollidable = 1;
		const int r = pElement->QueryIntAttribute(GS_L("collidable"), &nCollidable);
		if (nCollidable || r == TIXML_NO_ATTRIBUTE)
		{
			pNode = pElement->FirstChild(GS_L("Collision"));
			if (pNode)
			{
				collision = boost::shared_ptr<ETHCollisionBox>(new ETHCollisionBox);
				TiXmlElement *pCollision = pNode->ToElement();
				if (pCollision)
				{
					collision->ReadFromXMLFile(pCollision);
				}

				// if it's a legacy entity, it may have a collision body in its XML even though it is not collidable
				if (collision->size == Vector3(0, 0, 0))
				{
					collision.reset();
				}
				
				{
					TiXmlNode *pPolygonNode = pNode->FirstChild(GS_L("Polygon"));
					if (pPolygonNode)
					{
						pIter = pPolygonNode->ToElement();
						if (pIter)
						{
							polygon = ETHPolygonPtr(new ETHPolygon(pIter->GetText()));
						}
					}
					else
					{
						TiXmlNode *pCompoundNode = pNode->FirstChild(GS_L("Compound"));
						if (pCompoundNode)
						{
							pIter = pCompoundNode->ToElement();
							if (pIter)
							{
								compoundShape = ETHCompoundShapePtr(new ETHCompoundShape(pIter->GetText()));
							}
						}
					}
				}
				{
					TiXmlNode *pJointsNode = pNode->FirstChild(GS_L("Joints"));
					if (pJointsNode)
					{
						pIter = pJointsNode->ToElement();
						if (pIter)
						{
							enmlJointDefinitions = pIter->GetText();
						}
					}

				}
			}
		}
	}
	ReadDataFromXMLFile(pElement);
	return (successfullyLoaded = true);
}

bool ETHEntityProperties::WriteEntityNameToXMLFile(TiXmlElement *pHeadRoot) const
{
	TiXmlElement *pRoot = new TiXmlElement(GS_L("Entity"));
	pHeadRoot->LinkEndChild(pRoot);
	
	TiXmlElement *pElement = new TiXmlElement(GS_L("FileName"));
	pElement->LinkEndChild(new TiXmlText(entityName));
	pRoot->LinkEndChild(pElement);
	
	WriteDataToFile(pRoot);
	return true;
}

bool ETHEntityProperties::WriteContentToXMLFile(TiXmlElement *pHeadRoot) const
{
	TiXmlElement *pRoot = new TiXmlElement(GS_L("Entity"));
	pHeadRoot->LinkEndChild(pRoot); 

	TiXmlElement *pElement;

	if (emissiveColor != ETH_DEFAULT_EMISSIVE_COLOR)
		ETHEntityProperties::SetColorPropertyToXmlElement(pRoot, GS_L("EmissiveColor"), emissiveColor);

	if (diffuseColor != ETH_DEFAULT_DIFFUSE_COLOR)
		ETHEntityProperties::SetColorPropertyToXmlElement(pRoot, GS_L("DiffuseColor"), diffuseColor);

	if (spriteCut != ETH_DEFAULT_SPRITE_CUT)
		ETHEntityProperties::SetVector2iPropertyToXmlElement(pRoot, GS_L("SpriteCut"), spriteCut);

	if (scale != ETH_DEFAULT_SCALE)
		ETHEntityProperties::SetVector2PropertyToXmlElement(pRoot, GS_L("Scale"), scale);

	if (pivotAdjust != ETH_DEFAULT_PIVOT_ADJUST)
		ETHEntityProperties::SetVector2PropertyToXmlElement(pRoot, GS_L("PivotAdjust"), pivotAdjust);

	if (spriteFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("Sprite"));
		pElement->LinkEndChild(new TiXmlText(spriteFile));
		pRoot->LinkEndChild(pElement);
	}

	if (normalFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("Normal"));
		pElement->LinkEndChild(new TiXmlText(normalFile));
		pRoot->LinkEndChild(pElement);
	}

	if (glossFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("Gloss"));
		pElement->LinkEndChild(new TiXmlText(glossFile));
		pRoot->LinkEndChild(pElement);
	}

	if (!particleSystems.empty())
	{
		TiXmlElement *pParticles = new TiXmlElement(GS_L("Particles"));
		pRoot->LinkEndChild(pParticles);
		for (unsigned int t=0; t<particleSystems.size(); t++)
		{
			if (particleSystems[t]->nParticles > 0)
				particleSystems[t]->WriteToXMLFile(pParticles);
		}
	}

	if (light)
	{
		light->WriteToXMLFile(pRoot);
	}

	if (collision)
	{
		TiXmlElement *pCollisionRoot = collision->WriteToXMLFile(pRoot);
		if (pCollisionRoot)
		{
			// Write polygon data
			if (polygon)
			{
				pElement = new TiXmlElement(GS_L("Polygon"));
				TiXmlText* text = new TiXmlText(polygon->GetENMLDeclaration());
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}
			else if (shape == BS_POLYGON) // if the polygon data is empty, write sample data into it
			{
				pElement = new TiXmlElement(GS_L("Polygon"));
				TiXmlText* text = new TiXmlText(POLYGON_ENML_SAMPLE);
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}

			// Write compound shape data
			if (compoundShape)
			{
				pElement = new TiXmlElement(GS_L("Compound"));
				TiXmlText* text = new TiXmlText(compoundShape->GetENMLDeclaration());
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}
			else if (shape == BS_COMPOUND) // it the compound data is empty, write sample data into it
			{
				pElement = new TiXmlElement(GS_L("Compound"));
				TiXmlText* text = new TiXmlText(COMPOUND_SHAPE_ENML_SAMPLE);
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}

			// Write joint data
			if (enmlJointDefinitions != GS_L(""))
			{
				pElement = new TiXmlElement(GS_L("Joints"));
				pElement->LinkEndChild(new TiXmlText(enmlJointDefinitions));
				pCollisionRoot->LinkEndChild(pElement);
			}
		}
	}

	pRoot->SetAttribute(GS_L("shape"), shape);
	if (shape != BS_NONE)
	{
		pRoot->SetAttribute(GS_L("sensor"), sensor);
		pRoot->SetAttribute(GS_L("bullet"), bullet);
		pRoot->SetAttribute(GS_L("fixedRotation"), fixedRotation);
		pRoot->SetDoubleAttribute(GS_L("friction"), friction);
		pRoot->SetDoubleAttribute(GS_L("density"), density);
		pRoot->SetDoubleAttribute(GS_L("restitution"), restitution);
		pRoot->SetDoubleAttribute(GS_L("gravityScale"), gravityScale);
	}

	pRoot->SetAttribute(GS_L("applyLight"), applyLight);
	if (applyLight)
	{
		pRoot->SetDoubleAttribute(GS_L("specularPower"), specularPower);
		pRoot->SetDoubleAttribute(GS_L("specularBrightness"), specularBrightness);
	}

	pRoot->SetAttribute(GS_L("castShadow"), castShadow);
	if (castShadow)
	{
		pRoot->SetDoubleAttribute(GS_L("shadowScale"), shadowScale);
		pRoot->SetDoubleAttribute(GS_L("shadowLengthScale"), shadowLengthScale);
		pRoot->SetDoubleAttribute(GS_L("shadowOpacity"), shadowOpacity);
	}

	pRoot->SetAttribute(GS_L("type"), type);
	if (type == ET_LAYERABLE)
		pRoot->SetDoubleAttribute(GS_L("layerDepth"), layerDepth);

	if (parallaxIntensity != ETH_DEFAULT_PARALLAX_INTENS)
		pRoot->SetDoubleAttribute(GS_L("parallaxIntensity"), parallaxIntensity);

	if (hideFromSceneEditor != ETH_FALSE)
		pRoot->SetAttribute(GS_L("hideFromSceneEditor"), hideFromSceneEditor);

	pRoot->SetAttribute(GS_L("static"), staticEntity);
	pRoot->SetAttribute(GS_L("blendMode"), blendMode);

	WriteDataToFile(pRoot);
	return true;
}

void ETHEntityProperties::SetBooleanPropertyToXmlElement(TiXmlElement *pEntity, const str_type::string& name, const ETH_BOOL value)
{
	pEntity->SetAttribute(name, value);
}

void ETHEntityProperties::SetBooleanPropertyToXmlElement(
	TiXmlElement *pEntity,
	const str_type::string& name,
	const ETH_BOOL value,
	const ETH_BOOL defaultValue)
{
	if (value != defaultValue)
		SetBooleanPropertyToXmlElement(pEntity, name, value);
}

ETH_BOOL ETHEntityProperties::ReadBooleanPropertyFromXmlElement(
	TiXmlElement *pEntity,
	const str_type::string& name,
	const ETH_BOOL defaultValue)
{
	int value = 0;
	if (pEntity->QueryIntAttribute(name, &value) == TIXML_SUCCESS)
		return (!value) ? ETH_FALSE : ETH_TRUE;
	else
		return defaultValue;
}

void ETHEntityProperties::ReadVector2PropertyFromXmlElement(
	TiXmlElement *pElement,
	const str_type::string& name,
	Vector2& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &value.x);
			pIter->QueryFloatAttribute(GS_L("y"), &value.y);
		}
	}
}

void ETHEntityProperties::ReadVector2iPropertyFromXmlElement(
	TiXmlElement *pElement,
	const str_type::string& name,
	Vector2i& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryIntAttribute(GS_L("x"), &value.x);
			pIter->QueryIntAttribute(GS_L("y"), &value.y);
		}
	}
}

void ETHEntityProperties::ReadVector3PropertyFromXmlElement(
	TiXmlElement *pElement,
	const str_type::string& name,
	Vector3& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &value.x);
			pIter->QueryFloatAttribute(GS_L("y"), &value.y);
			pIter->QueryFloatAttribute(GS_L("z"), &value.z);
		}
	}
}

void ETHEntityProperties::ReadColorPropertyFromXmlElement(
	TiXmlElement *pElement,
	const str_type::string& name,
	Vector4& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &value.x);
			pIter->QueryFloatAttribute(GS_L("g"), &value.y);
			pIter->QueryFloatAttribute(GS_L("b"), &value.z);
			pIter->QueryFloatAttribute(GS_L("a"), &value.w);
		}
	}
}

void ETHEntityProperties::ReadColorPropertyFromXmlElement(
	TiXmlElement *pElement,
	const str_type::string& name,
	Vector3& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &value.x);
			pIter->QueryFloatAttribute(GS_L("g"), &value.y);
			pIter->QueryFloatAttribute(GS_L("b"), &value.z);
		}
	}
}

void ETHEntityProperties::SetVector2PropertyToXmlElement(
	TiXmlElement *pRoot,
	const str_type::string& name,
	const Vector2& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), value.x);
	pElement->SetDoubleAttribute(GS_L("y"), value.y);
}

void ETHEntityProperties::SetVector2iPropertyToXmlElement(
	TiXmlElement *pRoot,
	const str_type::string& name,
	const Vector2i& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetAttribute(GS_L("x"), value.x);
	pElement->SetAttribute(GS_L("y"), value.y);
}

void ETHEntityProperties::SetVector3PropertyToXmlElement(
	TiXmlElement *pRoot,
	const str_type::string& name,
	const Vector3& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), value.x);
	pElement->SetDoubleAttribute(GS_L("y"), value.y);
	pElement->SetDoubleAttribute(GS_L("z"), value.z);
}

void ETHEntityProperties::SetColorPropertyToXmlElement(
	TiXmlElement *pRoot,
	const str_type::string& name,
	const Vector3& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), value.x);
	pElement->SetDoubleAttribute(GS_L("g"), value.y);
	pElement->SetDoubleAttribute(GS_L("b"), value.z);
}

void ETHEntityProperties::SetColorPropertyToXmlElement(
	TiXmlElement *pRoot,
	const str_type::string& name,
	const Vector4& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), value.x);
	pElement->SetDoubleAttribute(GS_L("g"), value.y);
	pElement->SetDoubleAttribute(GS_L("b"), value.z);
	pElement->SetDoubleAttribute(GS_L("a"), value.w);
}
