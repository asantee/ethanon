// TextFormatVerifier.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "WildcardSearcher.h"
#include "Messages.h"

bool VerifyArguments(const std::vector<std::wstring>& arguments);
FileFormat ExtractFormatType(const std::vector<std::wstring>& arguments);
std::wstring ExtractWildcard(const std::vector<std::wstring>& arguments);
std::wstring ExtractExcludes(const std::vector<std::wstring>& arguments);

int _tmain(int argc, _TCHAR* argv[])
{
	FileFormat fromatType = unknown;
	std::wstring verifyWildcard;
	std::wstring excludeWildcard;

	bool argumentsOK = false;
	if (argc == 3 || argc == 4 )
	{
		std::vector<std::wstring> arguments(argc-1);
		for (int i = 1; i < argc; ++i)
		{
			arguments[i-1] = argv[i];
		}
		if (VerifyArguments(arguments))
		{
			argumentsOK = true;
			fromatType = ExtractFormatType(arguments);
			verifyWildcard = ExtractWildcard(arguments);
			excludeWildcard = ExtractExcludes(arguments);
		}
	}

	if (!argumentsOK)
	{
		PrintWrongCommandlineFormatMessage();
		PrintHelp();
		return 1;
	}

	VerifyFilesFormatByWildcard( fromatType, verifyWildcard, excludeWildcard );

	return 0;
}

bool IsCorrectWildcard(const std::wstring& wildcard);
bool IsCorrectExcludeWildcard(const std::wstring& wildcard);

bool VerifyArguments(const std::vector<std::wstring>& arguments)
{
	if (arguments.size() < 2 || arguments.size() > 3)
		return false;
	const std::wstring& fileFormat = arguments[0];
	if ( !(fileFormat == L"ASCII" || fileFormat == L"UTF-8" || fileFormat == L"UTF-16") )
		return false;
	if (!IsCorrectWildcard(arguments[1]))
		return false;
	if (arguments.size() > 2 && !IsCorrectExcludeWildcard(arguments[2]))
		return false;
	return true;
}

bool IsCorrectWildcard(const std::wstring& wildcard)
{
	// // TODO: implement correctly
	// should be like this: "C:\MyProject\Game\Resources\*.as;*.cg" or ".\data\*.xml;*.json;*.enml"
	return !wildcard.empty();
}

bool IsCorrectExcludeWildcard(const std::wstring& wildcard)
{
	// // TODO: implement correctly
	// should be like this: "DX_SDK;BOOST_SDK;.svn;.git"
	return !wildcard.empty();
}

FileFormat ExtractFormatType(const std::vector<std::wstring>& arguments)
{
	const std::wstring& fileFormat = arguments[0];
	if ( fileFormat == L"ASCII" )
		return ASCII;
	if ( fileFormat == L"UTF-8" )
		return UTF8;
	if ( fileFormat == L"UTF-16" )
		return UTF16;
	return unknown;
}

std::wstring ExtractWildcard(const std::vector<std::wstring>& arguments)
{
	return arguments[1];
}

std::wstring ExtractExcludes(const std::vector<std::wstring>& arguments)
{
	if (arguments.size() > 2)
		return arguments[2];
	else
		return L"";
}

