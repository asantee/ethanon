#ifndef GS2D_BITMAP_FONT_H_
#define GS2D_BITMAP_FONT_H_

#include "../Types.h"

#include "../Math/GameMath.h"

#include "../Math/Color.h"

#include <vector>

namespace gs2d {

#define GS2D_CHARSET_MAX_CHARS (65536)

/**
 * \brief Stores and renders bitmap fonts
 *
 * The BitmapFont class parses text fnt files generated with AngelCode's bitmap
 * font generator and handles its rendering. The parser code was originally written by Promit
 * posted in this thread: http://www.gamedev.net/community/forums/topic.asp?topic_id=330742
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
  		bool available;

		CHAR_DESCRIPTOR();
	};

	struct CHARSET
	{
		CHARSET();
		float lineHeight;
		float base;
		float width, height;
		int pages;
		CHAR_DESCRIPTOR chars[GS2D_CHARSET_MAX_CHARS];
		std::vector<str_type::string> textureNames;
		float paddingUp, paddingRight, paddingDown, paddingLeft;
	} m_charSet;

	bool ParseFNTString(const str_type::string& str);

	std::vector<SpritePtr> m_bitmaps;

    static const str_type::string COLOR_CODE_BEGIN_SEQUENCE;
    static const str_type::string COLOR_CODE_END_SEQUENCE;
    static void RemoveColorMarkup(str_type::string& str);
    static bool IsColorCode(const str_type::string& text, const std::size_t pos);

public:
	bool IsLoaded() const;
	BitmapFont(
		Video* video,
		const str_type::string& fileName,
		const str_type::string& str);

	math::Vector2 DrawBitmapText(
		const math::Vector2& pos,
		const str_type::string& text,
		const Color& color,
		const float scale = 1.0f);

	math::Vector2 ComputeTextBoxSize(const str_type::string& text);
	math::Vector2 ComputeCarretPosition(const str_type::string& text, const unsigned int pos);

    static str_type::string AssembleColorCode(const Color& color);

	unsigned int FindClosestCarretPosition(
		const str_type::string& text,
		const math::Vector2& textPos,
		const math::Vector2& reference);
};

typedef boost::shared_ptr<BitmapFont> BitmapFontPtr;

}

#endif
