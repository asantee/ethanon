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

#include "BitmapFontManager.h"
#include "../Video.h"
#include "../Platform/Platform.h"

#include <fstream>

namespace gs2d {
using namespace gs2d::math;

void BitmapFontManager::ClearFonts()
{
	m_fonts.clear();
}

BitmapFontPtr BitmapFontManager::LoadBitmapFont(Video* video, const str_type::string& fullFilePath)
{
	str_type::string out;
	video->GetFileIOHub()->GetFileManager()->GetAnsiFileString(fullFilePath, out);
	if (!out.empty())
	{
		BitmapFontPtr newFont = BitmapFontPtr(new BitmapFont(video, fullFilePath, out));
		if (newFont->IsLoaded())
		{
			m_fonts[Platform::GetFileName(fullFilePath)] = newFont;
			video->Message(fullFilePath + GS_L(" bitmap font created."), GSMT_INFO);
			return newFont;
		}
		else
		{
			str_type::string errorMsg = GS_L("Invalid font file ");
			errorMsg += fullFilePath;
			video->Message(errorMsg, GSMT_WARNING);
			return BitmapFontPtr();
		}
	}
	else
	{
		str_type::string errorMsg = GS_L("Font file not found ");
		errorMsg += fullFilePath;
		video->Message(errorMsg, GSMT_WARNING);
		return BitmapFontPtr();
	}
}

BitmapFontPtr BitmapFontManager::SeekBitmapFont(Video* video, const str_type::string& font)
{
	std::map<str_type::string, BitmapFontPtr>::iterator iter = m_fonts.find(font);
	BitmapFontPtr bitmapFont;
	if (iter == m_fonts.end())
	{
		bitmapFont = LoadBitmapFont(video, video->GetFileIOHub()->GenerateBitmapFontFilePath(font));
		if (!bitmapFont)
		{
			video->Message(font + GS_L(": couldn't create bitmap font"), GSMT_ERROR);
		}
	}
	else
	{
		bitmapFont = iter->second;
	}
	return bitmapFont;	
}

Vector2 BitmapFontManager::ComputeCarretPosition(
	Video* video,
	const str_type::string& font,
	const str_type::string& text,
	const unsigned int pos)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(video, font);
	if (bitmapFont)
		return bitmapFont->ComputeCarretPosition(text, pos);
	else
		return Vector2(0,0);
}

Vector2 BitmapFontManager::ComputeTextBoxSize(
	Video* video,
	const str_type::string& font,
	const str_type::string& text)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(video, font);
	if (bitmapFont)
		return bitmapFont->ComputeTextBoxSize(text);
	else
		return Vector2(0,0);
}

unsigned int BitmapFontManager::FindClosestCarretPosition(
	Video* video,
	const str_type::string& font,
	const str_type::string& text,
	const Vector2& textPos,
	const Vector2& reference)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(video, font);
	if (bitmapFont)
		return bitmapFont->FindClosestCarretPosition(text, textPos, reference);
	else
		return 0;
}

bool BitmapFontManager::DrawBitmapText(
	Video* video,
	const Vector2& v2Pos,
	const str_type::string& text,
	const str_type::string& font,
	const Color& color,
	const float scale)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(video, font);
	if (bitmapFont)
	{
		bitmapFont->DrawBitmapText(v2Pos, text, color, scale);
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace gs2d
