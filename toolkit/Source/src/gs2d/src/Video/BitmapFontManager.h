#ifndef GS2D_BITMAP_FONT_MANAGER_H_
#define GS2D_BITMAP_FONT_MANAGER_H_

#include "BitmapFont.h"
#include "../../../tsl/hopscotch_map.h"

namespace gs2d {

class BitmapFontManager
{
	BitmapFontPtr SeekBitmapFont(Video* video, const str_type::string& font);
	BitmapFontPtr LoadBitmapFont(Video* video, const str_type::string& fullFilePath);

	tsl::hopscotch_map<str_type::string, BitmapFontPtr> m_fonts;

public:

	void ClearFonts();

	math::Vector2 ComputeCarretPosition(
		Video* video,
		const str_type::string& font,
		const str_type::string& text,
		const unsigned int pos);

	math::Vector2 ComputeTextBoxSize(
		Video* video,
		const str_type::string& font,
		const str_type::string& text);

	unsigned int FindClosestCarretPosition(
		Video* video,
		const str_type::string& font,
		const str_type::string &text,
		const math::Vector2 &textPos,
		const math::Vector2 &reference);

	bool DrawBitmapText(
		Video* video,
		const math::Vector2 &v2Pos,
		const str_type::string& text,
		const str_type::string& font,
		const Color& color,
		const float scale = 1.0f);
};

} // namespace gs2d

#endif
