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
#include "../../Video/GLES2/GLES2Video.h"
#include "AndroidLog.h"

namespace gs2d {

void ShowMessage(str_type::stringstream& stream, const GS_MESSAGE_TYPE type)
{
	if (type == GSMT_INFO)
	{
		LOGI(stream.str().c_str());
	}
	else if (type == GSMT_WARNING)
	{
		LOGI(stream.str().c_str());
	}
	else if (type == GSMT_ERROR)
	{
		LOGE(stream.str().c_str());
	}
}

str_type::string GLES2Video::GetPlatformName() const
{
	return GS_L("android");
}

} // namespace gs2d

namespace Platform {

gs2d::str_type::string FileLogger::GetLogDirectory()
{
	return GS_L("/sdcard/.ethanon/gs2dlog/");
}

gs2d::str_type::string GetModuleDirectory()
{
	return GS_L("");
}

char GetDirectorySlashA()
{
	return '/';
}

} // namespace Platform
