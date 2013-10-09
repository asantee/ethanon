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

#include "ETHBinaryStream.h"
#include <assert.h>

ETHBinaryStream::ETHBinaryStream(const Platform::FileManagerPtr& fileManager) :
	m_fileManager(fileManager),
	m_out(0),
	m_inCarret(0)
{
}

void ETHBinaryStream::Read(void *ptr, asUINT size)
{
	assert(m_buffer->GetAddress());
	if (m_inCarret + size > static_cast<unsigned int>(m_buffer->GetBufferSize()))
	{
		size = m_buffer->GetBufferSize() - m_inCarret;
	}
	memcpy(ptr, &(m_buffer->GetAddress()[m_inCarret]), size);
	m_inCarret += size;
}

void ETHBinaryStream::Write(const void *ptr, asUINT size)
{
	assert(m_out != 0);
	if (m_out)
	{
		fwrite(ptr, 1, size, m_out);
	}
}

str_type::string ETHBinaryStream::GetFileName() const
{
	return m_fileName;
}

void ETHBinaryStream::SetFileName(const str_type::string& fileName)
{
	m_fileName = fileName;
}

bool ETHBinaryStream::OpenW(const str_type::string& fileName)
{
	CloseW();
	SetFileName(fileName);
	#ifdef WIN32
		errno_t error = fopen_s(&m_out, GetFileName().c_str(), GS_L("wb"));
	#else
		int error = 0; m_out = fopen(GetFileName().c_str(), "wb");
	#endif
	if (!error && m_out)
	{
		return true;
	}
	else
	{
		m_out = 0;
		return false;
	}
}

bool ETHBinaryStream::CloseW()
{
	if (m_out)
	{
		fclose(m_out);
		m_out = 0;
	}
	return true;
}

bool ETHBinaryStream::OpenR(const str_type::string& fileName)
{
	SetFileName(fileName);
	return m_fileManager->GetFileBuffer(fileName, m_buffer);
}

bool ETHBinaryStream::CloseR()
{
	m_buffer.reset();
	return true;
}
