#include "ETHEntityProperties.h"

#include "ETHEntityCache.h"

#include "../Resource/ETHResourceProvider.h"

#include <iostream>

static const std::string COMPOUND_SHAPE_ENML_SAMPLE(
	"\n\n// this is a compound shape sample\n"
	"// shape sizes and pos must be normalized (-1 to 1)\n"
	"// the final shape actual size is defined by our collision box\n"
	"\n// Lollipop sample\n"
	"\ncandy\n"
	"{\n"
	"	shape = circle;\n"
	"	posX = 0.0;\n"
	"	posY =-0.5;\n"
	"	radius = 1.0;\n"
	"	angle = 0;\n"
	"}\n\n"
	"stick\n"
	"{\n"
	"	shape = box;\n"
	"	sizeX = 0.1;\n"
	"	sizeY = 2.0;\n"
	"	posX = 0.0;\n"
	"	posY = 0.0;\n"
	"	angle = 0;\n"
	"}\n\n");


unsigned int ETHEntitySpriteProperties::GetNumFrames() const
{
	return static_cast<unsigned int>(spriteCut.x * spriteCut.y);
}

static const std::string POLYGON_ENML_SAMPLE(
	"\n\n// this is a basic triangle sample\n"
	"// polygons must be convex and ordered clockwise"
	"\nv0\n"
	"{\n"
	"	x=1;\n"
	"	y=-1;\n"
	"}\n"
	"v1\n"
	"{\n"
	"	x=1;\n"
	"	y=1;\n"
	"}\n"
	"v2\n"
	"{\n"
	"	x=-1;\n"
	"	y=1;\n"
	"}\n");

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
	sensor = ETH_FALSE;
	fixedRotation = ETH_FALSE;
	bullet = ETH_FALSE;
	spriteFile = "";
	blendMode = Video::AM_PIXEL;
	density = 1.0f;
	friction = 1.0f;
	restitution = 0.0f;
	gravityScale = 1.0f;
}

ETHParticleManager::DEPTH_SORTING_MODE ETHEntityProperties::ResolveDepthSortingMode(const ENTITY_TYPE type)
{
	switch (type)
	{
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

ETHEntityProperties::ETHEntityProperties(const std::string& filePath, const Platform::FileManagerPtr& fileManager)
{
	ETHEntityMaterial::Reset();
	Reset();

	TiXmlDocument doc(filePath);
	std::string content;
	fileManager->GetUTFFileString(filePath, content);
	if (!doc.LoadFile(content, TIXML_ENCODING_LEGACY))
	{
		ETH_STREAM_DECL(ss) << ("Couldn't load file: ") << filePath;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_ERROR);
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	TiXmlElement *pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
	{
		ETH_STREAM_DECL(ss) << ("The current file seems to be invalid: ") << filePath;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_ERROR);
		return;
	}

	hRoot = TiXmlHandle(pElem);
	entityName = Platform::GetFileName(filePath);
	ReadFromXMLFile(hRoot.FirstChildElement().Element());
}

void ETHEntityProperties::Reset()
{
	ETHEntityMaterial::Reset();
	entityName = ("");
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

bool ETHEntityProperties::SaveToFile(const std::string& filePath)
{
	TiXmlDocument doc;
	TiXmlDeclaration *pDecl = new TiXmlDeclaration(("1.0"), (""), (""));
	doc.LinkEndChild(pDecl);

	TiXmlElement *pElement = new TiXmlElement(("Ethanon"));
	doc.LinkEndChild(pElement);

	WriteContentToXMLFile(doc.RootElement());
	doc.SaveFile(filePath);
	return true;
}

bool ETHEntityProperties::ReadFromXMLFile(
	TiXmlElement *pElement,
	ETHEntityCache& entityCache,
	const std::string &entityPath,
	Platform::FileManagerPtr fileManager)
{
	TiXmlNode *pNode = pElement->FirstChild(("FileName"));
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
	pElement->QueryIntAttribute(("type"), (int*)&type);

	staticEntity = ReadBooleanPropertyFromXmlElement(pElement, ("static"), staticEntity);
	hideFromSceneEditor = ReadBooleanPropertyFromXmlElement(pElement, ("hideFromSceneEditor"), hideFromSceneEditor);
	applyLight = ReadBooleanPropertyFromXmlElement(pElement, ("applyLight"), applyLight);

	pElement->QueryIntAttribute(("shape"), (int*)&shape);
	pElement->QueryIntAttribute(("blendMode"), (int*)&blendMode);
	pElement->QueryFloatAttribute(("layerDepth"), &layerDepth);
	pElement->QueryFloatAttribute(("parallaxIntensity"), &parallaxIntensity);

	if (shape != BS_NONE)
	{
		sensor = ReadBooleanPropertyFromXmlElement(pElement, ("sensor"), sensor);
		fixedRotation = ReadBooleanPropertyFromXmlElement(pElement, ("fixedRotation"), fixedRotation);
		bullet = ReadBooleanPropertyFromXmlElement(pElement, ("bullet"), bullet);

		pElement->QueryFloatAttribute(("friction"), &friction);
		pElement->QueryFloatAttribute(("density"), &density);
		pElement->QueryFloatAttribute(("gravityScale"), &gravityScale);
		pElement->QueryFloatAttribute(("restitution"), &restitution);
	}

	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("EmissiveColor"), emissiveColor);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("DiffuseColor"), diffuseColor);

	ReadVector2iPropertyFromXmlElement(pElement, ("SpriteCut"), spriteCut);
	ReadVector2PropertyFromXmlElement(pElement, ("Scale"), scale);
	ReadVector2PropertyFromXmlElement(pElement, ("PivotAdjust"), pivotAdjust);
	
	originalSpriteCut = spriteCut;

	TiXmlElement *pIter;
	TiXmlNode *pNode;

	pNode = pElement->FirstChild(("Sprite"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			spriteFile = pIter->GetText();
		}
	}

	pNode = pElement->FirstChild(("Particles"));
	if (pNode)
	{
		TiXmlElement *pParticles = pNode->ToElement();
		if (pParticles)
		{
			pNode = pParticles->FirstChild(("ParticleSystem"));
			if (pNode)
			{
				TiXmlElement *pParticleIter = pNode->ToElement();
				if (pParticleIter)
				{
					while (pParticleIter)
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

	pNode = pElement->FirstChild(("Light"));
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
		const int r = pElement->QueryIntAttribute(("collidable"), &nCollidable);
		if (nCollidable || r == TIXML_NO_ATTRIBUTE)
		{
			pNode = pElement->FirstChild(("Collision"));
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
					TiXmlNode *pPolygonNode = pNode->FirstChild(("Polygon"));
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
						TiXmlNode *pCompoundNode = pNode->FirstChild(("Compound"));
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
					TiXmlNode *pJointsNode = pNode->FirstChild(("Joints"));
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
	TiXmlElement *pRoot = new TiXmlElement(("Entity"));
	pHeadRoot->LinkEndChild(pRoot);
	
	TiXmlElement *pElement = new TiXmlElement(("FileName"));
	pElement->LinkEndChild(new TiXmlText(entityName));
	pRoot->LinkEndChild(pElement);
	
	WriteDataToFile(pRoot);
	return true;
}

bool ETHEntityProperties::WriteContentToXMLFile(TiXmlElement *pHeadRoot) const
{
	TiXmlElement *pRoot = new TiXmlElement(("Entity"));
	pHeadRoot->LinkEndChild(pRoot); 

	TiXmlElement *pElement;

	if (emissiveColor != ETH_DEFAULT_EMISSIVE_COLOR)
		ETHEntityProperties::SetColorPropertyToXmlElement(pRoot, ("EmissiveColor"), emissiveColor);

	if (diffuseColor != ETH_DEFAULT_DIFFUSE_COLOR)
		ETHEntityProperties::SetColorPropertyToXmlElement(pRoot, ("DiffuseColor"), diffuseColor);

	if (spriteCut != ETH_DEFAULT_SPRITE_CUT)
		ETHEntityProperties::SetVector2iPropertyToXmlElement(pRoot, ("SpriteCut"), spriteCut);

	if (scale != ETH_DEFAULT_SCALE)
		ETHEntityProperties::SetVector2PropertyToXmlElement(pRoot, ("Scale"), scale);

	if (pivotAdjust != ETH_DEFAULT_PIVOT_ADJUST)
		ETHEntityProperties::SetVector2PropertyToXmlElement(pRoot, ("PivotAdjust"), pivotAdjust);

	if (spriteFile != (""))
	{
		pElement = new TiXmlElement(("Sprite"));
		pElement->LinkEndChild(new TiXmlText(spriteFile));
		pRoot->LinkEndChild(pElement);
	}

	if (!particleSystems.empty())
	{
		TiXmlElement *pParticles = new TiXmlElement(("Particles"));
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
				pElement = new TiXmlElement(("Polygon"));
				TiXmlText* text = new TiXmlText(polygon->GetENMLDeclaration());
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}
			else if (shape == BS_POLYGON) // if the polygon data is empty, write sample data into it
			{
				pElement = new TiXmlElement(("Polygon"));
				TiXmlText* text = new TiXmlText(POLYGON_ENML_SAMPLE);
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}

			// Write compound shape data
			if (compoundShape)
			{
				pElement = new TiXmlElement(("Compound"));
				TiXmlText* text = new TiXmlText(compoundShape->GetENMLDeclaration());
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}
			else if (shape == BS_COMPOUND) // it the compound data is empty, write sample data into it
			{
				pElement = new TiXmlElement(("Compound"));
				TiXmlText* text = new TiXmlText(COMPOUND_SHAPE_ENML_SAMPLE);
				text->SetCDATA(true);
				pElement->LinkEndChild(text);
				pCollisionRoot->LinkEndChild(pElement);
			}

			// Write joint data
			if (enmlJointDefinitions != (""))
			{
				pElement = new TiXmlElement(("Joints"));
				pElement->LinkEndChild(new TiXmlText(enmlJointDefinitions));
				pCollisionRoot->LinkEndChild(pElement);
			}
		}
	}

	pRoot->SetAttribute(("shape"), shape);
	if (shape != BS_NONE)
	{
		pRoot->SetAttribute(("sensor"), sensor);
		pRoot->SetAttribute(("bullet"), bullet);
		pRoot->SetAttribute(("fixedRotation"), fixedRotation);
		pRoot->SetDoubleAttribute(("friction"), friction);
		pRoot->SetDoubleAttribute(("density"), density);
		pRoot->SetDoubleAttribute(("restitution"), restitution);
		pRoot->SetDoubleAttribute(("gravityScale"), gravityScale);
	}

	pRoot->SetAttribute(("type"), type);
	if (type == ET_LAYERABLE)
		pRoot->SetDoubleAttribute(("layerDepth"), layerDepth);

	if (parallaxIntensity != ETH_DEFAULT_PARALLAX_INTENS)
		pRoot->SetDoubleAttribute(("parallaxIntensity"), parallaxIntensity);

	if (hideFromSceneEditor != ETH_FALSE)
		pRoot->SetAttribute(("hideFromSceneEditor"), hideFromSceneEditor);

	pRoot->SetAttribute(("static"), staticEntity);
	pRoot->SetAttribute(("blendMode"), blendMode);

	WriteDataToFile(pRoot);
	return true;
}

void ETHEntityProperties::SetBooleanPropertyToXmlElement(TiXmlElement *pEntity, const std::string& name, const ETH_BOOL value)
{
	pEntity->SetAttribute(name, value);
}

void ETHEntityProperties::SetBooleanPropertyToXmlElement(
	TiXmlElement *pEntity,
	const std::string& name,
	const ETH_BOOL value,
	const ETH_BOOL defaultValue)
{
	if (value != defaultValue)
		SetBooleanPropertyToXmlElement(pEntity, name, value);
}

ETH_BOOL ETHEntityProperties::ReadBooleanPropertyFromXmlElement(
	TiXmlElement *pEntity,
	const std::string& name,
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
	const std::string& name,
	Vector2& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(("x"), &value.x);
			pIter->QueryFloatAttribute(("y"), &value.y);
		}
	}
}

void ETHEntityProperties::ReadVector2iPropertyFromXmlElement(
	TiXmlElement *pElement,
	const std::string& name,
	Vector2i& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryIntAttribute(("x"), &value.x);
			pIter->QueryIntAttribute(("y"), &value.y);
		}
	}
}

void ETHEntityProperties::ReadVector3PropertyFromXmlElement(
	TiXmlElement *pElement,
	const std::string& name,
	Vector3& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(("x"), &value.x);
			pIter->QueryFloatAttribute(("y"), &value.y);
			pIter->QueryFloatAttribute(("z"), &value.z);
		}
	}
}

void ETHEntityProperties::ReadColorPropertyFromXmlElement(
	TiXmlElement *pElement,
	const std::string& name,
	Vector4& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(("r"), &value.x);
			pIter->QueryFloatAttribute(("g"), &value.y);
			pIter->QueryFloatAttribute(("b"), &value.z);
			pIter->QueryFloatAttribute(("a"), &value.w);
		}
	}
}

void ETHEntityProperties::ReadColorPropertyFromXmlElement(
	TiXmlElement *pElement,
	const std::string& name,
	Vector3& value)
{
	TiXmlNode *pNode = pElement->FirstChild(name);
	if (pNode)
	{
		TiXmlElement *pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(("r"), &value.x);
			pIter->QueryFloatAttribute(("g"), &value.y);
			pIter->QueryFloatAttribute(("b"), &value.z);
		}
	}
}

void ETHEntityProperties::SetVector2PropertyToXmlElement(
	TiXmlElement *pRoot,
	const std::string& name,
	const Vector2& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(("x"), value.x);
	pElement->SetDoubleAttribute(("y"), value.y);
}

void ETHEntityProperties::SetVector2iPropertyToXmlElement(
	TiXmlElement *pRoot,
	const std::string& name,
	const Vector2i& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetAttribute(("x"), value.x);
	pElement->SetAttribute(("y"), value.y);
}

void ETHEntityProperties::SetVector3PropertyToXmlElement(
	TiXmlElement *pRoot,
	const std::string& name,
	const Vector3& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(("x"), value.x);
	pElement->SetDoubleAttribute(("y"), value.y);
	pElement->SetDoubleAttribute(("z"), value.z);
}

void ETHEntityProperties::SetColorPropertyToXmlElement(
	TiXmlElement *pRoot,
	const std::string& name,
	const Vector3& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(("r"), value.x);
	pElement->SetDoubleAttribute(("g"), value.y);
	pElement->SetDoubleAttribute(("b"), value.z);
}

void ETHEntityProperties::SetColorPropertyToXmlElement(
	TiXmlElement *pRoot,
	const std::string& name,
	const Vector4& value)
{
	TiXmlElement* pElement = new TiXmlElement(name);
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(("r"), value.x);
	pElement->SetDoubleAttribute(("g"), value.y);
	pElement->SetDoubleAttribute(("b"), value.z);
	pElement->SetDoubleAttribute(("a"), value.w);
}

bool ETHEntityProperties::operator < (const ETHEntityProperties& other) const
{
	return (entityName < other.entityName);
}
