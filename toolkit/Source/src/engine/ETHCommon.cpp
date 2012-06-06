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

#include "ETHCommon.h"
#include <string.h>
#include <enml/enml.h>

ETH_VIDEO_MODE::ETH_VIDEO_MODE()
{
	width = height = 0;
	pf = GSPF_UNKNOWN;
}

ETH_WINDOW_ENML_FILE::ETH_WINDOW_ENML_FILE(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager) :
	width(640),
	height(480),
	windowed(true),
	vsync(true),
	title(GS_L("Ethanon Engine")),
	richLighting(true)
{
	str_type::string out;
	fileManager->GetAnsiFileString(fileName, out);
	enml::File file(out);
	if (file.getError() == enml::enmlevSUCCESS)
	{
		file.getUInt(GS_L("window"), GS_L("width"), &width);
		file.getUInt(GS_L("window"), GS_L("height"), &height);
		windowed = ETHGlobal::IsTrue(file.get(GS_L("window"), GS_L("windowed")));
		vsync = ETHGlobal::IsTrue(file.get(GS_L("window"), GS_L("vsync")));
		const str_type::string newTitle = file.get(GS_L("window"), GS_L("title"));
		richLighting = ETHGlobal::IsTrue(file.get(GS_L("rendering"), GS_L("richLighting")));
		title = newTitle.empty() ? title : newTitle;

		densityManager.FillParametersFromFile(file);
	}
	else
	{
		#ifdef GS2D_STR_TYPE_WCHAR
		std::wcerr
		#else
		std::cerr
		#endif
		 << file.getErrorString() << std::endl;
	}
}

ETH_STARTUP_RESOURCES_ENML_FILE::ETH_STARTUP_RESOURCES_ENML_FILE(const str_type::string& fileName, const Platform::FileManagerPtr& fileManager)
{
	emtprojFilename = GS_L("");
	escFilename = GS_L("");

	str_type::string out;
	fileManager->GetAnsiFileString(fileName, out);
	enml::File file(out);
	if (file.getError() == enml::enmlevSUCCESS)
	{
		emtprojFilename = file.get(GS_L("startup"), GS_L("project"));
		escFilename = file.get(GS_L("startup"), GS_L("scene"));
	}
	else
	{
		#ifdef GS2D_STR_TYPE_WCHAR
		std::wcerr
		#else
		std::cerr
		#endif
		 << file.getErrorString() << std::endl;
	}
}
