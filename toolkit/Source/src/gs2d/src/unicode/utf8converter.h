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

namespace utf8 {

class Converter
{
public:

	Converter(const char * szData);
	Converter(const std::string& sData);
	Converter(const wchar_t * wcsData);
	Converter(const std::wstring& wsData);
	const char* c_str() const;
	const wchar_t* wc_str() const;
	const std::string& str() const;
	const std::wstring& wstr() const;
	std::string::size_type length() const;

private:
	void SetData(const char * szData);
	void SetWideData(const wchar_t * wcsData);

	std::string ansidata;
	std::wstring widedata;
};

typedef Converter c;

} // utf8

#endif
