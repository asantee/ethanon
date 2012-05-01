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

#ifndef GS2D_TYPES_H_
#define GS2D_TYPES_H_

#ifdef GS2D_EXPORT
 #define GS2D_API __declspec(dllexport)
#else
 #define GS2D_API
#endif

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp> 
#include <boost/any.hpp>

#define GS2D_UNUSED_ARGUMENT(argument) ((void)(argument))

#include <string>
#include <sstream>

namespace gs2d {

namespace str_type {
#if (ANDROID) || (APPLE_IOS)
 typedef std::string string;
 typedef std::stringstream stringstream;
 typedef char char_t;
 typedef std::ifstream ifstream;
 typedef std::ofstream ofstream;
 typedef std::ostringstream ostringstream;
 #define GS_L(x) x
 #define GS2D_STR_TYPE_ANSI
 #undef GS2D_STR_TYPE_WCHAR
 #define GS2D_COUT std::cout
 #define GS2D_CERR std::cerr
#else
 typedef std::wstring string;
 typedef std::wstringstream stringstream;
 typedef wchar_t char_t;
 typedef std::wifstream ifstream;
 typedef std::wofstream ofstream;
 typedef std::wostringstream ostringstream;
 #define GS_L(x) L##x
 #define GS2D_STR_TYPE_WCHAR
 #undef GS2D_STR_TYPE_ANSI
 #define GS2D_COUT std::wcout
 #define GS2D_CERR std::wcerr
#endif
} // namespace str

class Texture;
class Sprite;
typedef boost::shared_ptr<Texture> TexturePtr;
typedef boost::weak_ptr<Texture> TextureWeakPtr;

typedef boost::shared_ptr<Sprite> SpritePtr;
typedef boost::weak_ptr<Texture> TextureWeakPtr;

typedef unsigned long GS_DWORD;
typedef unsigned char GS_BYTE;

#pragma warning( push )
#pragma warning( disable : 4201 )

struct GS_COLOR
{
	GS_COLOR()
	{
		color = 0;
	}
	GS_COLOR(const GS_DWORD color)
	{
		this->color = color;
	}
	GS_COLOR(const GS_BYTE na, const GS_BYTE nr, const GS_BYTE ng, const GS_BYTE nb)
	{
		a=na;
		r=nr;
		g=ng;
		b=nb;
	}
	void SetColor(const GS_BYTE na, const GS_BYTE nr, const GS_BYTE ng, const GS_BYTE nb)
	{
		a=na;
		r=nr;
		g=ng;
		b=nb;
	}
	void SetColor(const GS_DWORD color)
	{
		this->color = color;
	}
	GS_COLOR &operator = (GS_DWORD color)
	{
		this->color = color;
		return *this;
	}
	operator GS_DWORD () const
	{
		return color;
	}
	void SetAlpha(const GS_BYTE na)
	{
		a=na;
	}
	void SetRed(const GS_BYTE nr)
	{
		r=nr;
	}
	void SetGreen(const GS_BYTE ng)
	{
		g=ng;
	}
	void SetBlue(const GS_BYTE nb)
	{
		b=nb;
	}

	union
	{
		struct
		{
			GS_BYTE b, g, r, a;
		};
		GS_DWORD color;
	};
};

#pragma warning( pop )

const GS_COLOR GS_ZERO(0x0);
const GS_COLOR GS_BLACK(0xFF000000);
const GS_COLOR GS_WHITE(0xFFFFFFFF);
const GS_COLOR GS_RED(0xFFFF0000);
const GS_COLOR GS_GREEN(0xFF00FF00);
const GS_COLOR GS_BLUE(0xFF0000FF);
const GS_COLOR GS_YELLOW(0xFFFFFF00);

enum GS_MESSAGE_TYPE
{
	GSMT_INFO = 0,
	GSMT_WARNING = 1,
	GSMT_ERROR = 2
};

} // namespace gs2d
#endif