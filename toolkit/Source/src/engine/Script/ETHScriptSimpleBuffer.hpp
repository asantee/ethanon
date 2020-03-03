#pragma once

#include <msgpack.hpp>

class SimpleBuffer : msgpack::sbuffer
{
	int m_ref;
public:
	SimpleBuffer(size_t initsz = MSGPACK_SBUFFER_INIT_SIZE) : m_ref(1), sbuffer(initsz)
	{}

	static SimpleBuffer* __Factory(size_t initsz = MSGPACK_SBUFFER_INIT_SIZE)
	{
		return new SimpleBuffer(initsz);
	}

	void __AddRef()
	{
		m_ref++;
	}

	void __Release()
	{
		if (--m_ref == 0)
			delete this;
	}
};