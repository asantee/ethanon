/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

    Permission is hereby granted, free of charge, to any person obtaining a copy of this
    software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#ifndef STD_ANSI_FILE_MANAGER_H_
#define STD_ANSI_FILE_MANAGER_H_

#include "FileManager.h"

namespace Platform {

class StdAnsiFileManager : public FileManager
{
    gs2d::str_type::string m_resourcePath;
public:
    StdAnsiFileManager(const gs2d::str_type::string& resourcePath);
	bool IsLoaded() const;
	bool GetFileBuffer(const gs2d::str_type::string &fileName, FileBuffer &out);
	bool GetAnsiFileString(const gs2d::str_type::string &fileName, gs2d::str_type::string &out);
	bool GetUTF8BOMFileString(const gs2d::str_type::string &fileName, gs2d::str_type::string &out);
	bool GetUTF16FileString(const gs2d::str_type::string &fileName, gs2d::str_type::string &out);
	bool FileExists(const gs2d::str_type::string& fileName) const;
    bool IsPacked() const;
};

typedef boost::shared_ptr<StdAnsiFileManager> StdAnsiFileManagerPtr;

}

#endif
