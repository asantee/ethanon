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

#ifndef ETH_BINARY_STREAM_H_
#define ETH_BINARY_STREAM_H_

#include <Platform/FileManager.h>
#include "../angelscript/include/angelscript.h"
#include <string>
#include <iostream>
#include <fstream>
#include <Types.h>

using namespace gs2d;

class ETHBinaryStream : public asIBinaryStream
{
public:
	ETHBinaryStream(const Platform::FileManagerPtr& fileManager);

	virtual void Read(void *ptr, asUINT size);
	virtual void Write(const void *ptr, asUINT size);

	str_type::string GetFileName() const;
	bool OpenW(const str_type::string& fileName);
	bool CloseW();
	bool OpenR(const str_type::string& fileName);
	bool CloseR();

private:
	void SetFileName(const str_type::string& fileName);

	FILE* m_out;
	str_type::string m_fileName;
	unsigned int m_inCarret;
	Platform::FileBuffer m_buffer;
	Platform::FileManagerPtr m_fileManager;
};

#endif
