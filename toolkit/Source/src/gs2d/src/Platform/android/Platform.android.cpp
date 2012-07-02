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

#include "Platform.android.h"
#include "../Platform.h"
#include "../FileLogger.h"

namespace Platform {

gs2d::str_type::string FileLogger::GetLogPath()
{
	return GS_L("/sdcard/.ethanon/gs2dlog/");
}

gs2d::str_type::string AddLastSlash(const gs2d::str_type::string& path)
{
	if (path.empty())
	{
		return GS_L("");
	}
	gs2d::str_type::string r = (path);
	FixSlashes(r);
	const std::size_t lastChar = r.size()-1;

	if (r.at(lastChar) == GS_L('\\'))
	{
		r[lastChar] = GS_L('/');
		return r;
	}
	else if (r.at(lastChar) != GS_L('/'))
	{
		return r + GS_L("/");
	}
	else
	{
		return r;
	}
}

gs2d::str_type::string& FixSlashes(gs2d::str_type::string& path)
{
	return path;
}

gs2d::str_type::string GetModulePath()
{
	return GS_L("");
}

} // namespace
