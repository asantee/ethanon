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
		BuildEntryIndex();

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

void ZipFileManager::BuildEntryIndex()
{
#if defined(LIBZIP_VERSION_MAJOR)
	const zip_int64_t numEntries = zip_get_num_entries(m_archive, 0);
#else
	const int numEntries = zip_get_num_files(m_archive);
#endif
	if (numEntries <= 0)
		return;

	m_entryIndices.reserve(static_cast<std::size_t>(numEntries));
	for (int t = 0; t < static_cast<int>(numEntries); t++)
	{
		const char* name = zip_get_name(m_archive, t, 0);
		if (name != NULL)
		{
			// insert keeps the first entry of a duplicated name, the one zip_name_locate would return
			m_entryIndices.insert(std::make_pair(std::string(name), t));
		}
	}
}

int ZipFileManager::FindEntry(const std::string& fixedPath) const
{
	const std::unordered_map<std::string, int>::const_iterator iter = m_entryIndices.find(fixedPath);
	return (iter != m_entryIndices.end()) ? iter->second : -1;
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

	const int index = FindEntry(fixedPath);
	if (index < 0)
		return false;

	zip_file *file = zip_fopen_index(m_archive, index, 0);

	if (file == NULL)
		return false;

	struct zip_stat stat;
	zip_stat_index(m_archive, index, 0, &stat);

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

	return FindEntry(fixedPath) >= 0;
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
