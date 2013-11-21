/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "BitmapFont.h"
#include "../Video.h"
#include "../Unicode/utf8/utf8.h"

namespace gs2d {
using namespace gs2d::math;


const str_type::string BitmapFont::COLOR_CODE_BEGIN_SEQUENCE = GS_L("!#color#");
const str_type::string BitmapFont::COLOR_CODE_END_SEQUENCE   = GS_L("##!");

void BitmapFont::RemoveColorMarkup(str_type::string& str)
{
	std::size_t pos = 0;
	while ((pos = str.find(COLOR_CODE_BEGIN_SEQUENCE, pos)) != str_type::string::npos)
	{
		std::size_t end = str.find(COLOR_CODE_END_SEQUENCE, pos);
		if (end != str_type::string::npos)
		{
			str.erase(pos, (end - pos) + COLOR_CODE_END_SEQUENCE.length());
		}
	}
}

str_type::string BitmapFont::AssembleColorCode(const Color& color)
{
	str_type::stringstream ss;
	ss << COLOR_CODE_BEGIN_SEQUENCE << color.color << COLOR_CODE_END_SEQUENCE;
	return ss.str();
}

bool BitmapFont::IsColorCode(const str_type::string& text, const std::size_t pos)
{
	if (COLOR_CODE_BEGIN_SEQUENCE[0] == text[pos])
	{
		return (COLOR_CODE_BEGIN_SEQUENCE == text.substr(pos, COLOR_CODE_BEGIN_SEQUENCE.length()));
	}
	else
	{
		return false;
	}
}

BitmapFont::CHAR_DESCRIPTOR::CHAR_DESCRIPTOR() :
	x(0),
	y(0),
	width(0),
	height(0),
	xOffset(0),
	yOffset(0),
	xAdvance(0),
	page(0)
{
}

BitmapFont::CHARSET::CHARSET() :
	paddingDown(0),
	paddingUp(0),
	paddingLeft(0),
	paddingRight(0)
{
}

BitmapFont::BitmapFont(Video* video, const str_type::string& fileName, const str_type::string& str)
{
	if (ParseFNTString(str))
	{
		m_bitmaps.resize(m_charSet.textureNames.size());
		for (unsigned int t = 0; t < m_charSet.textureNames.size(); t++)
		{
			str_type::string path = fileName;
			std::size_t found = path.find_last_of(GS_L("/\\"));
			if (found != str_type::string::npos)
				path.resize(found + 1);

			// remove "'s from the texture name
			while ((found = m_charSet.textureNames[t].find(GS_L("\""))) != str_type::string::npos)
			{
				m_charSet.textureNames[t].erase(found, 1);
			}

			path += m_charSet.textureNames[t];
			m_bitmaps[t] = video->CreateSprite(path, gs2d::constant::ZERO, 0, 0);
			if (!m_bitmaps[t])
			{
				m_bitmaps.clear();
				break;
			}
			else
			{
				m_bitmaps[t]->SetOrigin(Sprite::EO_DEFAULT);
			}
		}
	}
}

bool BitmapFont::ParseFNTString(const str_type::string& str)
{
	if (str == GS_L(""))
		return false;

	str_type::stringstream stream;
	stream << str;

	str_type::string line;
	str_type::string read, key, value;
	std::size_t i;
	while( !stream.eof() )
	{
		str_type::stringstream lineStream;
		std::getline( stream, line );
		lineStream << line;

		//read the line's type
		lineStream >> read;
		if( read == GS_L("common") )
		{
			//this holds common data
			while( !lineStream.eof() )
			{
				str_type::stringstream converter;
				lineStream >> read;
				i = read.find( GS_L('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;
				if( key == GS_L("lineHeight") )
					converter >> m_charSet.lineHeight;
				else if( key == GS_L("base") )
					converter >> m_charSet.base;
				else if( key == GS_L("scaleW") )
					converter >> m_charSet.width;
				else if( key == GS_L("scaleH") )
					converter >> m_charSet.height;
				else if( key == GS_L("pages") )
					converter >> m_charSet.pages;
			}
		}
		else if( read == GS_L("char") )
		{
			//this is data for a specific char
			unsigned short charID = 0;

			while( !lineStream.eof() )
			{
				str_type::stringstream converter;
				lineStream >> read;
				i = read.find( GS_L('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;
				if( key == GS_L("id") )
					converter >> charID;
				else if( key == GS_L("x") )
					converter >> m_charSet.chars[charID].x;
				else if( key == GS_L("y") )
					converter >> m_charSet.chars[charID].y;
				else if( key == GS_L("width") )
					converter >> m_charSet.chars[charID].width;
				else if( key == GS_L("height") )
					converter >> m_charSet.chars[charID].height;
				else if( key == GS_L("xoffset") )
					converter >> m_charSet.chars[charID].xOffset;
				else if( key == GS_L("yoffset") )
					converter >> m_charSet.chars[charID].yOffset;
				else if( key == GS_L("xadvance") )
					converter >> m_charSet.chars[charID].xAdvance;
				else if( key == GS_L("page") )
					converter >> m_charSet.chars[charID].page;
			}
		}
		else if( read == GS_L("page") )
		{
			//this holds page data
			while( !lineStream.eof() )
			{
				str_type::stringstream converter;
				lineStream >> read;
				i = read.find( GS_L('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;

				// TODO: read also the 'id' for safety. Id's must start from 0 and be incremented 1 by 1
				// if id numbers aren't right, the user must be warned.
				if( key == GS_L("file") )
				{
					str_type::string textureName;
					converter >> textureName;
					m_charSet.textureNames.push_back(textureName);
					break;
				}
			}
		}
		else if( read == GS_L("info") )
		{
			//this holds page data
			while( !lineStream.eof() )
			{
				str_type::stringstream converter;
				lineStream >> read;
				i = read.find( GS_L('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;
				str_type::string subStr;
				if( key == GS_L("padding") )
				{
					// TODO do not duplicate
					{
						str_type::stringstream converter;
						i = value.find(GS_L(','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingUp;
						value = value.substr(i + 1);
					}
					{
						str_type::stringstream converter;
						i = value.find(GS_L(','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingRight;
						value = value.substr(i + 1);
					}
					{
						str_type::stringstream converter;
						i = value.find(GS_L(','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingDown;
						value = value.substr(i + 1);
					}
					{
						str_type::stringstream converter;
						i = value.find(GS_L(','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingLeft;
						value = value.substr(i + 1);
					}
				}
			}
		}
	}
	//stream.close();
	return true;
}

bool BitmapFont::IsLoaded() const
{
	return (!m_bitmaps.empty());
}

unsigned int BitmapFont::FindClosestCarretPosition(const str_type::string& text, const Vector2& textPos, const Vector2& reference)
{
	str_type::string cleanText = text;
	RemoveColorMarkup(cleanText);
	
	const std::size_t cursors = cleanText.length() + 1;
	float distance =-1;
	unsigned int returnCursor = 0;
	for (unsigned int t = 0; t < cursors; ++t)
	{
		const Vector2 pos = ComputeCarretPosition(cleanText, t) + textPos;
		if (distance == -1)
		{
			distance = Distance(reference, pos);
		}
		else
		{
			const float tempDistance = Distance(reference, pos);
			if (tempDistance < distance)
			{
				distance = tempDistance;
				returnCursor = t;
			}
		}
	}
	return returnCursor;
}

template<class TChar>
inline int ConvertCharacterToIndex(const TChar* character, std::size_t& t, const std::size_t length)
{
	int index = 0;
	if (utf8::is_valid(character, character + 1))
	{
		static const int bitsInByte = 8;
		static const int maxTCharValue = 1 << bitsInByte * sizeof(TChar);
		index = static_cast<int>(character[0]);
		if (index < 0)
		{
			index += maxTCharValue;
		}
		assert(index < maxTCharValue);
	}
	else
	{
        const std::size_t maxByteCount = math::Min(static_cast<std::size_t>(8), length - t);
		std::size_t charByteCount = 2;
		
		while (!utf8::is_valid(character, character + (charByteCount)) && charByteCount < maxByteCount)
		{
			++charByteCount;
		}

		if (charByteCount <= maxByteCount)
		{
			std::vector<unsigned long> utf32line;
			utf8::utf8to32(character, character + charByteCount, std::back_inserter(utf32line));
			index = static_cast<int>(utf32line[0]);

			t += (charByteCount - 1);
		}
		else
		{
			index = 0;
		}
	}
	return math::Min(index, static_cast<int>(GS2D_CHARSET_MAX_CHARS - 1));
}

Vector2 BitmapFont::ComputeCarretPosition(const str_type::string& text, const unsigned int pos)
{
	str_type::string cleanText = text;
	RemoveColorMarkup(cleanText);

	if (!IsLoaded())
	{
		return Vector2(0,0);
	}

	// seek the cursor position or the last character
	const std::size_t length = Min(cleanText.size(), static_cast<std::size_t>(pos));

	Vector2 cursor = Vector2(0,0);
	for (std::size_t t = 0; t < length; t++)
	{
		if (cleanText[t] == GS_L('\n'))
		{
			cursor.y += m_charSet.lineHeight;
			continue;
		}

		int charId = ConvertCharacterToIndex<str_type::char_t>(&cleanText[t], t, length);
		const CHAR_DESCRIPTOR& currentChar = m_charSet.chars[charId];
		cursor.x += currentChar.xAdvance;
	}
	return cursor;
}

Vector2 BitmapFont::ComputeTextBoxSize(const str_type::string& text)
{
	str_type::string cleanText = text;
	RemoveColorMarkup(cleanText);

	if (!IsLoaded())
	{
		return Vector2(0,0);
	}
	const std::size_t length = cleanText.size();
	Vector2 cursor = Vector2(0,m_charSet.lineHeight);
	float lineWidth = 0.0f;
	for (std::size_t t = 0; t < length; t++)
	{
		if (cleanText[t] == GS_L('\n'))
		{
			lineWidth = 0.0f;
			cursor.y += m_charSet.lineHeight;
			continue;
		}
		int charId = ConvertCharacterToIndex<str_type::char_t>(&cleanText[t], t, length);
		const CHAR_DESCRIPTOR &currentChar = m_charSet.chars[charId];
		lineWidth += currentChar.xAdvance;

		cursor.x = Max(cursor.x, lineWidth);
	}
	return cursor;
}

Vector2 BitmapFont::DrawBitmapText(const Vector2& pos, const str_type::string& text, const Color& color, const float scale)
{
	if (!IsLoaded())
	{
		return Vector2(0,0);
	}

	const std::size_t length = text.size();
    
	Vector2 cursor = pos;

	std::vector<Sprite*> bitmapsPointers(m_bitmaps.size());
	for (std::size_t t = 0; t < bitmapsPointers.size(); t++)
		bitmapsPointers[t] = m_bitmaps[t].get();

	Color currentColor = color;

	int lastPageUsed =-1;
	for (std::size_t t = 0; t < length; t++)
	{
		if (text[t] == GS_L('\n'))
		{
			cursor.x = pos.x;
			cursor.y += m_charSet.lineHeight * scale;
			continue;
		}
		
		// check color code
		if (IsColorCode(text, t))
		{
			const std::size_t end = text.find(COLOR_CODE_END_SEQUENCE, t);
			if (end != str_type::string::npos)
			{
				const std::size_t codeStartPos = t + COLOR_CODE_BEGIN_SEQUENCE.length();
				const str_type::string colorCodeValue = text.substr(codeStartPos, end);
				Color localColor;
				if (GS2D_SSCANF(colorCodeValue.c_str(), GS_L("%lu"), &localColor.color) == 1)
				{
					const Vector4 currentColorV4(Vector4(color) * Vector4(localColor));
					currentColor = math::ConvertToDW(currentColorV4);
				}
				t = (end) + COLOR_CODE_END_SEQUENCE.length() - 1;
				continue;
			}
		}

		// find mapped character
		int charId = ConvertCharacterToIndex<str_type::char_t>(&text[t], t, length);
		const CHAR_DESCRIPTOR &currentChar = m_charSet.chars[charId];
		if (currentChar.width > 0 && currentChar.height > 0)
		{
			Rect2Df rect;
			rect.pos = Vector2(currentChar.x, currentChar.y);
			rect.size = Vector2(currentChar.width, currentChar.height);

			const Vector2 charPos =	cursor + Vector2(currentChar.xOffset, currentChar.yOffset) * scale;

			const int currentPage = currentChar.page;

			assert(currentPage >= 0);
			assert(currentPage < m_charSet.pages);

			// will only swap textures if it has to use a different page for this character
			if (lastPageUsed != currentPage)
			{
				if (lastPageUsed >= 0)
				{
					bitmapsPointers[lastPageUsed]->EndFastRendering();
				}
				bitmapsPointers[currentPage]->BeginFastRendering();
			}

			bitmapsPointers[currentPage]->SetRect(rect);
			bitmapsPointers[currentPage]->DrawShapedFast(charPos, Vector2(currentChar.width, currentChar.height) * scale, currentColor);

			lastPageUsed = currentPage;
		}
		cursor.x += currentChar.xAdvance * scale;
	}

	// end the rendering for the last char drawn
	if (lastPageUsed >= 0)
	{
		bitmapsPointers[lastPageUsed]->EndFastRendering();
	}

	cursor.y += m_charSet.lineHeight;
	return cursor;
}

} // namespace gs2d
