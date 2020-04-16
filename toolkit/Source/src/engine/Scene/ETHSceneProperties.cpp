#include "ETHSceneProperties.h"

ETHSceneProperties::ETHSceneProperties()
{
	Reset();
}

bool ETHSceneProperties::ReadFromXMLFile(TiXmlElement *pRoot)
{
	TiXmlNode *pNode;
	pNode = pRoot->FirstChild(("SceneProperties"));
	if (pNode)
	{
		TiXmlElement *pElement = pNode->ToElement();
		pElement->QueryFloatAttribute(("parallaxIntensity"), &parallaxIntensity);

		TiXmlElement *pIter;
		pNode = pElement->FirstChild(("Ambient"));
		if (pNode)
		{
			pIter = pNode->ToElement();
			if (pIter)
			{
				pIter->QueryFloatAttribute(("r"), &ambient.x);
				pIter->QueryFloatAttribute(("g"), &ambient.y);
				pIter->QueryFloatAttribute(("b"), &ambient.z);
			}
		}
		pNode = pElement->FirstChild(("ZAxisDirection"));
		if (pNode)
		{
			pIter = pNode->ToElement();
			if (pIter)
			{
				pIter->QueryFloatAttribute(("x"), &zAxisDirection.x);
				pIter->QueryFloatAttribute(("y"), &zAxisDirection.y);
			}
		}
	}
	return true;
}

bool ETHSceneProperties::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pScenePropRoot = new TiXmlElement(("SceneProperties"));
	pRoot->LinkEndChild(pScenePropRoot); 

	TiXmlElement *pElement;
	pElement = new TiXmlElement(("Ambient"));
	pScenePropRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(("r"), ambient.x);
	pElement->SetDoubleAttribute(("g"), ambient.y);
	pElement->SetDoubleAttribute(("b"), ambient.z);

	pElement = new TiXmlElement(("ZAxisDirection"));
	pScenePropRoot->LinkEndChild(pElement);
	pElement->SetDoubleAttribute(("x"), zAxisDirection.x);
	pElement->SetDoubleAttribute(("y"), zAxisDirection.y);
	
	pScenePropRoot->SetDoubleAttribute(("lightIntensity"), 1.0);
	pScenePropRoot->SetDoubleAttribute(("parallaxIntensity"), parallaxIntensity);
	return true;
}

void ETHSceneProperties::Reset()
{
	ambient = math::constant::ONE_VECTOR3; // Vector3(_ETH_DEFAULT_AMBIENT_LIGHT,_ETH_DEFAULT_AMBIENT_LIGHT,_ETH_DEFAULT_AMBIENT_LIGHT);
	zAxisDirection = Vector2(0,-1);
	parallaxIntensity = 0;
}
