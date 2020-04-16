#include "BitmapFont.h"

#include "../Video.h"
#include "../Unicode/utf8/utf8.h"

#include "../Sprite.h"

#ifdef _MSC_VER
  #define GS2D_SSCANF sscanf_s
#else
  #define GS2D_SSCANF sscanf
#endif

namespace gs2d {
using namespace gs2d::math;


const std::string BitmapFont::COLOR_CODE_BEGIN_SEQUENCE = ("!#color#");
const std::string BitmapFont::COLOR_CODE_END_SEQUENCE   = ("##!");

void BitmapFont::RemoveColorMarkup(std::string& str)
{
	std::size_t pos = 0;
	while ((pos = str.find(COLOR_CODE_BEGIN_SEQUENCE, pos)) != std::string::npos)
	{
		std::size_t end = str.find(COLOR_CODE_END_SEQUENCE, pos);
		if (end != std::string::npos)
		{
			str.erase(pos, (end - pos) + COLOR_CODE_END_SEQUENCE.length());
		}
	}
}

std::string BitmapFont::AssembleColorCode(const Color& color)
{
	std::stringstream ss;
	ss << COLOR_CODE_BEGIN_SEQUENCE << color.To32BitARGB() << COLOR_CODE_END_SEQUENCE;
	return ss.str();
}

bool BitmapFont::IsColorCode(const std::string& text, const std::size_t pos)
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
	page(0),
	available(false)
{
}

BitmapFont::CHARSET::CHARSET() :
	paddingDown(0),
	paddingUp(0),
	paddingLeft(0),
	paddingRight(0)
{
}

BitmapFont::BitmapFont(Video* video, const std::string& fileName, const std::string& str)
{
	if (ParseFNTString(str))
	{
		m_bitmaps.resize(m_charSet.textureNames.size());
		for (unsigned int t = 0; t < m_charSet.textureNames.size(); t++)
		{
			std::string path = fileName;
			std::size_t found = path.find_last_of(("/\\"));
			if (found != std::string::npos)
				path.resize(found + 1);

			// remove "'s from the texture name
			while ((found = m_charSet.textureNames[t].find(("\""))) != std::string::npos)
			{
				m_charSet.textureNames[t].erase(found, 1);
			}

			path += m_charSet.textureNames[t];
			m_bitmaps[t] = SpritePtr(new Sprite(video, path, 1.0f));
	
			if (!m_bitmaps[t])
			{
				m_bitmaps.clear();
				break;
			}
		}
	}
}

bool BitmapFont::ParseFNTString(const std::string& str)
{
	if (str == (""))
		return false;

	std::stringstream stream;
	stream << str;

	std::string line;
	std::string read, key, value;
	std::size_t i;
	while( !stream.eof() )
	{
		std::stringstream lineStream;
		std::getline( stream, line );
		lineStream << line;

		//read the line's type
		lineStream >> read;
		if( read == ("common") )
		{
			//this holds common data
			while( !lineStream.eof() )
			{
				std::stringstream converter;
				lineStream >> read;
				i = read.find( ('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;
				if( key == ("lineHeight") )
					converter >> m_charSet.lineHeight;
				else if( key == ("base") )
					converter >> m_charSet.base;
				else if( key == ("scaleW") )
					converter >> m_charSet.width;
				else if( key == ("scaleH") )
					converter >> m_charSet.height;
				else if( key == ("pages") )
					converter >> m_charSet.pages;
			}
		}
		else if( read == ("char") )
		{
			//this is data for a specific char
			unsigned short charID = 0;

			while( !lineStream.eof() )
			{
				std::stringstream converter;
				lineStream >> read;
				i = read.find( ('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;
				if( key == ("id") )
					converter >> charID;
				else if( key == ("x") )
					converter >> m_charSet.chars[charID].x;
				else if( key == ("y") )
					converter >> m_charSet.chars[charID].y;
				else if( key == ("width") )
					converter >> m_charSet.chars[charID].width;
				else if( key == ("height") )
					converter >> m_charSet.chars[charID].height;
				else if( key == ("xoffset") )
					converter >> m_charSet.chars[charID].xOffset;
				else if( key == ("yoffset") )
					converter >> m_charSet.chars[charID].yOffset;
				else if( key == ("xadvance") )
					converter >> m_charSet.chars[charID].xAdvance;
				else if( key == ("page") )
					converter >> m_charSet.chars[charID].page;

				m_charSet.chars[charID].available = true;
			}
		}
		else if( read == ("page") )
		{
			//this holds page data
			while( !lineStream.eof() )
			{
				std::stringstream converter;
				lineStream >> read;
				i = read.find( ('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;

				// TODO: read also the 'id' for safety. Id's must start from 0 and be incremented 1 by 1
				// if id numbers aren't right, the user must be warned.
				if( key == ("file") )
				{
					std::string textureName;
					converter >> textureName;
					m_charSet.textureNames.push_back(textureName);
					break;
				}
			}
		}
		else if( read == ("info") )
		{
			//this holds page data
			while( !lineStream.eof() )
			{
				std::stringstream converter;
				lineStream >> read;
				i = read.find( ('=') );
				key = read.substr( 0, i );
				value = read.substr( i + 1 );

				//assign the correct value
				converter << value;
				std::string subStr;
				if( key == ("padding") )
				{
					// TODO do not duplicate
					{
						std::stringstream converter;
						i = value.find((','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingUp;
						value = value.substr(i + 1);
					}
					{
						std::stringstream converter;
						i = value.find((','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingRight;
						value = value.substr(i + 1);
					}
					{
						std::stringstream converter;
						i = value.find((','));
						subStr = value.substr(0, i);
						converter << subStr;
						converter >> m_charSet.paddingDown;
						value = value.substr(i + 1);
					}
					{
						std::stringstream converter;
						i = value.find((','));
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

unsigned int BitmapFont::FindClosestCarretPosition(const std::string& text, const Vector2& textPos, const Vector2& reference)
{
	std::string cleanText = text;
	RemoveColorMarkup(cleanText);
	
	const std::size_t cursors = cleanText.length() + 1;
	float distance =-1;
	unsigned int returnCursor = 0;
	for (unsigned int t = 0; t < cursors; ++t)
	{
		const Vector2 pos = ComputeCarretPosition(cleanText, t) + textPos;
		if (distance == -1)
		{
			distance = Vector2::Distance(reference, pos);
		}
		else
		{
			const float tempDistance = Vector2::Distance(reference, pos);
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

		// find how many bytes are in this character
		while (!utf8::is_valid(character, character + (charByteCount)) && charByteCount < maxByteCount)
		{
			++charByteCount;
		}
  
  		if (!utf8::is_valid(character, character + charByteCount))
		{
			index = 0;
		}
		else if (charByteCount <= maxByteCount)
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
	return Min(index, static_cast<int>(GS2D_CHARSET_MAX_CHARS - 1));
}

Vector2 BitmapFont::ComputeCarretPosition(const std::string& text, const unsigned int pos)
{
	std::string cleanText = text;
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
		if (cleanText[t] == ('\n'))
		{
			cursor.y += m_charSet.lineHeight;
			continue;
		}
		else if (cleanText[t] == ('\r'))
		{
			continue;
		}

		int charId = ConvertCharacterToIndex<char>(&cleanText[t], t, length);

		if (!m_charSet.chars[charId].available)
			charId = 63; // question mark

		const CHAR_DESCRIPTOR& currentChar = m_charSet.chars[charId];
		cursor.x += currentChar.xAdvance;
	}
	return cursor;
}

Vector2 BitmapFont::ComputeTextBoxSize(const std::string& text)
{
	std::string cleanText = text;
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
		if (cleanText[t] == ('\n'))
		{
			lineWidth = 0.0f;
			cursor.y += m_charSet.lineHeight;
			continue;
		}
		else if (cleanText[t] == ('\r'))
		{
			continue;
		}

		int charId = ConvertCharacterToIndex<char>(&cleanText[t], t, length);

		if (!m_charSet.chars[charId].available)
			charId = 63; // question mark

		const CHAR_DESCRIPTOR &currentChar = m_charSet.chars[charId];
		lineWidth += currentChar.xAdvance;

		cursor.x = Max(cursor.x, lineWidth);
	}
	return cursor;
}

Vector2 BitmapFont::DrawBitmapText(const Vector2& pos, const std::string& text, const Color& color, const float scale)
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
		if (text[t] == ('\n'))
		{
			cursor.x = pos.x;
			cursor.y += m_charSet.lineHeight * scale;
			continue;
		}
		else if (text[t] == ('\r'))
		{
			continue;
		}

		// check color code
		if (IsColorCode(text, t))
		{
			const std::size_t end = text.find(COLOR_CODE_END_SEQUENCE, t);
			if (end != std::string::npos)
			{
				const std::size_t codeStartPos = t + COLOR_CODE_BEGIN_SEQUENCE.length();
				const std::string colorCodeValue = text.substr(codeStartPos, end);
				unsigned long localColor;
				if (GS2D_SSCANF(colorCodeValue.c_str(), ("%lu"), &localColor) == 1)
				{
					const Vector4 currentColorV4(Color(color) * Color(static_cast<uint32_t>(localColor)));
					currentColor = Color(currentColorV4);
				}
				t = (end) + COLOR_CODE_END_SEQUENCE.length() - 1;
				continue;
			}
		}

		// find mapped character
		const bool isSpace = (text[t] == (' '));
		int charId = ConvertCharacterToIndex<char>(&text[t], t, length);

  		if (!m_charSet.chars[charId].available)
			charId = 63; // question mark

		const CHAR_DESCRIPTOR& currentChar = m_charSet.chars[charId];
		if (currentChar.width > 0 && currentChar.height > 0 && !isSpace)
		{
			const int currentPage = currentChar.page;

			Rect2D rect;
			const Vector2 bitmapSize = bitmapsPointers[currentPage]->GetSize(Rect2D());
			rect.pos = Vector2(currentChar.x, currentChar.y) / bitmapSize;
			rect.size = Vector2(currentChar.width, currentChar.height) / bitmapSize;

			const Vector2 charPos =	cursor + Vector2(currentChar.xOffset, currentChar.yOffset) * scale;

			assert(currentPage >= 0);
			assert(currentPage < m_charSet.pages);

			// will only swap textures if it has to use a different page for this character
			if (lastPageUsed != currentPage)
			{
				if (lastPageUsed >= 0)
				{
					bitmapsPointers[lastPageUsed]->EndFastDraw();
				}
				bitmapsPointers[currentPage]->BeginFastDraw();
			}

   			bitmapsPointers[currentPage]->FastDraw(
				Vector3(charPos, 0.0f),
				Vector2(currentChar.width, currentChar.height) * scale,
				Vector2(0.0f),
				currentColor,
				rect);

			lastPageUsed = currentPage;
		}
		cursor.x += currentChar.xAdvance * scale;
	}

	// end the rendering for the last char drawn
	if (lastPageUsed >= 0)
	{
		bitmapsPointers[lastPageUsed]->EndFastDraw();
	}

	cursor.y += m_charSet.lineHeight;
	return cursor;
}

} // namespace gs2d
