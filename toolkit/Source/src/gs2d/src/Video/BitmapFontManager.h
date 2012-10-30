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

#ifndef GS2D_BITMAP_FONT_MANAGER_H_
#define GS2D_BITMAP_FONT_MANAGER_H_

#include "BitmapFont.h"
#include <map>

namespace gs2d {

class BitmapFontManager
{
	BitmapFontPtr SeekBitmapFont(Video* video, const str_type::string& font);
	BitmapFontPtr LoadBitmapFont(Video* video, const str_type::string& fullFilePath);

	std::map<str_type::string, BitmapFontPtr> m_fonts;

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
