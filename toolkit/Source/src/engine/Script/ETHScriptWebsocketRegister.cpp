#include "../../addons/aswrappedcall.h"
#include "../Util/wsclient.hpp"
#undef GetObject
// std::shared_ptr<WebsocketClient> ws = std::make_shared<WebsocketClient>(ioc);

std::shared_ptr<WebsocketClient> WebsocketClientFactory()
{
	// WebsocketClient must be dynamically instantiated for Angelscript and
	// it must be a shared_ptr.
	// There are 2 referencing counting implementations, we use Angelscript,
	// so there is a delete on unref.
	std::shared_ptr<WebsocketClient> obj = std::shared_ptr<WebsocketClient>(new WebsocketClient());
	asITypeInfo* type = ETHScriptWrapper::m_pASEngine->GetTypeInfoByName("WebsocketClient");
	ETHScriptWrapper::m_pASEngine->NotifyGarbageCollectorOfNewObject(obj.get(), type);
	return obj;

	// this delete works, but there weakptr fail:
	//return new WebsocketClient();

	// weakptr works, but delete fails:
	//return std::make_shared<WebsocketClient>();
}

asDECLARE_FUNCTION_WRAPPER(__WebsocketClientFactory, WebsocketClientFactory);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__AddRef, WebsocketClient, AddRef, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Release, WebsocketClient, Release, (void), void);

// Garbage Collector behavior
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__SetGCFlag, WebsocketClient, SetGCFlag, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__GetGCFlag, WebsocketClient, GetGCFlag, (void), bool);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__GetRefCount, WebsocketClient, GetRefCount, (void), int);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__EnumReferences, WebsocketClient, EnumReferences, (int&), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__ReleaseAllReferences, WebsocketClient, ReleaseAllReferences, (int&), void);

// Do the same as above macros.
/*
static void WebsocketClient__GetRefCount(asIScriptGeneric* gen)
{
	CScriptDictionary* self = (CScriptDictionary*)gen->GetObject();
	*(int*)gen->GetAddressOfReturnLocation() = self->GetRefCount();
}

static void WebsocketClient__SetGCFlag(asIScriptGeneric* gen)
{
	CScriptDictionary* self = (CScriptDictionary*)gen->GetObject();
	self->SetGCFlag();
}

static void WebsocketClient__GetGCFlag(asIScriptGeneric* gen)
{
	CScriptDictionary* self = (CScriptDictionary*)gen->GetObject();
	*(bool*)gen->GetAddressOfReturnLocation() = self->GetGCFlag();
}

static void WebsocketClient__EnumReferences(asIScriptGeneric* gen)
{
	CScriptDictionary* self = (CScriptDictionary*)gen->GetObject();
	asIScriptEngine* engine = *(asIScriptEngine**)gen->GetAddressOfArg(0);
	self->EnumReferences(engine);
}

static void WebsocketClient__ReleaseAllReferences(asIScriptGeneric* gen)
{
	CScriptDictionary* self = (CScriptDictionary*)gen->GetObject();
	asIScriptEngine* engine = *(asIScriptEngine**)gen->GetAddressOfArg(0);
	self->ReleaseAllReferences(engine);
}
*/

// Main methods
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Update, WebsocketClient, Update, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Connect, WebsocketClient, Connect, (const std::string&, const std::string&), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Disconnect, WebsocketClient, Disconnect, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Send, WebsocketClient, Send, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Ping, WebsocketClient, Ping, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__GetLatency, WebsocketClient, GetLatency, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__GetUptime, WebsocketClient, GetUptime, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__IsConnected, WebsocketClient, IsConnected, (void), void);

asDECLARE_METHOD_WRAPPERPR(WebsocketClient__ClearBuffer, WebsocketClient, ClearBuffer, (void), void);

asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_bool, WebsocketClient, Pack, (bool), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_uint8, WebsocketClient, Pack, (uint8_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_uint16, WebsocketClient, Pack, (uint16_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_uint32, WebsocketClient, Pack, (uint32_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_uint64, WebsocketClient, Pack, (uint64_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_int8, WebsocketClient, Pack, (int8_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_int16, WebsocketClient, Pack, (int16_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_int32, WebsocketClient, Pack, (int32_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_int64, WebsocketClient, Pack, (int64_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_float, WebsocketClient, Pack, (float), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_double, WebsocketClient, Pack, (double), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_string, WebsocketClient, Pack, (const std::string&), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_dictionary, WebsocketClient, Pack, (CScriptDictionary*), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_any, WebsocketClient, Pack, (const CScriptAny&), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_array, WebsocketClient, Pack, (const CScriptArray&), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__PackArray, WebsocketClient, PackArray, (int32_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__PackMap, WebsocketClient, PackMap, (int32_t), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__PackNil, WebsocketClient, PackNil, (void), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_vector2, WebsocketClient, Pack, (const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__Pack_vector3, WebsocketClient, Pack, (const Vector3&), void);

// Callbacks
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__SetOnConnectCallback, WebsocketClient, SetOnConnectCallback, (asIScriptFunction* cb), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__SetOnDisconnectCallback, WebsocketClient, SetOnDisconnectCallback, (asIScriptFunction* cb), void);
asDECLARE_METHOD_WRAPPERPR(WebsocketClient__SetOnMessageCallback, WebsocketClient, SetOnMessageCallback, (asIScriptFunction* cb), void);


void RegisterWebSocketClient(asIScriptEngine* pASEngine)
{
	int r;

	// Class
	r = pASEngine->RegisterObjectType("WebsocketClient", 0, asOBJ_REF | asOBJ_GC); assert(r >= 0);

	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_FACTORY, "WebsocketClient@ f()", asFUNCTION(__WebsocketClientFactory), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_ADDREF, "void f()", asFUNCTION(WebsocketClient__AddRef), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_RELEASE, "void f()", asFUNCTION(WebsocketClient__Release), asCALL_GENERIC); assert(r >= 0);
	
	// Register the GC support behaviours
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_SETGCFLAG, "void f()", asFUNCTION(WebsocketClient__SetGCFlag), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_GETGCFLAG, "bool f()", asFUNCTION(WebsocketClient__GetGCFlag), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_GETREFCOUNT, "int f()", asFUNCTION(WebsocketClient__GetRefCount), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_ENUMREFS, "void f(int&in)", asFUNCTION(WebsocketClient__EnumReferences), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("WebsocketClient", asBEHAVE_RELEASEREFS, "void f(int&in)", asFUNCTION(WebsocketClient__ReleaseAllReferences), asCALL_GENERIC); assert(r >= 0);

	// Interfaces
	//r = pASEngine->RegisterInterfaceMethod("WebsocketClient", "void OnConnect()"); assert(r >= 0);

	// Methods
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Connect(const string &in, const string &in)", asFUNCTION(WebsocketClient__Connect), asCALL_GENERIC);	assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Disconnect()", asFUNCTION(WebsocketClient__Disconnect), asCALL_GENERIC);	assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Send()", asFUNCTION(WebsocketClient__Send), asCALL_GENERIC);	assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Update()", asFUNCTION(WebsocketClient__Update), asCALL_GENERIC);	assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Ping()", asFUNCTION(WebsocketClient__Ping), asCALL_GENERIC);	assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("WebsocketClient", "double GetLatency()", asFUNCTION(WebsocketClient__GetLatency), asCALL_GENERIC);	assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "double GetUptime()", asFUNCTION(WebsocketClient__GetUptime), asCALL_GENERIC);	assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "bool IsConnected()", asFUNCTION(WebsocketClient__IsConnected), asCALL_GENERIC);	assert(r >= 0);

	// MsgPack things
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(float)", asFUNCTION(WebsocketClient__Pack_float), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(double)", asFUNCTION(WebsocketClient__Pack_double), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(bool)", asFUNCTION(WebsocketClient__Pack_bool), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(uint8)", asFUNCTION(WebsocketClient__Pack_uint8), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(uint16)", asFUNCTION(WebsocketClient__Pack_uint16), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(uint32)", asFUNCTION(WebsocketClient__Pack_uint32), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(uint64)", asFUNCTION(WebsocketClient__Pack_uint64), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(int8)", asFUNCTION(WebsocketClient__Pack_int8), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(int16)", asFUNCTION(WebsocketClient__Pack_int16), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(int32)", asFUNCTION(WebsocketClient__Pack_int32), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(int64)", asFUNCTION(WebsocketClient__Pack_int64), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const string &in)", asFUNCTION(WebsocketClient__Pack_string), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const dictionary &in)", asFUNCTION(WebsocketClient__Pack_dictionary), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const any &in)", asFUNCTION(WebsocketClient__Pack_any), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const vector2 &in)", asFUNCTION(WebsocketClient__Pack_vector2), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const vector3 &in)", asFUNCTION(WebsocketClient__Pack_vector3), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void PackArray(uint32)", asFUNCTION(WebsocketClient__PackArray), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void PackMap(uint32)", asFUNCTION(WebsocketClient__PackMap), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void PackNil()", asFUNCTION(WebsocketClient__PackNil), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<int> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<int8> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<int16> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<int64> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<uint> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<uint8> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<uint16> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<uint64> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<float> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<double> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<vector2> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<vector3> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<string> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<dictionary> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void Pack(const array<any> &in)", asFUNCTION(WebsocketClient__Pack_array), asCALL_GENERIC); assert(r >= 0);

	//TODO: The above for Vector2, Vector3 and other Ethanon types

	// Callbacks
	r = pASEngine->RegisterFuncdef("void CALLBACK_void()"); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void SetOnConnect(CALLBACK_void @)", asFUNCTION(WebsocketClient__SetOnConnectCallback), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void SetOnDisconnect(CALLBACK_void @)", asFUNCTION(WebsocketClient__SetOnDisconnectCallback), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("WebsocketClient", "void SetOnMessage(CALLBACK_void @)", asFUNCTION(WebsocketClient__SetOnMessageCallback), asCALL_GENERIC); assert(r >= 0);
}