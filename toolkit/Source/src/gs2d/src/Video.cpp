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

} // namespace gs2d
