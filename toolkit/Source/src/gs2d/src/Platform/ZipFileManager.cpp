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

#include "ZipFileManager.h"

#ifdef ANDROID
	#include "android/AndroidLog.h"
#else
	#include <iostream>
#endif

#ifdef GS2D_STR_TYPE_WCHAR
	#define UNSAFE_UTF8_ALLOWED
	#include <unicode/utf8converter.h>
#endif

#include <zip.h>

using namespace gs2d;

namespace Platform {

static str_type::string FixSlashes(str_type::string path)
{
	const std::size_t size = path.size();
	for (std::size_t t = 0; t < size; t++)
	{
		if (path[t] == GS_L('\\'))
			path[t] = GS_L('/');
	}
	return path;
}

ZipFileManager::ZipFileManager(const str_type::char_t *filePath, const gs2d::str_type::char_t* password)
{
	#ifdef GS2D_STR_TYPE_WCHAR
		m_archive = zip_open(utf8::converter(filePath).c_str(), 0, NULL);
	#else
		m_archive = zip_open(filePath, 0, NULL);
	#endif
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

			#ifdef GS2D_STR_TYPE_WCHAR
				zip_set_default_password(m_archive, utf8::converter(password).c_str());
			#else
				zip_set_default_password(m_archive, password);
			#endif
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

	const str_type::string fixedPath = FixSlashes(fileName);

	#ifdef GS2D_STR_TYPE_WCHAR
		zip_file *file = zip_fopen(m_archive, utf8::converter(fixedPath).c_str(), 0);
	#else
		zip_file *file = zip_fopen(m_archive, fixedPath.c_str(), 0);
	#endif

	if (file == NULL)
		return false;

	struct zip_stat stat;
	#ifdef GS2D_STR_TYPE_WCHAR
		zip_stat(m_archive, utf8::converter(fixedPath).c_str(), 0, &stat);
	#else
		zip_stat(m_archive, fixedPath.c_str(), 0, &stat);
	#endif

	out = FileBuffer(new _FileBuffer<unsigned char>(static_cast<unsigned long>(stat.size)));
	zip_fread(file, out->GetAddress(), stat.size);

	zip_fclose(file);
	return true;
}

bool ZipFileManager::GetAnsiFileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		// TODO optimize it
		str_type::stringstream ss;
		const unsigned char *adr = buffer->GetAddress();
		for (unsigned long t = 0; t < buffer->GetBufferSize(); t++)
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

bool ZipFileManager::GetUTF8BOMFileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer buffer;
	if (GetFileBuffer(fileName, buffer))
	{
		// TODO optimize it
		str_type::stringstream ss;
		const unsigned char *adr = buffer->GetAddress();
		for (unsigned long t = 3; t < buffer->GetBufferSize(); t++)
		{
			ss << adr[t];
		}
		out = ss.str();
		return true;
	}
	else
	{
		return false;
	}
}

bool ZipFileManager::GetUTF16FileString(const str_type::string &fileName, str_type::string &out)
{
	FileBuffer utf16Buffer;
	if (GetFileBuffer(fileName, utf16Buffer))
	{
		// TODO/TO-DO: handle big-endian encoding too
		str_type::stringstream ss;
		const unsigned char *adr = utf16Buffer->GetAddress();
		for (unsigned long t = 2; t < utf16Buffer->GetBufferSize(); t += 2)
		{
			ss << *((str_type::char_t*)&adr[t]);
		}
		out = ss.str();
		return true;
	}
	else
	{
		return false;
	}
}

bool ZipFileManager::FileExists(const gs2d::str_type::string& fileName) const
{
	if (!IsLoaded())
		return false;

	#ifdef GS2D_STR_TYPE_WCHAR
		zip_file *file = zip_fopen(m_archive, utf8::converter(fileName).c_str(), 0);
	#else
		zip_file *file = zip_fopen(m_archive, fileName.c_str(), 0);
	#endif

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
