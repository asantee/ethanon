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

#include "ETHSceneProperties.h"

ETHSceneProperties::ETHSceneProperties()
{
	Reset();
}

bool ETHSceneProperties::ReadFromXMLFile(TiXmlElement *pRoot)
{
	TiXmlNode *pNode;
	pNode = pRoot->FirstChild(GS_L("SceneProperties"));
	if (pNode)
	{
		TiXmlElement *pElement = pNode->ToElement();
		pElement->QueryFloatAttribute(GS_L("lightIntensity"), &lightIntensity);
		pElement->QueryFloatAttribute(GS_L("parallaxIntensity"), &parallaxIntensity);

		TiXmlElement *pIter;
		pNode = pElement->FirstChild(GS_L("Ambient"));
		if (pNode)
		{
			pIter = pNode->ToElement();
			if (pIter)
			{
				pIter->QueryFloatAttribute(GS_L("r"), &ambient.x);
				pIter->QueryFloatAttribute(GS_L("g"), &ambient.y);
				pIter->QueryFloatAttribute(GS_L("b"), &ambient.z);
			}
		}
		pNode = pElement->FirstChild(GS_L("ZAxisDirection"));
		if (pNode)
		{
			pIter = pNode->ToElement();
			if (pIter)
			{
				pIter->QueryFloatAttribute(GS_L("x"), &zAxisDirection.x);
				pIter->QueryFloatAttribute(GS_L("y"), &zAxisDirection.y);
			}
		}
	}
	return true;
}

bool ETHSceneProperties::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pScenePropRoot = new TiXmlElement(GS_L("SceneProperties"));
	pRoot->LinkEndChild(pScenePropRoot); 

	TiXmlElement *pElement;
	pElement = new TiXmlElement(GS_L("Ambient"));
	pScenePropRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("r"), ambient.x);
	pElement->SetDoubleAttribute(GS_L("g"), ambient.y);
	pElement->SetDoubleAttribute(GS_L("b"), ambient.z);

	pElement = new TiXmlElement(GS_L("ZAxisDirection"));
	pScenePropRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(GS_L("x"), zAxisDirection.x);
	pElement->SetDoubleAttribute(GS_L("y"), zAxisDirection.y);
	
	pScenePropRoot->SetDoubleAttribute(GS_L("lightIntensity"), lightIntensity);
	pScenePropRoot->SetDoubleAttribute(GS_L("parallaxIntensity"), parallaxIntensity);
	return true;
}

void ETHSceneProperties::Reset()
{
	ambient = Vector3(_ETH_DEFAULT_AMBIENT_LIGHT,_ETH_DEFAULT_AMBIENT_LIGHT,_ETH_DEFAULT_AMBIENT_LIGHT);
	lightIntensity = _ETH_DEFAULT_LIGHT_INTENSITY;
	zAxisDirection = Vector2(0,-1);
	parallaxIntensity = 0;
}