#pragma once

#include <xstring>

enum FileFormat
{
  unknown,
  ASCII,
  UTF8,
  UTF16
};

void VerifyFilesFormatByWildcard(FileFormat formatType, const std::wstring& wildcard, const std::wstring& excludeDirectories);
