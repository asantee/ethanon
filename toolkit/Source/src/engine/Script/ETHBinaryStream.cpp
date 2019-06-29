#include "ETHBinaryStream.h"
#include <assert.h>

ETHBinaryStream::ETHBinaryStream(const Platform::FileManagerPtr& fileManager) :
	m_fileManager(fileManager),
	m_out(0),
	m_inCarret(0)
{
}

int ETHBinaryStream::Read(void *ptr, asUINT size)
{
	assert(m_buffer->GetAddress());
	if (m_inCarret + size > static_cast<unsigned int>(m_buffer->GetBufferSize()))
	{
		size = static_cast<asUINT>(m_buffer->GetBufferSize() - m_inCarret);
	}
	memcpy(ptr, &(m_buffer->GetAddress()[m_inCarret]), size);
	m_inCarret += size;
	return 1;
}

int ETHBinaryStream::Write(const void *ptr, asUINT size)
{
	assert(m_out != 0);
	if (m_out)
	{
		fwrite(ptr, 1, size, m_out);
	}
	return 1;
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
