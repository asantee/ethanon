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
#include "AndroidLog.h"

using namespace gs2d;

namespace Platform {

ZipFileManager::ZipFileManager(const str_type::char_t *filePath)
{
	m_archive = zip_open(filePath, 0, NULL);
	if (m_archive == NULL)
	{
		LOGE("Error loading APK");
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

	zip_file *file = zip_fopen(m_archive, fileName.c_str(), 0);

	if (file == NULL)
		return false;

	struct zip_stat stat;
	zip_stat(m_archive, fileName.c_str(), 0, &stat);

	out = FileBuffer(new _FileBuffer<unsigned char>(stat.size));
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
		for (int t=0; t<buffer->GetBufferSize(); t++)
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
		for (int t=3; t<buffer->GetBufferSize(); t++)
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
		for (int t = 2; t < utf16Buffer->GetBufferSize(); t += 2)
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
	return true;
}

bool ZipFileManager::FileExists(const gs2d::str_type::string& fileName) const
{
	if (!IsLoaded())
		return false;

	zip_file *file = zip_fopen(m_archive, fileName.c_str(), 0);

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

} // namespace Platform