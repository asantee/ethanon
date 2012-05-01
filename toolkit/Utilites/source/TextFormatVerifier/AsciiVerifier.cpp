#include "stdafx.h"
#include "AsciiVerifier.h"

namespace ascii
{
	bool Verify(const unsigned char * fileByteContent, unsigned int fileByteSize, std::vector<int>& errorPositions)
	{
		errorPositions.clear();
		for (unsigned int i = 0; i < fileByteSize; ++i)
		{
			if ( fileByteContent[i] > 0x7F )
			{
				errorPositions.push_back(i);
			}
		}
		return errorPositions.size() == 0;
	}
}