#include "ETHLight.h"
#include "../Util/ETHASUtil.h"
#include "ETHEntityProperties.h"

ETHLight::ETHLight(const bool active) :
	active(active),
	staticLight(false),
	pos(Vector3(0,0,0)),
	color(Vector3(1,1,1)),
	range(256.0f),
	haloSize(64.0f),
	haloBrightness(1.0f),
	castShadows(false),
	haloBitmap((""))
{
}

bool ETHLight::ReadFromXMLFile(TiXmlElement *pElement)
{
	active      = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("active"), active);
	staticLight = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("static"), staticLight);
	castShadows = ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, ("castShadows"), castShadows);

	pElement->QueryFloatAttribute(("range"), &range);
	pElement->QueryFloatAttribute(("haloBrightness"), &haloBrightness);
	pElement->QueryFloatAttribute(("haloSize"), &haloSize);

	ETHEntityProperties::ReadVector3PropertyFromXmlElement(pElement, ("Position"), pos);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, ("Color"), color);

	TiXmlNode *pNode = pElement->FirstChild(("HaloBitmap"));
	if (pNode)
	{
		TiXmlElement *pStringElement = pNode->ToElement();
		if (pStringElement)
		{
			haloBitmap = pStringElement->GetText();
		}
	}
	return true;
}

bool ETHLight::WriteToXMLFile(TiXmlElement *pRoot) const
{
	TiXmlElement *pLightRoot = new TiXmlElement(("Light"));
	pRoot->LinkEndChild(pLightRoot); 

	ETHEntityProperties::SetVector3PropertyToXmlElement(pLightRoot, ("Position"), pos);
	ETHEntityProperties::SetColorPropertyToXmlElement(pLightRoot, ("Color"), color);

	if (haloBitmap != (""))
	{
		TiXmlElement *pElement;
		pElement = new TiXmlElement(("HaloBitmap"));
		pElement->LinkEndChild(new TiXmlText(haloBitmap));
		pLightRoot->LinkEndChild(pElement);
	}

	pLightRoot->SetAttribute(("active"), active);
	pLightRoot->SetAttribute(("static"), staticLight);
	pLightRoot->SetAttribute(("castShadows"), castShadows);
	pLightRoot->SetDoubleAttribute(("range"), range);
	pLightRoot->SetDoubleAttribute(("haloBrightness"), haloBrightness);
	pLightRoot->SetDoubleAttribute(("haloSize"), haloSize);
	return true;
}

bool ETHLight::IsActive() const
{
	return ETHGlobal::ToBool(active);
}
