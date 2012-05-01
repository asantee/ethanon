#pragma once

#include <vector>

namespace ascii
{
	bool Verify(const unsigned char * fileByteContent, unsigned int fileByteSize, std::vector<int>& errorPositions);
}