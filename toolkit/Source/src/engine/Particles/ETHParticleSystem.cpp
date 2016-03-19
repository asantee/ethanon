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

#include "ETHParticleSystem.h"
#include "../Entity/ETHEntityProperties.h"
#include <Platform/Platform.h>

void ETHParticleSystem::Scale(const float scale)
{
	gravityVector *= scale;
	directionVector *= scale;
	randomizeDir *= scale;
	startPoint *= scale;
	randStartPoint *= scale;
	size *= scale;
	randomizeSize *= scale;
	growth *= scale;
	minSize *= scale;
	maxSize *= scale;
}

void ETHParticleSystem::MirrorX(const bool mirrorGravity)
{
	if (mirrorGravity)
		gravityVector.x *=-1;
	startPoint.x *=-1;
	directionVector.x *=-1;
	randomizeDir.x *=-1;
	randStartPoint.x *=-1;
}

void ETHParticleSystem::MirrorY(const bool mirrorGravity)
{
	if (mirrorGravity)
		gravityVector.y *=-1;
	startPoint.y *=-1;
	directionVector.y *=-1;
	randomizeDir.y *=-1;
	randStartPoint.y *=-1;
}

ETHParticleSystem::ETHParticleSystem()
{
	Reset();
}

void ETHParticleSystem::Reset()
{
	alphaMode = Video::AM_PIXEL;
	nParticles = 0;
	lifeTime = 0.0f;
	randomizeLifeTime = 0.0f;
	size = 1.0f;
	growth = 0.0f;
	minSize = 0.0f;
	maxSize = 99999.0f;
	repeat = 0;
	randomizeSize = 0.0f;
	randAngleStart = 0.0f;
	angleStart = 0.0f;
	emissive = Vector3(1,1,1);
	allAtOnce = false;
	bitmapFile = ETH_DEFAULT_PARTICLE_BITMAP;
	spriteCut = Vector2i(1,1);
	animationMode = PLAY_ANIMATION;
}

bool ETHParticleSystem::ReadFromXMLFile(TiXmlElement *pElement)
{
	pElement->QueryIntAttribute(GS_L("particles"), &nParticles);

	allAtOnce = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("allAtOnce"), allAtOnce);

	pElement->QueryIntAttribute(GS_L("alphaMode"), (int*)&alphaMode);
	pElement->QueryIntAttribute(GS_L("repeat"), &repeat);
	pElement->QueryFloatAttribute(GS_L("lifeTime"), &lifeTime);
	pElement->QueryFloatAttribute(GS_L("randomLifeTime"), &randomizeLifeTime);
	pElement->QueryFloatAttribute(GS_L("angleDir"), &angleDir);
	pElement->QueryFloatAttribute(GS_L("randAngle"), &randAngle);
	pElement->QueryFloatAttribute(GS_L("size"), &size);
	pElement->QueryFloatAttribute(GS_L("randomizeSize"), &randomizeSize);
	pElement->QueryFloatAttribute(GS_L("growth"), &growth);
	pElement->QueryFloatAttribute(GS_L("minSize"), &minSize);
	pElement->QueryFloatAttribute(GS_L("maxSize"), &maxSize);
	pElement->QueryFloatAttribute(GS_L("angleStart"), &angleStart);
	pElement->QueryFloatAttribute(GS_L("randAngleStart"), &randAngleStart);
	pElement->QueryIntAttribute(GS_L("animationMode"), &animationMode);

	TiXmlNode *pNode;
	TiXmlElement *pStringElement;

	pNode = pElement->FirstChild(GS_L("Bitmap"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			bitmapFile = pStringElement->GetText();
		}
	}

	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, GS_L("Gravity"), gravityVector);
	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, GS_L("Direction"), directionVector);
	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, GS_L("RandomizeDir"), randomizeDir);
	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, GS_L("RandStartPoint"), randStartPoint);
	ETHEntityProperties::ReadVector2iPropertyFromXmlElement(pElement, GS_L("SpriteCut"), spriteCut);

	ETHEntityProperties::ReadVector3PropertyFromXmlElement(pElement, GS_L("StartPoint"), startPoint);

	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("Color0"), color0);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("Color1"), color1);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("Luminance"), emissive);
	return true;
}

bool ETHParticleSystem::ReadFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager)
{
	TiXmlDocument doc(fileName);
	str_type::string content;
	fileManager->GetUTFFileString(fileName, content);
	if (!doc.LoadFile(content, TIXML_ENCODING_LEGACY))
		return false;

	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);

	TiXmlElement *pElem = hDoc.FirstChildElement().Element();
	if (!pElem)
		return false;

	hRoot = TiXmlHandle(pElem);

	return ReadFromXMLFile(hRoot.FirstChildElement().Element());
}

bool ETHParticleSystem::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pParticleRoot = new TiXmlElement(GS_L("ParticleSystem"));
	pRoot->LinkEndChild(pParticleRoot); 

	TiXmlElement *pElement;

	if (bitmapFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("Bitmap"));
		pElement->LinkEndChild(new TiXmlText( Platform::GetFileName(bitmapFile)));
		pParticleRoot->LinkEndChild(pElement);
	}

	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, GS_L("Gravity"), gravityVector);
	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, GS_L("Direction"), directionVector);
	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, GS_L("RandomizeDir"), randomizeDir);
	ETHEntityProperties::SetVector2iPropertyToXmlElement(pParticleRoot, GS_L("SpriteCut"), spriteCut);
	ETHEntityProperties::SetVector3PropertyToXmlElement(pParticleRoot, GS_L("StartPoint"), startPoint);
	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, GS_L("RandStartPoint"), randStartPoint);
	
	ETHEntityProperties::SetColorPropertyToXmlElement(pParticleRoot, GS_L("Color0"), color0);
	ETHEntityProperties::SetColorPropertyToXmlElement(pParticleRoot, GS_L("Color1"), color1);
	ETHEntityProperties::SetColorPropertyToXmlElement(pParticleRoot, GS_L("Luminance"), emissive);

	pParticleRoot->SetAttribute(GS_L("particles"), nParticles);
	pParticleRoot->SetAttribute(GS_L("allAtOnce"), allAtOnce);
	pParticleRoot->SetAttribute(GS_L("alphaMode"), alphaMode);
	pParticleRoot->SetAttribute(GS_L("repeat"), repeat);
	pParticleRoot->SetAttribute(GS_L("animationMode"), animationMode);
	pParticleRoot->SetDoubleAttribute(GS_L("lifeTime"), lifeTime);
	pParticleRoot->SetDoubleAttribute(GS_L("randomLifeTime"), randomizeLifeTime);
	pParticleRoot->SetDoubleAttribute(GS_L("angleDir"), angleDir);
	pParticleRoot->SetDoubleAttribute(GS_L("randAngle"), randAngle);
	pParticleRoot->SetDoubleAttribute(GS_L("size"), size);
	pParticleRoot->SetDoubleAttribute(GS_L("randomizeSize"), randomizeSize);
	pParticleRoot->SetDoubleAttribute(GS_L("growth"), growth);
	pParticleRoot->SetDoubleAttribute(GS_L("minSize"), minSize);
	pParticleRoot->SetDoubleAttribute(GS_L("maxSize"), maxSize);
	pParticleRoot->SetDoubleAttribute(GS_L("angleStart"), angleStart);
	pParticleRoot->SetDoubleAttribute(GS_L("randAngleStart"), randAngleStart);	

	return true;
}

bool ETHParticleSystem::ShouldUseHighlightPS() const
{
	return (
		   color0.x > 1.0f
		|| color0.y > 1.0f
		|| color0.z > 1.0f
		|| color1.x > 1.0f
		|| color1.y > 1.0f
		|| color1.z > 1.0f
	);
}

int ETHParticleSystem::GetNumFrames() const
{
	return spriteCut.x * spriteCut.y;
}

str_type::string ETHParticleSystem::GetActualBitmapFile() const
{
	return (bitmapFile == GS_L("")) ? ETH_DEFAULT_PARTICLE_BITMAP : bitmapFile;
}
