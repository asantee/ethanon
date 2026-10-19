#include "FileManager.h"

#include <fstream>
#include <sstream>
#include <vector>

#include "../Unicode/utf8/utf8.h"

namespace Platform {

const unsigned short FileManager::UTF16LE_BOM = 0xFEFF;

bool FileManager::GetUTF8FileString(const FileBuffer& buffer, std::string &out)
{
	std::stringstream ss;
	unsigned char *adr = buffer->GetAddress();
	const std::size_t bufferSize = buffer->GetBufferSize();
	
	if (bufferSize == 0)
		return false;

	// check for UTF8 BOM
	std::size_t firstChar = 0;
	if (utf8::starts_with_bom(&adr[0], &adr[bufferSize]))
	{
		firstChar = 3;
	}

	utf8::replace_invalid(&adr[firstChar], &adr[bufferSize], back_inserter(out));
	return true;
}

bool FileManager::GetUTF16FileString(const FileBuffer& buffer, std::string &out)
{
	if (!HasUTF16LEBOM(buffer))
		return false;

	// Build each 16-bit unit from its two bytes (little-endian) instead of reading the bytes through a
	// pointer cast. Whole units only: with an odd byte count the decoder would never reach the end
	const std::size_t bomSizeInBytes = sizeof(UTF16LE_BOM);
	const unsigned char* bytes = &(buffer->GetAddress()[bomSizeInBytes]);
	const std::size_t unitCount = (buffer->GetBufferSize() - bomSizeInBytes) / 2;
	std::vector<utf8::uint16_t> units(unitCount);
	for (std::size_t t = 0; t < unitCount; t++)
	{
		units[t] = static_cast<utf8::uint16_t>(bytes[t * 2] | (bytes[t * 2 + 1] << 8));
	}

	const std::size_t originalSize = out.size();
	try
	{
		utf8::utf16to8(units.begin(), units.end(), back_inserter(out));
	}
	catch (const utf8::exception&)
	{
		// malformed UTF-16 (an unpaired surrogate): the file fails to load instead of ending the program
		out.resize(originalSize);
		return false;
	}
	return true;
}

bool FileManager::HasUTF16LEBOM(const FileBuffer& buffer)
{
	if (buffer->GetBufferSize() < sizeof(UTF16LE_BOM))
		return false;

	// compare the bytes (FF FE) instead of reading them through a pointer cast
	const unsigned char* bytes = buffer->GetAddress();
	return (bytes[0] == (UTF16LE_BOM & 0xFF) && bytes[1] == (UTF16LE_BOM >> 8));
}

bool FileManager::GetUTF8FileString(const std::string &fileName, std::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		return GetUTF8FileString(buffer, out);
	}
	else
	{
		return false;
	}
}

bool FileManager::GetUTF16FileString(const std::string &fileName, std::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		return GetUTF16FileString(buffer, out);
	}
	else
	{
		return false;
	}
}

bool FileManager::GetUTFFileString(const std::string &fileName, std::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
        if (HasUTF16LEBOM(buffer))
            return GetUTF16FileString(buffer, out);
        else
            return GetUTF8FileString(buffer, out);
	}
	else
	{
		return false;
	}
}

bool FileManager::GetAnsiFileString(const std::string &fileName, std::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		std::stringstream ss;
		unsigned char *adr = buffer->GetAddress();
		for (std::size_t t = 0; t < buffer->GetBufferSize(); t++)
		{
			const char c = static_cast<char>(adr[t]);
			if (c != 0x0D) // remove all carriage return
				ss << c;
		}
		out = ss.str();
		return true;
	}
	else
	{
		return false;
	}
}

} // namespace Platform
