#ifndef ETH_BINARY_STREAM_H_
#define ETH_BINARY_STREAM_H_

#include <Platform/FileManager.h>

#include "../../angelscript/include/angelscript.h"

#include <string>
#include <iostream>
#include <fstream>

class ETHBinaryStream : public asIBinaryStream
{
public:
	ETHBinaryStream(const Platform::FileManagerPtr& fileManager);

	virtual int Read(void *ptr, asUINT size);
	virtual int Write(const void *ptr, asUINT size);

	std::string GetFileName() const;
	bool OpenW(const std::string& fileName);
	bool CloseW();
	bool OpenR(const std::string& fileName);
	bool CloseR();

private:
	void SetFileName(const std::string& fileName);

	FILE* m_out;
	std::string m_fileName;
	unsigned long m_inCarret;
	Platform::FileBuffer m_buffer;
	Platform::FileManagerPtr m_fileManager;
};

#endif
