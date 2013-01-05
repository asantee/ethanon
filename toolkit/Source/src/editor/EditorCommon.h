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

#ifndef ETH_EDITORCOMMON_H_
#define ETH_EDITORCOMMON_H_

#include "../engine/ETHTypes.h"

#include <Platform/Platform.h>

#ifdef MACOSX
 #define EDITOR_GSK_CONTROL GSK_META
#else
 #define EDITOR_GSK_CONTROL GSK_CTRL
#endif

struct FILE_FORM_FILTER
{
	inline FILE_FORM_FILTER(const str_type::string& _title, const str_type::string& commaSeparatedExtensions) :
		title(_title),
		extensionsAllowed(Platform::SplitString(commaSeparatedExtensions, GS_L(",")))
	{
	}
	str_type::string title;
	std::vector<str_type::string> extensionsAllowed;
};

#endif
