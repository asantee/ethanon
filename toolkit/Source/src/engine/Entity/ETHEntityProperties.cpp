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

#include "ETHEntityProperties.h"
#include <iostream>

void ETHEntityMaterial::Reset()
{
	emissiveColor = Vector4(0,0,0,0);
	castShadow = ETH_FALSE;
	applyLight = ETH_TRUE;
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
	blendMode = GSAM_PIXEL;
	shape = ETHBS_NONE;
	density = 0.0f;
	friction = 1.0f;
	restitution = 0.0f;
	gravityScale = 1.0f;
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
	fileManager->GetUTF16FileString(filePath, content);
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
	entityName = ETHGlobal::GetFileName(filePath);
	if (ReadFromXMLFile(hRoot.FirstChildElement().Element()))
	{
		successfullyLoaded = true;
	}
}

void ETHEntityProperties::Reset()
{
	ETHEntityMaterial::Reset();
	entityName = GS_L("");
	spriteCut = Vector2i(1,1);
	pivotAdjust = Vector2(0,0);
	startFrame = 0;
	staticEntity = false;
	type = ETH_HORIZONTAL;
	soundVolume = 1.0f;
	layerDepth = 0.0f;
	successfullyLoaded = false;
	scale = Vector2(1, 1);
	parallaxIntensity = 1.0f;
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

	WriteToXMLFile(doc.RootElement());
	doc.SaveFile(filePath);
	return true;
}

bool ETHEntityProperties::ReadFromXMLFile(TiXmlElement *pElement)
{
	pElement->QueryIntAttribute(GS_L("type"), (int*)&type);

	int nStaticEntity = static_cast<int>(staticEntity);
	pElement->QueryIntAttribute(GS_L("static"), &nStaticEntity);
	staticEntity = static_cast<ETH_BOOL>(nStaticEntity);

	int nApplyLight = static_cast<int>(applyLight);
	pElement->QueryIntAttribute(GS_L("applyLight"), &nApplyLight);
	applyLight = static_cast<ETH_BOOL>(nApplyLight);

	int nCastShadow = static_cast<int>(castShadow);
	pElement->QueryIntAttribute(GS_L("castShadow"), &nCastShadow);
	castShadow = static_cast<ETH_BOOL>(nCastShadow);

	int nSensor = static_cast<int>(sensor);
	pElement->QueryIntAttribute(GS_L("sensor"), &nSensor);
	sensor = static_cast<ETH_BOOL>(nSensor);

	int nFixedRotation = static_cast<int>(fixedRotation);
	pElement->QueryIntAttribute(GS_L("fixedRotation"), &nFixedRotation);
	fixedRotation = static_cast<ETH_BOOL>(nFixedRotation);

	int nBullet = static_cast<int>(bullet);
	pElement->QueryIntAttribute(GS_L("bullet"), &nBullet);
	bullet = static_cast<ETH_BOOL>(nBullet);

	pElement->QueryIntAttribute(GS_L("shape"), (int*)&shape);
	pElement->QueryIntAttribute(GS_L("startFrame"), &startFrame);
	pElement->QueryIntAttribute(GS_L("blendMode"), (int*)&blendMode);
	pElement->QueryFloatAttribute(GS_L("friction"), &friction);
	pElement->QueryFloatAttribute(GS_L("density"), &density);
	pElement->QueryFloatAttribute(GS_L("layerDepth"), &layerDepth);
	pElement->QueryFloatAttribute(GS_L("soundVolume"), &soundVolume);
	pElement->QueryFloatAttribute(GS_L("shadowScale"), &shadowScale);
	pElement->QueryFloatAttribute(GS_L("shadowLengthScale"), &shadowLengthScale);
	pElement->QueryFloatAttribute(GS_L("shadowOpacity"), &shadowOpacity);
	pElement->QueryFloatAttribute(GS_L("specularPower"), &specularPower);
	pElement->QueryFloatAttribute(GS_L("specularBrightness"), &specularBrightness);
	pElement->QueryFloatAttribute(GS_L("restitution"), &restitution);
	pElement->QueryFloatAttribute(GS_L("parallaxIntensity"), &parallaxIntensity);
	pElement->QueryFloatAttribute(GS_L("gravityScale"), &gravityScale);

	TiXmlElement *pIter;
	TiXmlNode *pNode;

	pNode = pElement->FirstChild(GS_L("EmissiveColor"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &emissiveColor.x);
			pIter->QueryFloatAttribute(GS_L("g"), &emissiveColor.y);
			pIter->QueryFloatAttribute(GS_L("b"), &emissiveColor.z);
			pIter->QueryFloatAttribute(GS_L("a"), &emissiveColor.w);
		}
	}

	pNode = pElement->FirstChild(GS_L("SpriteCut"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryIntAttribute(GS_L("x"), &spriteCut.x);
			pIter->QueryIntAttribute(GS_L("y"), &spriteCut.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("Scale"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &scale.x);
			pIter->QueryFloatAttribute(GS_L("y"), &scale.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("PivotAdjust"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &pivotAdjust.x);
			pIter->QueryFloatAttribute(GS_L("y"), &pivotAdjust.y);
		}
	}

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
					for (unsigned int t=0; t<_ETH_MAX_PARTICLE_SYS_PER_ENTITY; t++)
					{
						if (pParticleIter)
						{
							boost::shared_ptr<ETH_PARTICLE_SYSTEM> newSystem = boost::shared_ptr<ETH_PARTICLE_SYSTEM>(new ETH_PARTICLE_SYSTEM);
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
	return true;
}

bool ETHEntityProperties::WriteToXMLFile(TiXmlElement *pHeadRoot) const
{
	TiXmlElement *pRoot = new TiXmlElement(GS_L("Entity"));
	pHeadRoot->LinkEndChild(pRoot); 

	TiXmlElement *pElement;
	pElement = new TiXmlElement(GS_L("EmissiveColor"));
	pRoot->LinkEndChild(pElement); 
	pElement->SetDoubleAttribute(GS_L("r"), emissiveColor.x);
	pElement->SetDoubleAttribute(GS_L("g"), emissiveColor.y);
	pElement->SetDoubleAttribute(GS_L("b"), emissiveColor.z);
	pElement->SetDoubleAttribute(GS_L("a"), emissiveColor.w);

	pElement = new TiXmlElement(GS_L("SpriteCut"));
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), spriteCut.x);
	pElement->SetDoubleAttribute(GS_L("y"), spriteCut.y);

	pElement = new TiXmlElement(GS_L("Scale"));
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), scale.x);
	pElement->SetDoubleAttribute(GS_L("y"), scale.y);

	pElement = new TiXmlElement(GS_L("PivotAdjust"));
	pRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), pivotAdjust.x);
	pElement->SetDoubleAttribute(GS_L("y"), pivotAdjust.y);

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

	TiXmlElement *pParticles = new TiXmlElement(GS_L("Particles"));
	pRoot->LinkEndChild(pParticles);
	for (unsigned int t=0; t<particleSystems.size(); t++)
	{
		if (particleSystems[t]->nParticles > 0)
			particleSystems[t]->WriteToXMLFile(pParticles);
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
			if (polygon)
			{
				pElement = new TiXmlElement(GS_L("Polygon"));
				pElement->LinkEndChild(new TiXmlText(polygon->GetENMLDeclaration()));
				pCollisionRoot->LinkEndChild(pElement);
			}
			if (compoundShape)
			{
				pElement = new TiXmlElement(GS_L("Compound"));
				pElement->LinkEndChild(new TiXmlText(compoundShape->GetENMLDeclaration()));
				pCollisionRoot->LinkEndChild(pElement);
			}
			if (enmlJointDefinitions != GS_L(""))
			{
				pElement = new TiXmlElement(GS_L("Joints"));
				pElement->LinkEndChild(new TiXmlText(enmlJointDefinitions));
				pCollisionRoot->LinkEndChild(pElement);
			}
		}
	}

	pRoot->SetAttribute(GS_L("type"), type);
	pRoot->SetAttribute(GS_L("static"), staticEntity);
	pRoot->SetAttribute(GS_L("shape"), shape);
	pRoot->SetAttribute(GS_L("startFrame"), startFrame);
	pRoot->SetAttribute(GS_L("applyLight"), applyLight);
	pRoot->SetAttribute(GS_L("castShadow"), castShadow);
	pRoot->SetAttribute(GS_L("sensor"), sensor);
	pRoot->SetAttribute(GS_L("bullet"), bullet);
	pRoot->SetAttribute(GS_L("fixedRotation"), fixedRotation);
	pRoot->SetAttribute(GS_L("blendMode"), blendMode);
	pRoot->SetDoubleAttribute(GS_L("friction"), friction);
	pRoot->SetDoubleAttribute(GS_L("density"), density);
	pRoot->SetDoubleAttribute(GS_L("layerDepth"), layerDepth);
	pRoot->SetDoubleAttribute(GS_L("soundVolume"), soundVolume);
	pRoot->SetDoubleAttribute(GS_L("shadowScale"), shadowScale);
	pRoot->SetDoubleAttribute(GS_L("shadowLengthScale"), shadowLengthScale);
	pRoot->SetDoubleAttribute(GS_L("shadowOpacity"), shadowOpacity);
	pRoot->SetDoubleAttribute(GS_L("specularPower"), specularPower);
	pRoot->SetDoubleAttribute(GS_L("specularBrightness"), specularBrightness);
	pRoot->SetDoubleAttribute(GS_L("restitution"), restitution);
	pRoot->SetDoubleAttribute(GS_L("parallaxIntensity"), parallaxIntensity);
	pRoot->SetDoubleAttribute(GS_L("gravityScale"), gravityScale);

	WriteDataToFile(pRoot);
	return true;
}
