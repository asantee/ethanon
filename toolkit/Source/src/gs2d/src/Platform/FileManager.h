#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

#include "../Types.h"

namespace Platform {

template <class T> class _FileBuffer
{
public:
	_FileBuffer(const unsigned long size);
	~_FileBuffer();
	unsigned long GetBufferSize();
	T *GetAddress();
private:
	T *m_buffer;
	unsigned long m_bufferSize;
};
typedef boost::shared_ptr<_FileBuffer<unsigned char> > FileBuffer;

template <class T>
_FileBuffer<T>::_FileBuffer(const unsigned long size)
{
	assert(sizeof(T) == 1);
	m_buffer = NULL;
	m_bufferSize = size;
	if (size > 0)
		m_buffer = new T [size];
}

template <class T>
_FileBuffer<T>::~_FileBuffer()
{
	if (m_buffer)
	{
		delete [] m_buffer;
	}
}

template <class T>
unsigned long _FileBuffer<T>::GetBufferSize()
{
	return m_bufferSize;
}

template <class T>
T *_FileBuffer<T>::GetAddress()
{
	return m_buffer;
}

class FileManager
{
	virtual bool GetUTF8FileString(const FileBuffer& buffer, gs2d::str_type::string &out);
	virtual bool GetUTF16FileString(const FileBuffer& buffer, gs2d::str_type::string &out);
	virtual bool HasUTF16LEBOM(const FileBuffer& buffer);

public:
	virtual bool IsLoaded() const = 0;
	virtual bool GetFileBuffer(const gs2d::str_type::string& fileName, FileBuffer &out) = 0;

	virtual bool GetAnsiFileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out);
	virtual bool GetUTF8FileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out);
	virtual bool GetUTF16FileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out);
	virtual bool GetUTFFileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out);

	virtual bool FileExists(const gs2d::str_type::string& fileName) const = 0;
	virtual bool IsPacked() const = 0;
    
	static const unsigned short UTF16LE_BOM;
};

typedef boost::shared_ptr<FileManager> FileManagerPtr;

}

#endif
