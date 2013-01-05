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

#include "EditorBase.h"

#include "../engine/ETHTypes.h"
#include "../engine/Util/ETHASUtil.h"

#include <Unicode/UTF8Converter.h>

#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#define _ETH_SAFE_strcat strcat_s
#define _ETH_SAFE_sscanf sscanf_s
#define _ETH_SAFE_sprintf sprintf_s
inline void _ETH_SAFE_strcpy(char *szOut, const char *szIn)
{
	strcpy_s(szOut, strlen(szIn)+1, szIn);
}	
#else
#define _ETH_SAFE_strcat strcat
#define _ETH_SAFE_sscanf sscanf
#define _ETH_SAFE_sprintf sprintf
#define _ETH_SAFE_strcpy strcpy
#endif

bool DirectoryExists(const std::string& dir)
{
	if (_access(dir.c_str(), 0) == 0)
	{
		struct stat status;
		stat(dir.c_str(), &status);
		return true;
	}
	else
	{
		return false;
	}
}

static char* AssembleWin32FilterString(const FILE_FORM_FILTER& filter, std::string& out)
{
	str_type::char_t nullcharSymbol = GS_L('?');
	str_type::stringstream ss;
	ss << filter.title << nullcharSymbol;
	for (std::size_t t = 0; t < filter.extensionsAllowed.size(); t++)
	{
		ss << GS_L("*.") << filter.extensionsAllowed[t] << ((t < filter.extensionsAllowed.size() - 1) ? GS_L(";") : GS_L(""));
	}
	ss << nullcharSymbol << nullcharSymbol;

	out = utf8::c(ss.str()).str();
	const std::size_t initialSize = out.size();
	for (std::size_t t = 0; t < initialSize; t++)
	{
		if (out[t] == GS_L('?'))
			out[t] = GS_L('\0');
	}
	return &out[0];
}

bool EditorBase::OpenForm(
	const FILE_FORM_FILTER& filter,
	const char *directory,
	char *szoFilePathName,
	char *szoFileName)
{
	char fileInfo[___OUTPUT_LENGTH] = "\0";
	char fileName[___OUTPUT_LENGTH];

	std::string sInitDir = Platform::AddLastSlash(directory), out;

	OPENFILENAMEA openFileName;
	openFileName.lStructSize       = sizeof(openFileName);
	openFileName.hwndOwner         = NULL;
	openFileName.hInstance         = 0;
	openFileName.lpstrFilter       = AssembleWin32FilterString(filter, out);
	openFileName.lpstrCustomFilter = NULL;
	openFileName.nMaxCustFilter    = 0;
	openFileName.nFilterIndex      = 1;
	openFileName.lpstrFile         = fileInfo;
	openFileName.nMaxFile          = ___OUTPUT_LENGTH;
	openFileName.lpstrFileTitle    = fileName;
	openFileName.nMaxFileTitle     = ___OUTPUT_LENGTH;
	openFileName.lpstrInitialDir   = sInitDir.c_str();
	openFileName.lpstrTitle        = NULL;
	openFileName.Flags             = (OFN_FILEMUSTEXIST);
	openFileName.lpstrDefExt       = NULL;

	if (!GetOpenFileNameA(&openFileName))
	{
		return false;
	}

	_ETH_SAFE_strcpy(szoFileName, fileName);
	_ETH_SAFE_strcpy(szoFilePathName, fileInfo);
	return true;
}

bool EditorBase::SaveForm(
	const FILE_FORM_FILTER& filter,
	const char *szDir,
	char *szoFilePathName,
	char *szoFileName)
{
	char fileInfo[___OUTPUT_LENGTH] = "\0";
	char fileName[___OUTPUT_LENGTH];

	std::string sInitDir = Platform::AddLastSlash(szDir), out;

	OPENFILENAMEA saveFileName;
	saveFileName.lStructSize       = sizeof(saveFileName);
	saveFileName.hwndOwner         = NULL;
	saveFileName.hInstance         = 0;
	saveFileName.lpstrFilter       = AssembleWin32FilterString(filter, out);
	saveFileName.lpstrCustomFilter = NULL;
	saveFileName.nMaxCustFilter    = 0;
	saveFileName.nFilterIndex      = 1;
	saveFileName.lpstrFile         = fileInfo;
	saveFileName.nMaxFile          = ___OUTPUT_LENGTH;
	saveFileName.lpstrFileTitle    = fileName;
	saveFileName.nMaxFileTitle     = ___OUTPUT_LENGTH;
	saveFileName.lpstrInitialDir   = sInitDir.c_str();
	saveFileName.lpstrTitle        = NULL;
	saveFileName.Flags             = (OFN_OVERWRITEPROMPT);
	saveFileName.lpstrDefExt       = NULL;

	if (!GetSaveFileNameA(&saveFileName))
	{
		return false;
	}

	_ETH_SAFE_strcpy(szoFileName, fileName);
	_ETH_SAFE_strcpy(szoFilePathName, fileInfo);
	return true;
}
