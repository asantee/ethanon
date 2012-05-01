#include "stdafx.h"
#include "Messages.h"
#include <iostream>

void PrintHelp()
{
	// // TODO: implement
}

void PrintWrongCommandlineFormatMessage()
{
	// // TODO: implement
}

void PrintWrongFileFormatMessage()
{
	// // TODO: implement
	return;
}

void PrintErrorWhileFilesFindingMessage()
{
	// // TODO: implement
	return;
}

void PrintErrorTooLargeFileMessage()
{
	// // TODO: implement
	return;
}

void PrintErrorFileFormatValidationFailedMessage(
	const std::wstring& filename, const std::wstring& desiredFormat, const std::vector<int>& errorPositions
)
{
	std::wcout << L"\t" << filename.c_str() << L" - non-" << desiredFormat.c_str() << L" fromat" << std::endl;
	std::wcout << L"\t\terror positions: ";
	for (int i = 0; i < (int)errorPositions.size(); ++i)
	{
		std::wcout << errorPositions[i] << L", ";
	}	
	std::wcout << L"." << std::endl;
	return;
}
