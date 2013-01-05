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

void Converter::SetData(const char* szData)
{
	ansidata = szData == 0 ? "" : szData;
}

Converter::Converter(const char* szData)
{
	SetData(szData);
}

Converter::Converter(const std::string& sData)
{
	SetData(sData.c_str());
}

const char* Converter::c_str() const
{
	return ansidata.c_str();
}

const gs2d::str_type::char_t* Converter::wc_str() const
{
	return ansidata.c_str();
}

const std::string& Converter::str() const
{
	return ansidata;
}

const gs2d::str_type::string& Converter::wstr() const
{
	return ansidata;
}

std::string::size_type Converter::length() const
{
	return ansidata.length();
}

} // namespace utf8
