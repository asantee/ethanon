#pragma once

#include <vector>

namespace utf16
{
	bool Verify(const unsigned char * fileByteContent, unsigned int fileByteSize, std::vector<int>& errorPositions);
}