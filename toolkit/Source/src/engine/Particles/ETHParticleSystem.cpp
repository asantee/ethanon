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
	pElement->QueryIntAttribute(("particles"), &nParticles);

	allAtOnce = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("allAtOnce"), allAtOnce);

	pElement->QueryIntAttribute(("alphaMode"), (int*)&alphaMode);
	pElement->QueryIntAttribute(("repeat"), &repeat);
	pElement->QueryFloatAttribute(("lifeTime"), &lifeTime);
	pElement->QueryFloatAttribute(("randomLifeTime"), &randomizeLifeTime);
	pElement->QueryFloatAttribute(("angleDir"), &angleDir);
	pElement->QueryFloatAttribute(("randAngle"), &randAngle);
	pElement->QueryFloatAttribute(("size"), &size);
	pElement->QueryFloatAttribute(("randomizeSize"), &randomizeSize);
	pElement->QueryFloatAttribute(("growth"), &growth);
	pElement->QueryFloatAttribute(("minSize"), &minSize);
	pElement->QueryFloatAttribute(("maxSize"), &maxSize);
	pElement->QueryFloatAttribute(("angleStart"), &angleStart);
	pElement->QueryFloatAttribute(("randAngleStart"), &randAngleStart);
	pElement->QueryIntAttribute(("animationMode"), &animationMode);

	TiXmlNode *pNode;
	TiXmlElement *pStringElement;

	pNode = pElement->FirstChild(("Bitmap"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			bitmapFile = pStringElement->GetText();
		}
	}

	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, ("Gravity"), gravityVector);
	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, ("Direction"), directionVector);
	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, ("RandomizeDir"), randomizeDir);
	ETHEntityProperties::ReadVector2PropertyFromXmlElement(pElement, ("RandStartPoint"), randStartPoint);
	ETHEntityProperties::ReadVector2iPropertyFromXmlElement(pElement, ("SpriteCut"), spriteCut);

	ETHEntityProperties::ReadVector3PropertyFromXmlElement(pElement, ("StartPoint"), startPoint);

	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("Color0"), color0);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("Color1"), color1);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("Luminance"), emissive);
	return true;
}

bool ETHParticleSystem::ReadFromFile(const std::string& fileName, const Platform::FileManagerPtr& fileManager)
{
	TiXmlDocument doc(fileName);
	std::string content;
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
	TiXmlElement *pParticleRoot = new TiXmlElement(("ParticleSystem"));
	pRoot->LinkEndChild(pParticleRoot); 

	TiXmlElement *pElement;

	if (bitmapFile != (""))
	{
		pElement = new TiXmlElement(("Bitmap"));
		pElement->LinkEndChild(new TiXmlText( Platform::GetFileName(bitmapFile)));
		pParticleRoot->LinkEndChild(pElement);
	}

	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, ("Gravity"), gravityVector);
	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, ("Direction"), directionVector);
	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, ("RandomizeDir"), randomizeDir);
	ETHEntityProperties::SetVector2iPropertyToXmlElement(pParticleRoot, ("SpriteCut"), spriteCut);
	ETHEntityProperties::SetVector3PropertyToXmlElement(pParticleRoot, ("StartPoint"), startPoint);
	ETHEntityProperties::SetVector2PropertyToXmlElement(pParticleRoot, ("RandStartPoint"), randStartPoint);
	
	ETHEntityProperties::SetColorPropertyToXmlElement(pParticleRoot, ("Color0"), color0);
	ETHEntityProperties::SetColorPropertyToXmlElement(pParticleRoot, ("Color1"), color1);
	ETHEntityProperties::SetColorPropertyToXmlElement(pParticleRoot, ("Luminance"), emissive);

	pParticleRoot->SetAttribute(("particles"), nParticles);
	pParticleRoot->SetAttribute(("allAtOnce"), allAtOnce);
	pParticleRoot->SetAttribute(("alphaMode"), alphaMode);
	pParticleRoot->SetAttribute(("repeat"), repeat);
	pParticleRoot->SetAttribute(("animationMode"), animationMode);
	pParticleRoot->SetDoubleAttribute(("lifeTime"), lifeTime);
	pParticleRoot->SetDoubleAttribute(("randomLifeTime"), randomizeLifeTime);
	pParticleRoot->SetDoubleAttribute(("angleDir"), angleDir);
	pParticleRoot->SetDoubleAttribute(("randAngle"), randAngle);
	pParticleRoot->SetDoubleAttribute(("size"), size);
	pParticleRoot->SetDoubleAttribute(("randomizeSize"), randomizeSize);
	pParticleRoot->SetDoubleAttribute(("growth"), growth);
	pParticleRoot->SetDoubleAttribute(("minSize"), minSize);
	pParticleRoot->SetDoubleAttribute(("maxSize"), maxSize);
	pParticleRoot->SetDoubleAttribute(("angleStart"), angleStart);
	pParticleRoot->SetDoubleAttribute(("randAngleStart"), randAngleStart);

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

std::string ETHParticleSystem::GetActualBitmapFile() const
{
	return (bitmapFile == ("")) ? ETH_DEFAULT_PARTICLE_BITMAP : bitmapFile;
}
