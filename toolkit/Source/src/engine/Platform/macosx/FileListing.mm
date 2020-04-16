#include "../FileListing.h"

namespace Platform {

bool FileListing::ListDirectoryFiles(const char* directory, const char* extension)
{
	m_fileName.clear();

	NSFileManager* fileManager;
	fileManager = [NSFileManager defaultManager];

	NSArray* files;
	files = [fileManager contentsOfDirectoryAtPath:[NSString stringWithUTF8String:directory] error:nil];

	for (std::size_t i = 0; i < [files count]; i++)
	{
		const std::string fileName = [[files objectAtIndex:i] cStringUsingEncoding:1];
		FILE_NAME file;
		file.dir = directory;
		file.file = Platform::GetFileName(fileName.c_str());
		
		if (Platform::IsExtensionRight(file.file, extension))
			m_fileName.push_back(file);
	}

	return true;
}

} // namespace Platform
