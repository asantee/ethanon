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

#include "UTF8Converter.h"
#include "../Platform/Platform.h"

namespace utf8 {

void Converter::SetData(const char * szData)
{
	ansidata = szData == 0 ? "" : szData;
	widedata = Platform::ConvertUtf8ToUnicode(ansidata.c_str());
}

void Converter::SetWideData(const wchar_t * wcsData)
{
	widedata = wcsData == 0 ? L"" : wcsData;
	ansidata = Platform::ConvertUnicodeToUtf8(widedata.c_str());
}

Converter::Converter(const char * szData)
{
	SetData(szData);
}

Converter::Converter(const std::string& sData)
{
	SetData(sData.c_str());
}

Converter::Converter(const wchar_t * wcsData)
{
	SetWideData(wcsData);
}

Converter::Converter(const std::wstring& wsData)
{
	SetWideData(wsData.c_str());
}

const char* Converter::c_str() const
{
	return ansidata.c_str();
}

const wchar_t* Converter::wc_str() const
{
	return widedata.c_str();
}

const std::string& Converter::str() const
{
	return ansidata;
}

const std::wstring& Converter::wstr() const
{
	return widedata;
}

std::string::size_type Converter::length() const
{
	//if (widedata.length() != ansidata.length()) _asm { int 3 }; // temporary debug test stuff
	return ansidata.length();
}

} // namespace utf8

