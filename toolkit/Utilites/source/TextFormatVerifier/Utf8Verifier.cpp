#include "stdafx.h"
#include "Utf8Verifier.h"

namespace utf8
{
	bool Verify(const unsigned char * fileByteContent, unsigned int fileByteSize, std::vector<int>& errorPositions)
	{
		errorPositions.clear();
		// check BOM
		if (fileByteSize >= 3 && fileByteContent[0] == 0xEF && fileByteContent[1] == 0xBB && fileByteContent[2] == 0xBF)
			return true;
		// // TODO: implement parsing of whole file
		return false;
	}
}