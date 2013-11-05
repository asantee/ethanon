/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#include "ZipFileManager.h"
#include "Platform.h"

#ifdef ANDROID
	#include "android/AndroidLog.h"
#else
	#include <iostream>
#endif

#include <zip.h>

using namespace gs2d;

namespace Platform {

ZipFileManager::ZipFileManager(const str_type::char_t *filePath, const gs2d::str_type::char_t* password)
{
	m_archive = zip_open(filePath, 0, NULL);
	if (m_archive == NULL)
	{
		#ifdef ANDROID
			LOGE("Error loading APK");
		#else
			std::cerr << "Error loading APK" << std::endl;
		#endif
	}
	else
	{
		// now set the password
		// not supported on android
		#ifndef ANDROID
			if (password == 0)
				return;

			if (str_type::string(password) == GS_L(""))
				return;

			zip_set_default_password(m_archive, password);
		#endif
	}
}

ZipFileManager::~ZipFileManager()
{
	zip_close(m_archive);
}

bool ZipFileManager::IsLoaded() const
{
	return m_archive != NULL;
}

bool ZipFileManager::GetFileBuffer(const str_type::string &fileName, FileBuffer &out)
{
	if (!IsLoaded())
		return false;

	str_type::string fixedPath = fileName;
	FixSlashesForUnix(fixedPath);

	zip_file *file = zip_fopen(m_archive, fixedPath.c_str(), 0);

	if (file == NULL)
		return false;

	struct zip_stat stat;
	zip_stat(m_archive, fixedPath.c_str(), 0, &stat);

	out = FileBuffer(new _FileBuffer<unsigned char>(static_cast<unsigned long>(stat.size)));
	zip_fread(file, out->GetAddress(), stat.size);

	zip_fclose(file);
	return true;
}

bool ZipFileManager::FileExists(const gs2d::str_type::string& fileName) const
{
	if (!IsLoaded())
		return false;

	str_type::string fixedPath = fileName;
	FixSlashesForUnix(fixedPath);

	zip_file *file = zip_fopen(m_archive, fixedPath.c_str(), 0);

	if (file != NULL)
	{
		zip_fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}

zip *ZipFileManager::GetZip()
{
	return m_archive;
}

bool ZipFileManager::IsPacked() const
{
	return true;
}

} // namespace Platform
