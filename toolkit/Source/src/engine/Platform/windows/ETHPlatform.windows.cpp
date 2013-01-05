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

#ifdef WIN32

#include "../../ETHEngine.h"
#include "../ETHPlatform.h"
#include "../../../angelscript/include/angelscript.h"
#include <Platform/Platform.h>

#include <assert.h>
#include <windows.h>
#include <vector>
#include <Enml/Enml.h>

namespace ETHPlatform {

bool LoadDynamicLibrary(const str_type::char_t *filePath, asIScriptEngine *pASEngine)
{
	HINSTANCE hinstLib; 
	ETH_PLUGIN_PROC currentProc; 

	#ifdef GS2D_STR_TYPE_WCHAR
		hinstLib = LoadLibraryW(filePath);
	#else
		hinstLib = LoadLibraryA(filePath);
	#endif
 
	if (hinstLib != NULL) 
	{
		#ifdef GS2D_STR_TYPE_WCHAR
			currentProc = (ETH_PLUGIN_PROC)GetProcAddress(hinstLib, Platform::ConvertUnicodeToAscii(_ETH_PLUGIN_FUNCTION_NAME).c_str());
		#else
			currentProc = (ETH_PLUGIN_PROC)GetProcAddress(hinstLib, _ETH_PLUGIN_FUNCTION_NAME);
		#endif

		if (currentProc == NULL) 
		{
			std::cerr << "Invalid or non-existent function: " << _ETH_PLUGIN_FUNCTION_NAME << std::endl;
			return false;
		}
		else
		{
			(currentProc)(pASEngine); 
		}
		FreeLibrary(hinstLib); 
	}
	else
	{
		return false;
	}
	return true;
}

std::vector<str_type::string> splitString(str_type::string str, const str_type::string& c)
{
	std::vector<str_type::string> v;
	std::size_t pos;
	while ((pos = str.find(c)) != str_type::string::npos)
	{
		v.push_back(str.substr(0, pos));
		str = str.substr(pos + c.length(), str_type::string::npos);
		str.resize(str.length());
	}
	v.push_back(str);
	return v;
}

void LoadAllDynamicLibraries(const str_type::string& path, asIScriptEngine *pASEngine)
{
	const str_type::string content = enml::GetStringFromAnsiFile(path + _ETH_DYNAMIC_LIBRARY_LIST);
	if (content == GS_L(""))
		return;

	std::vector<str_type::string> v = splitString(content, GS_L("\n"));
	for (std::size_t t = 0; t < v.size(); t++)
	{
		LoadDynamicLibrary(str_type::string(path + v[t]).c_str(), pASEngine);
	}
}

}
// namespace ETHPlatform

#endif // WIN32
