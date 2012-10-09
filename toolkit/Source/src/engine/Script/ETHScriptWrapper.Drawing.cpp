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

#include "ETHScriptWrapper.h"

Vector2 ETHScriptWrapper::ComputeCarretPosition(const str_type::string &font, const str_type::string &text, const unsigned int pos)
{
	return m_provider->GetVideo()->ComputeCarretPosition(font, text, pos);
}

Vector2 ETHScriptWrapper::ComputeTextBoxSize(const str_type::string &font, const str_type::string &text)
{
	return m_provider->GetVideo()->ComputeTextBoxSize(font, text);
}

void ETHScriptWrapper::DrawText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const GS_DWORD color, const float scale)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(
		new ETHTextDrawer(m_provider, v2Pos, text, font, color, 0x0, scale)));
}

void ETHScriptWrapper::LoadSprite(const str_type::string &name)
{
	if (WarnIfRunsInMainFunction(GS_L("LoadSprite")))
		return;
	LoadAndGetSprite(name);
}

SpritePtr ETHScriptWrapper::LoadAndGetSprite(const str_type::string &name)
{
	str_type::string resourceDirectory = m_provider->GetFileIOHub()->GetResourceDirectory();
	str_type::string path = resourceDirectory;
	path += name;
	return m_provider->GetGraphicResourceManager()->AddFile(m_provider->GetVideo(), path, resourceDirectory, false);
}

Vector2 ETHScriptWrapper::GetSpriteSize(const str_type::string &name)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		return pSprite->GetBitmapSizeF();
	}
	else
	{
		return Vector2(0,0);
	}
}

Vector2 ETHScriptWrapper::GetSpriteFrameSize(const str_type::string& name)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		return pSprite->GetFrameSize();
	}
	else
	{
		return Vector2(0,0);
	}
}

void ETHScriptWrapper::SetupSpriteRects(const str_type::string& name, const unsigned int columns, const unsigned int rows)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetupSpriteRects(columns, rows);
	}
}

void ETHScriptWrapper::SetSpriteRect(const str_type::string& name, const unsigned int frame)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetRect(frame);
	}
}

void ETHScriptWrapper::SetSpriteOrigin(const str_type::string& name, const Vector2& origin)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	if (pSprite)
	{
		pSprite->SetOrigin(origin);
	}
}

void ETHScriptWrapper::DrawSprite(const str_type::string &name, const Vector2 &v2Pos, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(),
							m_provider->GetFileIOHub()->GetResourceDirectory(), name, v2Pos,
							Vector2(0,0), color, 0.0f, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawShaped(const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(),
							m_provider->GetFileIOHub()->GetResourceDirectory(), name, v2Pos,
							v2Size, color, 0.0f, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawSpriteZ(const str_type::string &name, const float depth, const Vector2 &v2Pos, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(),
							m_provider->GetFileIOHub()->GetResourceDirectory(), name, v2Pos,
							Vector2(0,0), color, depth, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawShapedZ(const str_type::string &name, const float depth, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle)
{
	SpritePtr pSprite = LoadAndGetSprite(name);
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(
		new ETHSpriteDrawer(m_provider, m_provider->GetGraphicResourceManager(),
							m_provider->GetFileIOHub()->GetResourceDirectory(), name, v2Pos,
							v2Size, color, depth, angle, pSprite->GetRectIndex())));
}

void ETHScriptWrapper::DrawFadingText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const GS_DWORD color, unsigned long time, const float scale)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(
		new ETHTextDrawer(m_provider, v2Pos, text, font, color, time, scale)));
}

void ETHScriptWrapper::AddLight(const Vector3 &v3Pos, const Vector3 &v3Color, const float range, const bool castShadows)
{
	if (WarnIfRunsInMainFunction(GS_L("AddLight")))
		return;

	ETHLight light(true);
	light.castShadows = castShadows;
	light.color = v3Color;
	light.pos = v3Pos;
	light.range = range;
	light.staticLight = false;
	m_pScene->AddLight(light);
}

void ETHScriptWrapper::DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
									 const GS_DWORD color0, const GS_DWORD color1,
									 const GS_DWORD color2, const GS_DWORD color3)
{
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(new ETHRectangleDrawer(m_provider, v2Pos, v2Size, color0, color1, color2, color3)));
}

void ETHScriptWrapper::DrawLine(const Vector2 &v2A, const Vector2 &v2B, const Color a, const Color b, const float width) //-V801
{
	m_primitiveList.push_back(boost::shared_ptr<ETHElementDrawer>(new ETHLineDrawer(m_provider, v2A, v2B, a, b, width)));
}
