#ifndef STD_FILE_MANAGER_H_
#define STD_FILE_MANAGER_H_

#include "FileManager.h"

namespace Platform {

class StdFileManager : public FileManager
{
public:
	bool IsLoaded() const;
	bool GetFileBuffer(const gs2d::str_type::string &fileName, FileBuffer &out);
	bool FileExists(const gs2d::str_type::string& fileName) const;
	bool IsPacked() const;
};

typedef boost::shared_ptr<StdFileManager> StdFileManagerPtr;

}

#endif
