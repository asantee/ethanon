#pragma once

#include <xstring>
#include <vector>

void PrintHelp();
void PrintWrongCommandlineFormatMessage();
void PrintWrongFileFormatMessage();
void PrintErrorWhileFilesFindingMessage();
void PrintErrorTooLargeFileMessage();
void PrintErrorFileFormatValidationFailedMessage(
	const std::wstring& filename, const std::wstring& desiredFormat, const std::vector<int>& errorPositions
);
