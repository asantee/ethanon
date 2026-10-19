#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include <cassert>
#include <cstddef>
#include <new>
#include <string>

#include <boost/shared_ptr.hpp>

namespace Platform {

// A file's bytes. When the memory cannot be allocated the buffer stays empty (null address, size 0)
// instead of throwing, so a file too large for the available memory fails to load
template <class T> class FileBufferData
{
public:
	FileBufferData(const std::size_t size);
	~FileBufferData();
	std::size_t GetBufferSize();
	T *GetAddress();
private:
	T *m_buffer;
	std::size_t m_bufferSize;
};
typedef boost::shared_ptr<FileBufferData<unsigned char> > FileBuffer;

template <class T>
FileBufferData<T>::FileBufferData(const std::size_t size)
{
	assert(sizeof(T) == 1);
	m_buffer = NULL;
	m_bufferSize = 0;
	if (size > 0)
	{
		m_buffer = new (std::nothrow) T [size];
		if (m_buffer)
			m_bufferSize = size;
	}
}

template <class T>
FileBufferData<T>::~FileBufferData()
{
	if (m_buffer)
	{
		delete [] m_buffer;
	}
}

template <class T>
std::size_t FileBufferData<T>::GetBufferSize()
{
	return m_bufferSize;
}

template <class T>
T *FileBufferData<T>::GetAddress()
{
	return m_buffer;
}

class FileManager
{
	virtual bool GetUTF8FileString(const FileBuffer& buffer, std::string &out);
	virtual bool GetUTF16FileString(const FileBuffer& buffer, std::string &out);
	virtual bool HasUTF16LEBOM(const FileBuffer& buffer);

public:
	virtual bool IsLoaded() const = 0;
	virtual bool GetFileBuffer(const std::string& fileName, FileBuffer &out) = 0;

	virtual bool GetAnsiFileString(const std::string& fileName, std::string &out);
	virtual bool GetUTF8FileString(const std::string& fileName, std::string &out);
	virtual bool GetUTF16FileString(const std::string& fileName, std::string &out);
	virtual bool GetUTFFileString(const std::string& fileName, std::string &out);

	virtual bool FileExists(const std::string& fileName) const = 0;
	virtual bool IsPacked() const = 0;
    
	static const unsigned short UTF16LE_BOM;
};

typedef boost::shared_ptr<FileManager> FileManagerPtr;

}

#endif
