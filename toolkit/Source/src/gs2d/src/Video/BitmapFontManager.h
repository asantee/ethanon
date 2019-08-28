#ifndef GS2D_BITMAP_FONT_MANAGER_H_
#define GS2D_BITMAP_FONT_MANAGER_H_

#include "BitmapFont.h"
#include "hopscotch_map.h"

namespace gs2d {

class BitmapFontManager
{
	BitmapFontPtr SeekBitmapFont(Video* video, const std::string& font);
	BitmapFontPtr LoadBitmapFont(Video* video, const std::string& fullFilePath);

	tsl::hopscotch_map<std::string, BitmapFontPtr> m_fonts;

public:

	void ClearFonts();

	math::Vector2 ComputeCarretPosition(
		Video* video,
		const std::string& font,
		const std::string& text,
		const unsigned int pos);

	math::Vector2 ComputeTextBoxSize(
		Video* video,
		const std::string& font,
		const std::string& text);

	unsigned int FindClosestCarretPosition(
		Video* video,
		const std::string& font,
		const std::string &text,
		const math::Vector2 &textPos,
		const math::Vector2 &reference);

	bool DrawBitmapText(
		Video* video,
		const math::Vector2 &v2Pos,
		const std::string& text,
		const std::string& font,
		const Color& color,
		const float scale = 1.0f);
};

} // namespace gs2d

#endif
