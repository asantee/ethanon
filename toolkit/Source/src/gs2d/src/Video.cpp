#include "Video.h"

namespace gs2d {

Video::Video() :
	m_virtualScreenHeight(720.0f),
	m_rendering(false)
{
}

math::Vector2 Video::ComputeCarretPosition(
	const std::string& font,
	const std::string& text,
	const unsigned int pos)
{
	return BitmapFontManager::ComputeCarretPosition(this, font, text, pos);
}

math::Vector2 Video::ComputeTextBoxSize(const std::string& font, const std::string& text)
{
	return BitmapFontManager::ComputeTextBoxSize(this, font, text);
}

unsigned int Video::FindClosestCarretPosition(
	const std::string& font,
	const std::string& text,
	const math::Vector2 &textPos,
	const math::Vector2 &reference)
{
	return BitmapFontManager::FindClosestCarretPosition(this, font, text, textPos, reference);
}

bool Video::DrawBitmapText(
	const math::Vector2& v2Pos,
	const std::string& text,
	const std::string& font,
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
	if (IsRendering())
		EndRendering();

	APP_STATUS status = APP_SKIP;
	while (status == APP_SKIP)
	{
		status = HandleEvents();
		if (status == APP_QUIT)
			return false;
	}

	if (!IsRendering())
		BeginRendering();

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
	return m_cameraPos;
}

bool Video::IsRendering() const
{
	return m_rendering;
}

void Video::SetVirtualScreenHeight(const float height)
{
	m_virtualScreenHeight = height;
}

float Video::GetVirtualScreenHeight() const
{
	return m_virtualScreenHeight;
}

float Video::GetScaleFactor() const
{
	return (m_virtualScreenHeight / GetScreenSizeInPixels().y);
}

} // namespace gs2d
