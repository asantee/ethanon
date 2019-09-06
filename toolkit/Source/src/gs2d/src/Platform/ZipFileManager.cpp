#include "ZipFileManager.h"
#include "Platform.h"

#ifdef ANDROID
	#include "android/AndroidLog.h"
#else
	#include <iostream>
#endif

#include <zip.h>

namespace Platform {

ZipFileManager::ZipFileManager(const char* filePath, const char* password)
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

			if (std::string(password) == "")
				return;

			//zip_set_default_password(m_archive, password);
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

bool ZipFileManager::GetFileBuffer(const std::string &fileName, FileBuffer &out)
{
	if (!IsLoaded())
		return false;

	std::string fixedPath = fileName;
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

bool ZipFileManager::FileExists(const std::string& fileName) const
{
	if (!IsLoaded())
		return false;

	std::string fixedPath = fileName;
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
