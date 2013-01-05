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
#include <Platform/Platform.h>

void ETHParticleSystem::Scale(const float scale)
{
	boundingSphere *= scale;
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
	boundingSphere = 512.0f;
	soundFXFile = GS_L("");
	bitmapFile = ETH_DEFAULT_PARTICLE_BITMAP;
	spriteCut = Vector2i(1,1);
	animationMode = PLAY_ANIMATION;
}

bool ETHParticleSystem::ReadFromXMLFile(TiXmlElement *pElement)
{
	pElement->QueryIntAttribute(GS_L("particles"), &nParticles);

	int tempAllAtOnce;
	pElement->QueryIntAttribute(GS_L("allAtOnce"), &tempAllAtOnce);
	allAtOnce = static_cast<ETH_BOOL>(tempAllAtOnce);

	pElement->QueryIntAttribute(GS_L("alphaMode"), (int*)&alphaMode);
	pElement->QueryIntAttribute(GS_L("repeat"), &repeat);
	pElement->QueryFloatAttribute(GS_L("boundingSphere"), &boundingSphere);
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

	pNode = pElement->FirstChild(GS_L("SoundEffect"));
	if (pNode)
	{
		pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			soundFXFile = pStringElement->GetText();
		}
	}

	TiXmlElement *pIter;
	pNode = pElement->FirstChild(GS_L("Gravity"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &gravityVector.x);
			pIter->QueryFloatAttribute(GS_L("y"), &gravityVector.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("Direction"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &directionVector.x);
			pIter->QueryFloatAttribute(GS_L("y"), &directionVector.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("RandomizeDir"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &randomizeDir.x);
			pIter->QueryFloatAttribute(GS_L("y"), &randomizeDir.y);
		}
	}

	pNode = pElement->FirstChild(GS_L("StartPoint"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &startPoint.x);
			pIter->QueryFloatAttribute(GS_L("y"), &startPoint.y);
			pIter->QueryFloatAttribute(GS_L("z"), &startPoint.z);
		}
	}

	pNode = pElement->FirstChild(GS_L("RandStartPoint"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("x"), &randStartPoint.x);
			pIter->QueryFloatAttribute(GS_L("y"), &randStartPoint.y);
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

	pNode = pElement->FirstChild(GS_L("Color0"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &color0.x);
			pIter->QueryFloatAttribute(GS_L("g"), &color0.y);
			pIter->QueryFloatAttribute(GS_L("b"), &color0.z);
			pIter->QueryFloatAttribute(GS_L("a"), &color0.w);
		}
	}

	pNode = pElement->FirstChild(GS_L("Color1"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &color1.x);
			pIter->QueryFloatAttribute(GS_L("g"), &color1.y);
			pIter->QueryFloatAttribute(GS_L("b"), &color1.z);
			pIter->QueryFloatAttribute(GS_L("a"), &color1.w);
		}
	}

	pNode = pElement->FirstChild(GS_L("Luminance"));
	if (pNode)
	{
		pIter = pNode->ToElement();
		if (pIter)
		{
			pIter->QueryFloatAttribute(GS_L("r"), &emissive.x);
			pIter->QueryFloatAttribute(GS_L("g"), &emissive.y);
			pIter->QueryFloatAttribute(GS_L("b"), &emissive.z);
		}
	}
	return true;
}

bool ETHParticleSystem::ReadFromFile(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager)
{
	TiXmlDocument doc(fileName);
	str_type::string content;
	fileManager->GetUTF16FileString(fileName, content);
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

	if (soundFXFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("SoundEffect"));
		pElement->LinkEndChild(
			new TiXmlText(Platform::GetFileName(soundFXFile))
			);
		pParticleRoot->LinkEndChild(pElement);
	}

	if (bitmapFile != GS_L(""))
	{
		pElement = new TiXmlElement(GS_L("Bitmap"));
		pElement->LinkEndChild(new TiXmlText( Platform::GetFileName(bitmapFile)));
		pParticleRoot->LinkEndChild(pElement);
	}

	pElement = new TiXmlElement(GS_L("Gravity"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), gravityVector.x);
	pElement->SetDoubleAttribute(GS_L("y"), gravityVector.y);

	pElement = new TiXmlElement(GS_L("Direction"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), directionVector.x);
	pElement->SetDoubleAttribute(GS_L("y"), directionVector.y);

	pElement = new TiXmlElement(GS_L("RandomizeDir"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), randomizeDir.x);
	pElement->SetDoubleAttribute(GS_L("y"), randomizeDir.y);

	pElement = new TiXmlElement(GS_L("SpriteCut"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), spriteCut.x);
	pElement->SetDoubleAttribute(GS_L("y"), spriteCut.y);

	pElement = new TiXmlElement(GS_L("StartPoint"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), startPoint.x);
	pElement->SetDoubleAttribute(GS_L("y"), startPoint.y);
	pElement->SetDoubleAttribute(GS_L("z"), startPoint.z);

	pElement = new TiXmlElement(GS_L("RandStartPoint"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), randStartPoint.x);
	pElement->SetDoubleAttribute(GS_L("y"), randStartPoint.y);

	pElement = new TiXmlElement(GS_L("Color0"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), color0.x);
	pElement->SetDoubleAttribute(GS_L("g"), color0.y);
	pElement->SetDoubleAttribute(GS_L("b"), color0.z);
	pElement->SetDoubleAttribute(GS_L("a"), color0.w);

	pElement = new TiXmlElement(GS_L("Color1"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), color1.x);
	pElement->SetDoubleAttribute(GS_L("g"), color1.y);
	pElement->SetDoubleAttribute(GS_L("b"), color1.z);
	pElement->SetDoubleAttribute(GS_L("a"), color1.w);

	pElement = new TiXmlElement(GS_L("Luminance"));
	pParticleRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), emissive.x);
	pElement->SetDoubleAttribute(GS_L("g"), emissive.y);
	pElement->SetDoubleAttribute(GS_L("b"), emissive.z);

	pParticleRoot->SetAttribute(GS_L("particles"), nParticles);
	pParticleRoot->SetAttribute(GS_L("allAtOnce"), allAtOnce);
	pParticleRoot->SetAttribute(GS_L("alphaMode"), alphaMode);
	pParticleRoot->SetAttribute(GS_L("repeat"), repeat);
	pParticleRoot->SetAttribute(GS_L("animationMode"), animationMode);
	pParticleRoot->SetDoubleAttribute(GS_L("boundingSphere"), boundingSphere);
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

int ETHParticleSystem::GetNumFrames() const
{
	return spriteCut.x * spriteCut.y;
}

str_type::string ETHParticleSystem::GetActualBitmapFile() const
{
	return (bitmapFile == GS_L("")) ? ETH_DEFAULT_PARTICLE_BITMAP : bitmapFile;
}
