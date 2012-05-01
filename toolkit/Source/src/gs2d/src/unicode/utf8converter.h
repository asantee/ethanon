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

#ifndef ETH_UTF8STRING_H_
#define ETH_UTF8STRING_H_

#include <xstring>

namespace utf8
{
	class converter
	{
	private:
		std::string ansidata;
		std::wstring widedata;

	public:

		// this is a stub to convert any char sequence to wstring and vice versa
//#define UNSAFE_UTF8_DISABLE_CHECKS	// just comment it and compile to see where are you have non-safe strings conversion
#if defined(UNSAFE_UTF8_DISABLE_CHECKS) || defined(UNSAFE_UTF8_ALLOWED) // UNSAFE_UTF8_ALLOWED used to don't verify Editor
		#pragma message("WARNING: possible treating of invalid string data as UTF8 while conversion to Unicode")
		converter()
		{
		}

		converter(const char * szData)
		{
			SetData(szData);
		}
		converter(const std::string& sData)
		{
			SetData(sData.c_str());
		}
		converter(const wchar_t * wcsData)
		{
			SetWideData(wcsData);
		}
		converter(const std::wstring& wsData)
		{
			SetWideData(wsData.c_str());
		}

		const char* c_str() const
		{
			return ansidata.c_str();
		}

		const wchar_t* wc_str() const
		{
			return widedata.c_str();
		}

		const std::string& str() const
		{
			return ansidata;
		}

		const std::wstring& wstr() const
		{
			return widedata;
		}

		std::string::size_type length() const
		{
			//if (widedata.length() != ansidata.length()) _asm { int 3 }; // temporary debug test stuff
			return ansidata.length();
		}
#else
	private:
		converter()
		{
		}
#endif

	private:
		void SetData(const char * szData);
		void SetWideData(const wchar_t * wcsData);
	};

#if defined(UNSAFE_UTF8_ALLOWED)
	// this is a short notation for utf8::converter, it should be removed after full implementation of I18N
	// use full notation for normal conversions
	typedef converter c;
#endif
} 
// namespace ETHPlatform

#endif