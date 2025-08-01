#pragma once

/*
TODO: 
 - onWebsocketFail
 - Bonus: Separate msgpack from websocket.
*/

#include "../Script/ETHScriptWrapper.h"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/chrono.hpp>

#include <msgpack.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#include <angelscript.h>

#include "../addons/scriptarray.h"
#include "../addons/scriptdictionary.h"
#include "../addons/scriptany.h"

#include "moving_average.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class WebsocketClient : public std::enable_shared_from_this<WebsocketClient>
{
	int m_destructCount;
	int m_ref;
	bool m_gcFlag;
	bool m_closing;
	int m_reconnectCount;
	net::io_context m_ioc;
	tcp::resolver m_resolver;
	websocket::stream<beast::tcp_stream, true> m_ws;
	//net::ssl::context ctx(net::ssl::context::tlsv12);
	beast::flat_buffer m_inputBuffer;
	beast::flat_buffer m_outputBuffer;
	std::string m_host;
	std::string m_port;

	msgpack::sbuffer m_outputPd;
	//msgpack::sbuffer m_input_pd;
	msgpack::packer<msgpack::sbuffer> m_msgOut;
	
	asIScriptContext* m_asContext;

	asIScriptFunction* m_onConnectCallback;
	asITypeInfo* m_onConnectCallbackObjectType;
	void* m_onConnectCallbackObject;

	asIScriptFunction* m_onMessageCallback;
	asITypeInfo* m_onMessageCallbackObjectType;
	void* m_onMessageCallbackObject;

	asIScriptFunction* m_onDisconnectCallback;
	asITypeInfo* m_onDisconnectCallbackObjectType;
	void* m_onDisconnectCallbackObject;

	asIScriptFunction* m_onWebsocketFailCallback;
	asITypeInfo* m_onWebsocketFailCallbackObjectType;
	void* m_onWebsocketFailCallbackObject;

	MovingAverage<double> m_latency;
	bool m_waitingPong = false;
	boost::chrono::steady_clock::time_point m_pingTime;

	uint32_t m_keepaliveTimeout;
	boost::chrono::steady_clock::time_point m_connectedAt;

	void OnResolve(beast::error_code ec,tcp::resolver::results_type results);
	void OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
	void OnHandshake(beast::error_code ec);
	void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
	void OnRead(beast::error_code ec, std::size_t bytes_transferred);
	void OnClose(beast::error_code ec, const std::string& origin);

	// Cache extra asITypeId for types used in ehtanon engine
	int m_vector2TypeId;
	int m_vector3TypeId;
	int m_stringTypeId;
	int m_dictionaryTypeId;
	int m_anyTypeId;

	bool m_writingState = false;
	asITypeInfo* m_anyArrayTypeInfo;

	// Cache for all the array<T> specializations
	int m_arraytypeIds[15];

	// and a helper for testing if it is an array
	bool IsCScriptArray(const int typeId);

public:

	explicit WebsocketClient();
	~WebsocketClient();
	void Fail(beast::error_code ec, const char* what, const char *origin);
	void SetGCFlag();
	bool GetGCFlag();
	int  GetRefCount();
	void EnumReferences(asIScriptEngine* engine);
	void ReleaseAllReferences(asIScriptEngine* engine);

	void AddRef();
	void Release();
	void Update();
	void Connect(const std::string& host, const std::string& port);
	void Connect(char const* host, char const* port);
	void Connect();
	void Disconnect();
	void SendRaw(char* data, size_t size);
	void ClearBuffer();
	void Send();

	// MessagePack serializer stuff
	void Pack(bool value);
	void Pack(uint8_t value);
	void Pack(uint16_t value);
	void Pack(uint32_t value);
	void Pack(uint64_t value);
	void Pack(int8_t value);
	void Pack(int16_t value);
	void Pack(int32_t value);
	void Pack(int64_t value);
	void Pack(float value);
	void Pack(double value);
	void Pack(const std::string& value);
	void Pack(CScriptDictionary* dictionary);
	void Pack(const CScriptAny* any);
	void Pack(const CScriptArray& array);
	void Pack(const void* address, int type_id);
	void Pack(const gs2d::math::Vector2& vector);
	void Pack(const gs2d::math::Vector2i& vector);
	void Pack(const gs2d::math::Vector3& vector);
	void PackNil();

	bool ParseMsgPack(CScriptAny* any, const char* data, const size_t size, std::size_t& offset);

	// Create the array header on message pack, informing array size
	void PackArray(uint32_t length);
	
	// Create the map header on message pack, informing map size
	void PackMap(uint32_t length);

	void Ping();
	//	void OnMessage(void callback(std::string message));
	//	void OnClose(void callback(std::string message));
	double GetLatency();
	double GetUptime();
	bool IsConnected();

	void SetOnConnectCallback(asIScriptFunction* cb);
	void SetOnMessageCallback(asIScriptFunction* cb);

	void SetOnDisconnectCallback(asIScriptFunction* cb);
	void SetOnWebsocketFailCallback(asIScriptFunction* cb);
};
