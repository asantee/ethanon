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

BitmapFontPtr BitmapFontManager::LoadBitmapFont(Video* video, const std::string& fullFilePath)
{
	std::string out;
	video->GetFileIOHub()->GetFileManager()->GetAnsiFileString(fullFilePath, out);
	if (!out.empty())
	{
		BitmapFontPtr newFont = BitmapFontPtr(new BitmapFont(video, fullFilePath, out));
		if (newFont->IsLoaded())
		{
			const std::string fileName(Platform::GetFileName(fullFilePath));
			m_fonts[fileName] = newFont;
			video->Message(fileName + (" bitmap font created."), GSMT_INFO);
			return newFont;
		}
		else
		{
			std::string errorMsg = ("Invalid font file ");
			errorMsg += fullFilePath;
			video->Message(errorMsg, GSMT_WARNING);
			return BitmapFontPtr();
		}
	}
	else
	{
		std::string errorMsg = ("Font file not found ");
		errorMsg += fullFilePath;
		video->Message(errorMsg, GSMT_WARNING);
		return BitmapFontPtr();
	}
}

BitmapFontPtr BitmapFontManager::SeekBitmapFont(Video* video, const std::string& font)
{
	tsl::hopscotch_map<std::string, BitmapFontPtr>::iterator iter = m_fonts.find(font);
	BitmapFontPtr bitmapFont;
	if (iter == m_fonts.end())
	{
		bitmapFont = LoadBitmapFont(video, video->GetFileIOHub()->GenerateBitmapFontFilePath(font));
		if (!bitmapFont)
		{
			video->Message(font + (": couldn't create bitmap font"), GSMT_ERROR);
		}
	}
	else
	{
		bitmapFont = iter.value();
	}
	return bitmapFont;	
}

Vector2 BitmapFontManager::ComputeCarretPosition(
	Video* video,
	const std::string& font,
	const std::string& text,
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
	const std::string& font,
	const std::string& text)
{
	BitmapFontPtr bitmapFont = SeekBitmapFont(video, font);
	if (bitmapFont)
		return bitmapFont->ComputeTextBoxSize(text);
	else
		return Vector2(0,0);
}

unsigned int BitmapFontManager::FindClosestCarretPosition(
	Video* video,
	const std::string& font,
	const std::string& text,
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
	const std::string& text,
	const std::string& font,
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
