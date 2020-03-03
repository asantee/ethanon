#include "ETHScriptObjRegister.h"
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

void RegisterMsgpackSimpleBuffer(asIScriptEngine* pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("SimpleBuffer", 0, asOBJ_REF); assert(r >= 0);

	r = pASEngine->RegisterObjectBehaviour("SimpleBuffer", asBEHAVE_FACTORY, "SimpleBuffer@ f()", asFUNCTION(SimpleBuffer::__Factory), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("SimpleBuffer", asBEHAVE_ADDREF, "SimpleBuffer@ f()", asMETHOD(SimpleBuffer,__AddRef), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("SimpleBuffer", asBEHAVE_RELEASE, "SimpleBuffer@ f()", asMETHOD(SimpleBuffer,__Release), asCALL_GENERIC); assert(r >= 0);

	/*r = pASEngine->RegisterObjectMethod("SimpleBuffer", "size_t size()", asMETHOD(SimpleBuffer, size), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("SimpleBuffer", "void clear()", asMETHOD(SimpleBuffer, clear), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("SimpleBuffer", "void data()", asMETHOD(SimpleBuffer, data), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("SimpleBuffer", "void release()", asMETHOD(SimpleBuffer, clear), asCALL_GENERIC); assert(r >= 0);

	void write(const char* buf, size_t len)
	size_t size() const
	char* data()
	char* release()
	void clear()
	*/
}

void RegisterMsgpack(asIScriptEngine* pASEngine)
{
	//int r;
	//r = pASEngine->register
	return;
}