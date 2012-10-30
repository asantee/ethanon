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

#include "Video.h"

namespace gs2d {

Video::Video() :
	m_depth(0.0f),
	m_lineWidth(1.0f),
	m_roundUpPosition(false)
{
}

math::Vector2 Video::ComputeCarretPosition(
	const str_type::string& font,
	const str_type::string& text,
	const unsigned int pos)
{
	return BitmapFontManager::ComputeCarretPosition(this, font, text, pos);
}

math::Vector2 Video::ComputeTextBoxSize(const str_type::string& font, const str_type::string& text)
{
	return BitmapFontManager::ComputeTextBoxSize(this, font, text);
}

unsigned int Video::FindClosestCarretPosition(
	const str_type::string& font,
	const str_type::string& text,
	const math::Vector2 &textPos,
	const math::Vector2 &reference)
{
	return BitmapFontManager::FindClosestCarretPosition(this, font, text, textPos, reference);
}

bool Video::DrawBitmapText(
	const math::Vector2& v2Pos,
	const str_type::string& text,
	const str_type::string& font,
	const Color& color,
	const float scale)
{
	return BitmapFontManager::DrawBitmapText(this, v2Pos, text, font, color, scale);
}

bool Video::VIDEO_MODE::operator==(const VIDEO_MODE& other) const
{
	return (width == other.width && height == other.height && pf == other.pf);
}

bool Video::VIDEO_MODE::operator<(const VIDEO_MODE &other) const
{
	if (pf < other.pf)
	{
		return true;
	}
	else if (pf == other.pf	&& width * height < other.width * other.height)
	{
		return true;
	}
	return false;
}

bool Video::ManageLoop()
{
	if (Rendering())
		EndSpriteScene();

	APP_STATUS status = APP_SKIP;
	while (status == APP_SKIP)
	{
		status = HandleEvents();
		if (status == APP_QUIT)
			return false;
	}

	if (!Rendering())
		BeginSpriteScene();

	return true;
}

bool Video::SetCameraPos(const math::Vector2& pos)
{
	m_cameraPos = pos;
	return true;
}

bool Video::MoveCamera(const math::Vector2& dir)
{
	SetCameraPos(m_cameraPos + dir);
	return true;
}

math::Vector2 Video::GetCameraPos() const
{
	if (IsRoundingUpPosition())
	{
		return math::Vector2(floor(m_cameraPos.x), floor(m_cameraPos.y));
	}
	else
	{
		return m_cameraPos;
	}
}

void Video::SetLineWidth(const float width)
{
	m_lineWidth = (width < 1.0f) ? 1.0f : width;
}

float Video::GetLineWidth() const
{
	return m_lineWidth;
}

void Video::RoundUpPosition(const bool roundUp)
{
	m_roundUpPosition = roundUp;
}
bool Video::IsRoundingUpPosition() const
{
	return m_roundUpPosition;
}

bool Video::SetSpriteDepth(const float depth)
{
	m_depth = depth;
	if (m_depth > 1.0f || m_depth < 0.0f)
	{
		ShowMessage(GS_L("Warning: the depth must range 0.0f - 1.0f - D3D9Video::SetSpriteDepth"), GSMT_WARNING);
		m_depth = math::Max(m_depth, 0.0f);
		m_depth = math::Min(m_depth, 1.0f);
	}
	return true;
}
float Video::GetSpriteDepth() const
{
	return m_depth;
}

} // namespace gs2d
