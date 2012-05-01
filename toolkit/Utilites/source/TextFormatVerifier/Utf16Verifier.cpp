#include "stdafx.h"
#include "Utf16Verifier.h"

namespace utf16
{
	bool Verify(const unsigned char * fileByteContent, unsigned int fileByteSize, std::vector<int>& errorPositions)
	{
		errorPositions.clear();
		// check BOM
		if (fileByteSize >= 2 && fileByteContent[0] == 0xFF && fileByteContent[1] == 0xFE)
			return true;
		// // TODO: implement parsing of whole file
		return false;
	}
}