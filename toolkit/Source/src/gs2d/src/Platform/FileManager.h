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
public:
	virtual bool IsLoaded() const = 0;
	virtual bool GetFileBuffer(const gs2d::str_type::string& fileName, FileBuffer &out) = 0;
	virtual bool GetAnsiFileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out) = 0;
	virtual bool GetUTF8BOMFileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out) = 0;
	virtual bool GetUTF16FileString(const gs2d::str_type::string& fileName, gs2d::str_type::string &out) = 0;
	virtual bool FileExists(const gs2d::str_type::string& fileName) const = 0;
	virtual bool IsPacked() const = 0;

	virtual bool ConvertAnsiFileToUTF16LE(const gs2d::str_type::string& fileName);
};
typedef boost::shared_ptr<FileManager> FileManagerPtr;

}

#endif