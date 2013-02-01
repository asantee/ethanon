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
	haloBitmap(GS_L(""))
{
}

bool ETHLight::ReadFromXMLFile(TiXmlElement *pElement)
{
	ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("active"), active);
	ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("static"), staticLight);
	ETHEntityProperties::ReadBooleanPropertyFromXmlElement(pElement, GS_L("castShadows"), castShadows);

	pElement->QueryFloatAttribute(GS_L("range"), &range);
	pElement->QueryFloatAttribute(GS_L("haloBrightness"), &haloBrightness);
	pElement->QueryFloatAttribute(GS_L("haloSize"), &haloSize);

	ETHEntityProperties::ReadVector3PropertyFromXmlElement(pElement, GS_L("Position"), pos);
	ETHEntityProperties::ReadColorPropertyFromXmlElement(pElement, GS_L("Color"), color);

	TiXmlNode *pNode = pElement->FirstChild(GS_L("HaloBitmap"));
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
	TiXmlElement *pLightRoot = new TiXmlElement(GS_L("Light"));
	pRoot->LinkEndChild(pLightRoot); 

	ETHEntityProperties::SetVector3PropertyToXmlElement(pLightRoot, GS_L("Position"), pos);
	ETHEntityProperties::SetColorPropertyToXmlElement(pLightRoot, GS_L("Color"), color);

	if (haloBitmap != GS_L(""))
	{
		TiXmlElement *pElement;
		pElement = new TiXmlElement(GS_L("HaloBitmap"));
		pElement->LinkEndChild(new TiXmlText(haloBitmap));
		pLightRoot->LinkEndChild(pElement);
	}

	pLightRoot->SetAttribute(GS_L("active"), active);
	pLightRoot->SetAttribute(GS_L("static"), staticLight);
	pLightRoot->SetAttribute(GS_L("castShadows"), castShadows);
	pLightRoot->SetDoubleAttribute(GS_L("range"), range);
	pLightRoot->SetDoubleAttribute(GS_L("haloBrightness"), haloBrightness);
	pLightRoot->SetDoubleAttribute(GS_L("haloSize"), haloSize);
	return true;
}

bool ETHLight::IsActive() const
{
	return ETHGlobal::ToBool(active);
}

void ETHLight::SetLightScissor(const VideoPtr& video, const Vector2& zAxisDir) const
{
	const float squareEdgeSize = range * 2.0f;
	Vector2 sum((zAxisDir.SquaredLength() > 0.0f) ? (zAxisDir * pos.z * 4.0f) : math::constant::ZERO_VECTOR2);
	sum.x = Abs(sum.x);
	sum.y = Abs(sum.y);
	const Vector2 squareSize(Vector2(squareEdgeSize, squareEdgeSize) + sum);
	const Vector2 absPos(ETHGlobal::ToScreenPos(pos, zAxisDir) - video->GetCameraPos() - (squareSize * 0.5f));
	video->SetScissor(Rect2D(absPos.ToVector2i(), squareSize.ToVector2i()));
}
