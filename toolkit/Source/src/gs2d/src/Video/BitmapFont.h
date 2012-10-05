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

#ifndef GS2D_BITMAP_FONT_H_
#define GS2D_BITMAP_FONT_H_

#include "../gs2dmath.h"

#include <vector>

namespace gs2d {

/**
 * \brief Stores and renders bitmap fonts
 *
 * This class parses text fnt files generated with the AngelCode's bitmap
 * font generator and renders it. The parser code was originally written by Promit
 * and posted in this thread: http://www.gamedev.net/community/forums/topic.asp?topic_id=330742
 */
class BitmapFont
{
	struct CHAR_DESCRIPTOR
	{
		//clean 16 bytes
		float x, y;
		float width, height;
		float xOffset, yOffset;
		float xAdvance;
		int page;

		CHAR_DESCRIPTOR() : x(0), y(0), width(0), height(0), xOffset(0), yOffset(0),
			xAdvance(0), page(0)
		{ }
	};

	struct CHARSET
	{
		CHARSET() : paddingDown(0), paddingUp(0), paddingLeft(0), paddingRight(0) {}
		float lineHeight;
		float base;
		float width, height;
		int pages;
		CHAR_DESCRIPTOR chars[65536];
		std::vector<str_type::string> textureNames;
		float paddingUp, paddingRight, paddingDown, paddingLeft;
	} m_charSet;

	bool ParseFNTString(const str_type::string& str);

	std::vector<SpritePtr> m_bitmaps;

public:
	bool IsLoaded() const;
	BitmapFont(
		VideoWeakPtr video,
		const str_type::string& fileName,
		const str_type::string& str);

	math::Vector2 DrawBitmapText(
		const math::Vector2 &pos,
		const str_type::string& text,
		const Color& color,
		const float scale = 1.0f);

	math::Vector2 ComputeTextBoxSize(const str_type::string &text);
	math::Vector2 ComputeCarretPosition(const str_type::string &text, const unsigned int pos);

	unsigned int FindClosestCarretPosition(
		const str_type::string &text,
		const math::Vector2 &textPos,
		const math::Vector2 &reference);
};

typedef boost::shared_ptr<BitmapFont> BitmapFontPtr;

}

#endif
