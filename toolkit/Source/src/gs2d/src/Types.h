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
#if defined(ANDROID) || defined(APPLE_IOS) || defined(MACOSX)
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
 #if defined(__WIN32__) || defined(_MSC_VER)
  #include <xstring>
 #endif
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

#ifdef _MSC_VER
 #ifdef GS2D_STR_TYPE_ANSI
  #define GS2D_SSCANF sscanf_s
 #else
  #define GS2D_SSCANF swscanf_s
 #endif
#else
 #ifdef GS2D_STR_TYPE_ANSI
  #define GS2D_SSCANF sscanf
 #else
  #define GS2D_SSCANF swscanf
 #endif
#endif

class Texture;
typedef boost::shared_ptr<Texture> TexturePtr;
typedef boost::weak_ptr<Texture> TextureWeakPtr;

class Sprite;
typedef boost::shared_ptr<Sprite> SpritePtr;
typedef boost::weak_ptr<Texture> TextureWeakPtr;

class Video;
typedef boost::shared_ptr<Video> VideoPtr;
typedef boost::shared_ptr<const Video> VideoConstPtr;
typedef boost::weak_ptr<Video> VideoWeakPtr;

typedef unsigned long GS_DWORD;
typedef unsigned char GS_BYTE;

enum GS_MESSAGE_TYPE
{
	GSMT_INFO = 0,
	GSMT_WARNING = 1,
	GSMT_ERROR = 2
};

} // namespace gs2d
#endif
