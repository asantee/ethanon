#include "WebsocketClient.h"

#include <boost/exception/diagnostic_information.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

WebsocketClient::WebsocketClient() :
	m_resolver(net::make_strand(m_ioc)),
	m_ws(net::make_strand(m_ioc)),
	m_closing(false),
	m_keepaliveTimeout(30),
	m_ref(1),
	m_destructCount(0),
	m_reconnectCount(0),
	m_gcFlag(false),
	m_msgOut(&m_outputPd),
	m_onConnectCallback(0),
	m_onConnectCallbackObject(0),
	m_onConnectCallbackObjectType(0),
	m_onMessageCallback(0),
	m_onMessageCallbackObject(0),
	m_onMessageCallbackObjectType(0),
	m_onDisconnectCallback(0),
	m_onDisconnectCallbackObject(0),
	m_onDisconnectCallbackObjectType(0),
	m_onWebsocketFailCallback(0),
	m_onWebsocketFailCallbackObject(0),
	m_onWebsocketFailCallbackObjectType(0)
{
	// Create a new context for execution
	m_asContext = ETHScriptWrapper::m_pASEngine->CreateContext();

	// Cache extra asITypeInfo for types used in ehtanon engine
	m_vector2TypeId = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("vector2");
	m_vector3TypeId = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("vector3");
	m_stringTypeId = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("string");
	m_dictionaryTypeId = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("dictionary");
	m_anyTypeId = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("any");

	// get all the array type ids.
	m_arraytypeIds[0] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int>");
	m_arraytypeIds[1] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int8>");
	m_arraytypeIds[2] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int16>");
	m_arraytypeIds[3] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int64>");
	m_arraytypeIds[4] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint>");
	m_arraytypeIds[5] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint8>");
	m_arraytypeIds[6] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint16>");
	m_arraytypeIds[7] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint64>");
	m_arraytypeIds[8] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<float>");
	m_arraytypeIds[9] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<double>");
	m_arraytypeIds[10] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<string>");
	m_arraytypeIds[11] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<dictionary>");
	m_arraytypeIds[12] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<vector2>");
	m_arraytypeIds[13] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<vector3>");
	m_arraytypeIds[14] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<any>");

	m_anyArrayTypeInfo = ETHScriptWrapper::m_pASEngine->GetTypeInfoByDecl("array<any>");
}

WebsocketClient::~WebsocketClient()
{
	Disconnect();
}

// Report a failure
void WebsocketClient::Fail(beast::error_code ec, const char* what, const char *origin)
{
	std::string reason = ec.message();
	const std::string category = ec.category().name();

	std::stringstream logStream;
	logStream << "Failure in '" << what
			   << "': Category: " << category
			   << ", Value: " << ec.value()
			   << ", Reason: " << reason
			   << " — [" << origin << "]";

	ETHResourceProvider::Log(logStream.str(), Platform::Logger::LT_WARNING);

	if (m_onWebsocketFailCallback)
	{
		m_asContext->Prepare(m_onWebsocketFailCallback);
		m_asContext->SetObject(m_onWebsocketFailCallbackObject);

		m_asContext->SetArgObject(0, &what);
		m_asContext->SetArgObject(1, &reason);
		m_asContext->Execute();
	}
}

//  Garbage Collect extra support behaviors
void WebsocketClient::SetGCFlag()
{
	// Set the gc flag as the high bit in the reference counter
	//m_ref |= 0x80000000;
	m_gcFlag = true;
}

bool WebsocketClient::GetGCFlag()
{
	// Return the gc flag
	//return (m_ref & 0x80000000) ? true : false;
	return m_gcFlag;
}

int WebsocketClient::GetRefCount()
{
	// Return the reference count, without the gc flag
	//return (m_ref & 0x7FFFFFFF);
	return m_ref;
}

void WebsocketClient::EnumReferences(asIScriptEngine* engine)
{
	// Call the engine::GCEnumCallback for all references to other objects held
	engine->GCEnumCallback(m_onConnectCallback);
	engine->GCEnumCallback(m_onConnectCallbackObject);
}

void WebsocketClient::ReleaseAllReferences(asIScriptEngine* engine)
{
	// When we receive this call, we are as good as dead, but
	// the garbage collector will still hold a references to us, so we
	// cannot just delete ourself yet. Just free all references to other
	// objects that we hold

	if (m_onConnectCallback)
		m_onConnectCallback->Release();
	if (m_onConnectCallbackObject)
		engine->ReleaseScriptObject(m_onConnectCallbackObject, m_onConnectCallbackObjectType);
	m_onConnectCallback = 0;
	m_onConnectCallbackObject = 0;
	m_onConnectCallbackObjectType = 0;
}

void WebsocketClient::AddRef()
{
	m_ref++;
}

void WebsocketClient::Release()
{
	if (--m_ref == 0)
		delete this;
}

void WebsocketClient::Update()
{
	try {
		m_ioc.poll();
	} catch (boost::exception const& ex) {
		ETHResourceProvider::Log(boost::diagnostic_information(ex), Platform::Logger::LT_WARNING);
	} catch (std::exception const&  ex) {
		ETHResourceProvider::Log(ex.what(), Platform::Logger::LT_WARNING);
	}
};

void WebsocketClient::Connect(const std::string& host, const std::string& port)
{
	Connect(host.c_str(), port.c_str());
}

void WebsocketClient::SetOnConnectCallback(asIScriptFunction* cb)
{
	// Release the previous callback, if any
	if (m_onConnectCallback)
		m_onConnectCallback->Release();
	if (m_onConnectCallbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_onConnectCallbackObject, m_onConnectCallbackObjectType);
	m_onConnectCallback = 0;
	m_onConnectCallbackObject = 0;
	m_onConnectCallbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_onConnectCallbackObject = cb->GetDelegateObject();
		m_onConnectCallbackObjectType = cb->GetDelegateObjectType();
		m_onConnectCallback = cb->GetDelegateFunction();

		// Hold on to the object and method

		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_onConnectCallbackObject, m_onConnectCallbackObjectType);
		m_onConnectCallback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();

	}
	else
	{
		// Store the received handle for later use
		m_onConnectCallback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

void WebsocketClient::SetOnDisconnectCallback(asIScriptFunction* cb)
{
	// Release the previous callback, if any
	if (m_onDisconnectCallback)
		m_onDisconnectCallback->Release();
	if (m_onDisconnectCallbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_onDisconnectCallbackObject, m_onDisconnectCallbackObjectType);
	m_onDisconnectCallback = 0;
	m_onDisconnectCallbackObject = 0;
	m_onDisconnectCallbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_onDisconnectCallbackObject = cb->GetDelegateObject();
		m_onDisconnectCallbackObjectType = cb->GetDelegateObjectType();
		m_onDisconnectCallback = cb->GetDelegateFunction();

		// Hold on to the object and method

		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_onDisconnectCallbackObject, m_onDisconnectCallbackObjectType);
		m_onDisconnectCallback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();
	}
	else
	{
		// Store the received handle for later use
		m_onDisconnectCallback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

void WebsocketClient::SetOnWebsocketFailCallback(asIScriptFunction* cb)
{
	// Release the previous callback, if any
	if (m_onWebsocketFailCallback)
		m_onWebsocketFailCallback->Release();
	if (m_onWebsocketFailCallbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_onWebsocketFailCallbackObject, m_onWebsocketFailCallbackObjectType);
	m_onWebsocketFailCallback = 0;
	m_onWebsocketFailCallbackObject = 0;
	m_onWebsocketFailCallbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_onWebsocketFailCallbackObject = cb->GetDelegateObject();
		m_onWebsocketFailCallbackObjectType = cb->GetDelegateObjectType();
		m_onWebsocketFailCallback = cb->GetDelegateFunction();

		// Hold on to the object and method

		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_onWebsocketFailCallbackObject, m_onWebsocketFailCallbackObjectType);
		m_onWebsocketFailCallback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();
	}
	else
	{
		// Store the received handle for later use
		m_onWebsocketFailCallback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

void WebsocketClient::SetOnMessageCallback(asIScriptFunction* cb)
{
	// Release the previous callback, if any
	if (m_onMessageCallback)
		m_onMessageCallback->Release();
	if (m_onMessageCallbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_onMessageCallbackObject, m_onMessageCallbackObjectType);
	m_onMessageCallback = 0;
	m_onMessageCallbackObject = 0;
	m_onMessageCallbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_onMessageCallbackObject = cb->GetDelegateObject();
		m_onMessageCallbackObjectType = cb->GetDelegateObjectType();
		m_onMessageCallback = cb->GetDelegateFunction();

		// Hold on to the object and method
		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_onMessageCallbackObject, m_onMessageCallbackObjectType);
		m_onMessageCallback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();
	}
	else
	{
		// Store the received handle for later use
		m_onMessageCallback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

// Start the asynchronous operation
void WebsocketClient::Connect(char const* host, char const* port)
{
	if (m_ws.is_open())
		return;
	// Save these for later
	m_host = host;
	m_port = port;
	Connect();
}

void WebsocketClient::Connect()
{
	if (m_ws.is_open())
		return;

	m_closing = false;
	
	// Look up the domain name
	m_resolver.async_resolve(m_host, m_port, beast::bind_front_handler(&WebsocketClient::OnResolve, shared_from_this()));
}

void WebsocketClient::OnResolve(beast::error_code ec, tcp::resolver::results_type results)
{
	if (ec)
		return Fail(ec, "resolve", "OnResolve callback");

	// Set the timeout for the operation
	beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

	// Make the connection on the IP address we get from a lookup
	beast::get_lowest_layer(m_ws).async_connect(results, beast::bind_front_handler(&WebsocketClient::OnConnect,shared_from_this()));
}

void WebsocketClient::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
	if (ec)
		return Fail(ec, "connect", "OnConnect callback");

	m_connectedAt = boost::chrono::steady_clock::now();

	// Turn off the timeout on the tcp_stream, because
	// the websocket stream has its own timeout system.
	beast::get_lowest_layer(m_ws).expires_never();

	// Set suggested timeout settings for the websocket
	m_ws.set_option(
		websocket::stream_base::timeout::suggested(
			beast::role_type::client));

	// Set a decorator to change the User-Agent of the handshake
	m_ws.set_option(websocket::stream_base::decorator(
		[](websocket::request_type& req)
		{
			req.set(http::field::user_agent, "Magic Rampage WS Client");
		}));

	// Set up what-to-do's on control packets
	m_ws.control_callback([this](websocket::frame_type kind, beast::string_view data)
	{
		if (kind == websocket::frame_type::ping)
		{
			std::cout << "<<< received a ping: "<< data << std::endl;
		}
		else if (kind == websocket::frame_type::pong)
		{
			if (m_waitingPong)
			{
				boost::chrono::duration<double> elapsed;
				elapsed = boost::chrono::steady_clock::now() - m_pingTime;
				m_latency.update((double)elapsed.count());
				m_waitingPong = false;
			}
		}
		else if (kind == websocket::frame_type::close)
		{
			std::stringstream ss; ss << "<<< received a close request: " << data;
			ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_WARNING);
		}
	});

	// Perform the websocket handshake
	m_ws.async_handshake(m_host, "/", beast::bind_front_handler(&WebsocketClient::OnHandshake, shared_from_this()));
}

void WebsocketClient::OnHandshake(beast::error_code ec)
{
	if (ec)
		return Fail(ec, "handshake", "OnHandshake callback");
	
	ETHResourceProvider::Log("Websocket handshake completed!\n", Platform::Logger::LT_INFO);

	// call AS OnConnect callback
	if (m_onConnectCallback)
	{
		m_asContext->Prepare(m_onConnectCallback);
		m_asContext->SetObject(m_onConnectCallbackObject);

		// Set the function arguments
		//m_pScriptContext->SetArgDWord(...);
		/*int r = */m_asContext->Execute();
		/*if (r == asEXECUTION_FINISHED)
		{
			// The return value is only valid if the execution finished successfully
			//asDWORD ret = m_as_ctx->GetReturnDWord();
		}*/
	}

	// Start reading incomming messages
	m_ws.async_read(m_inputBuffer, beast::bind_front_handler(&WebsocketClient::OnRead, shared_from_this()));
}

void WebsocketClient::OnWrite(beast::error_code ec, std::size_t bytes_transferred)
{
	if (ec)
		return Fail(ec, "write", "OnWrite callback");

	// msg sent, consume output buffer.
	m_outputBuffer.consume(bytes_transferred);
	if (m_outputBuffer.size() <= 0)
	{
		// reset writing state when transfer finishes
		m_writingState = false;
		return;
	}

	// send message while buffer not empty
	if (m_ws.is_open())
	{
		m_ws.binary(true);
		m_writingState = true;
		m_ws.async_write(m_outputBuffer.cdata(), [sp = shared_from_this()](beast::error_code ec, std::size_t bytes) {
			sp->OnWrite(ec, bytes);
		});
	}
}

void WebsocketClient::OnRead(beast::error_code ec, std::size_t bytes_transferred)
{
	if (ec)
	{
		// If error on reading, close conection
		OnClose(ec, "read");
		//fail(ec, "read");
		return;
	}

	if (!m_ws.is_open())
		return;

	size_t size = m_inputBuffer.size();
	size_t offset = 0;
	CScriptAny* parsed_data;

	while(offset < size)
	{
		parsed_data = new CScriptAny(ETHScriptWrapper::m_pASEngine);
		if (ParseMsgPack(parsed_data, net::buffer_cast<char const*>(m_inputBuffer.data()), size, offset))
		{
			// call AS OnMessage Callback
			if (m_onMessageCallback)
			{
				m_asContext->Prepare(m_onMessageCallback);
				m_asContext->SetObject(m_onMessageCallbackObject);

				// Set the function arguments
				m_asContext->SetArgObject(0, parsed_data);
				/*int r = */m_asContext->Execute();
				/*if (r == asEXECUTION_FINISHED)
				{
					// The return value is only valid if the execution finished successfully
					//asDWORD ret = m_as_ctx->GetReturnDWord();
				}*/
			}
		}
		else
		{
			std::stringstream ss; ss << "ParseMsgPack Failed." << std::endl;
			ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_WARNING);
			/*
				If parse failed, m_input_buffer may need to be consumed, or before that, can be send
				to a raw data callback.
			*/
		}
		parsed_data->Release();
	}
	m_inputBuffer.consume(size);

	// Read again, and again...
	m_ws.async_read(m_inputBuffer, beast::bind_front_handler(&WebsocketClient::OnRead, shared_from_this()));

	// Close the WebSocket connection... nooooooo! not now.
	//m_ws.async_close(websocket::close_code::normal, beast::bind_front_handler(&WebsocketClient::on_close, shared_from_this()));
}

void WebsocketClient::OnClose(beast::error_code ec, const std::string& origin)
{
	// call AS OnDisconnect callback
	if (m_onDisconnectCallback)
	{
		m_asContext->Prepare(m_onDisconnectCallback);
		m_asContext->SetObject(m_onDisconnectCallbackObject);

		// Set the function arguments
		//m_pScriptContext->SetArgDWord(...);
		/*int r = */m_asContext->Execute();
		/*if (r == asEXECUTION_FINISHED)
		{
			// The return value is only valid if the execution finished successfully
			//asDWORD ret = m_as_ctx->GetReturnDWord();
		}*/
	}

	// The make_printable() function helps print a ConstBufferSequence
	if (m_inputBuffer.size() > 0)
	{
		msgpack::object_handle inputOH;
		msgpack::unpack(inputOH, net::buffer_cast<char const *>(m_inputBuffer.data()), m_inputBuffer.size());
		msgpack::object inputObj = inputOH.get();
		m_inputBuffer.consume(m_inputBuffer.size());

		std::stringstream ss; ss << "Last input message: " << inputObj;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_WARNING);
	}

	m_closing = true;

	if (ec)
	{
		return Fail(ec, "close", origin.c_str());
	}

	// If we get here then the connection is closed gracefully
}

void WebsocketClient::Ping()
{
	if (!m_ws.is_open())
	{
		m_waitingPong = false;
		return;
	}

	if (!m_waitingPong)
	{
		m_ws.async_ping("ping", [this](beast::error_code ec){
			#ifdef _DEBUG
				std::cout << "ping sent!\n";
			#endif
			m_waitingPong = true;
			m_pingTime = boost::chrono::steady_clock::now();
		});
	}
}

void WebsocketClient::ClearBuffer()
{
	m_outputPd.clear();
}

// function to send a messages
void WebsocketClient::Send()
{
	// don't try to send when there is a writing in progress
	if (m_writingState)
		return;
	SendRaw(m_outputPd.data(), m_outputPd.size());
	ClearBuffer();
}

void WebsocketClient::SendRaw(char* data, size_t size)
{
	// don't try to send when there is a writing in progress (check just in case)
	if (m_writingState)
		return;

	// reserve space and copy data to output buffer
	net::buffer_copy(m_outputBuffer.prepare(size),net::buffer(data,size));
	m_outputBuffer.commit(size);

	if (m_ws.is_open() && m_outputBuffer.size() > 0)
	{
		m_ws.binary(true);
		// send (write) output buffer
		m_writingState = true;
		m_ws.async_write(m_outputBuffer.cdata(), [sp = shared_from_this()](beast::error_code ec, std::size_t bytes) {
			sp->OnWrite(ec, bytes);
		});
	}
}

bool WebsocketClient::IsConnected()
{
	return m_ws.is_open();
}

void WebsocketClient::Disconnect()
{
	if (!m_ws.is_open() || m_closing)
		return;

	m_closing = true;

	//m_ws.async_close(beast::websocket::close_code::normal,[sp = shared_from_this()](beast::error_code ec) {
	//	sp->OnClose(ec);
	//});
	
	auto self = shared_from_this();
	m_ws.async_close(beast::websocket::close_code::normal, [self](beast::error_code ec) {
		self->OnClose(ec, "disconnect");
	});
}

double WebsocketClient::GetLatency()
{
	return m_latency.get_average();
}

double WebsocketClient::GetUptime()
{
	boost::chrono::duration<double> ets = boost::chrono::steady_clock::now() - m_connectedAt;
	return ets.count();
}

bool WebsocketClient::IsCScriptArray(const int typeId)
{
	for (int8 i = 0; i < 15; i++)
	{
		if (m_arraytypeIds[i] == typeId)
			return true;
	}
	return false;
}

////////////////
//
//  Message Pack Serializer/Desserializer
//
///////////////

// Visitor for de-serializer
struct as_any_visitor : msgpack::v2::null_visitor {
	as_any_visitor(CScriptAny* ref_any) : m_any(ref_any), m_current_array(NULL),
		m_current_dictionary(NULL), m_current_is_dictionary(false),
		m_is_key(false), m_ref(false)
	{
		m_ti_any = ETHScriptWrapper::m_pASEngine->GetTypeInfoByDecl("any");
		m_ti_any_array = ETHScriptWrapper::m_pASEngine->GetTypeInfoByDecl("array<any>");
		m_ti_string = ETHScriptWrapper::m_pASEngine->GetTypeInfoByDecl("string");
		m_ti_dictionary = ETHScriptWrapper::m_pASEngine->GetTypeInfoByDecl("dictionary");
	}

	bool insert_value(void* ref, int refTypeId)
	{
		if (m_current_is_dictionary)
		{
			if (m_is_key)
			{
				if (refTypeId == m_ti_string->GetTypeId())
					m_current_key.push_back(*(dictKey_t*)ref);
				else
					return false;
			}
			else if (m_current_dictionary)
			{
				m_current_dictionary->Set(m_current_key.back(), ref, refTypeId);
				m_current_key.pop_back();
			}
			else
				return false;
		}
		else
		{
			CScriptAny* value;
			// If m_current_array is null, then return the any object
			if (m_current_array == nullptr)
				value = m_any;
			// else, we have a current array and will store a new Any object
			else
				value = new CScriptAny(ETHScriptWrapper::m_pASEngine);

			// that was needed because of this switch that handle non double/int64
			// primitives, Any's weirdness.
			switch (refTypeId)
			{
				case asTYPEID_FLOAT:
				// this context is needed because Store() accepts only double as floating point
				{
					double val = *(float*)ref;
					value->Store(val);
				}
				break;
				case asTYPEID_UINT64:
					value->Store(ref, asTYPEID_INT64);
					break;
				default:
					if (refTypeId > asTYPEID_DOUBLE || refTypeId == asTYPEID_VOID || refTypeId == asTYPEID_BOOL || refTypeId == asTYPEID_INT64 || refTypeId == asTYPEID_DOUBLE)
						value->Store(ref, refTypeId);
					// release if dictionary
					if (refTypeId == m_ti_dictionary->GetTypeId())
						((CScriptDictionary*)ref)->Release();

					// release if array
					if (refTypeId == m_ti_any_array->GetTypeId())
						((CScriptArray*)ref)->Release();

					// release if any
					if (refTypeId == m_ti_any->GetTypeId())
						((CScriptAny*)ref)->Release();

			}
			// at this point m_current_array may be null, check that again.
			if (m_current_array != nullptr)
			{
				// All that could be done with 'm_any->CopyFrom(value);' !?!?!?1
				m_current_array->InsertLast(value);
				value->Release();
			}
		}
		return true;
	}

	bool visit_nil() {
		return insert_value(0, asTYPEID_VOID);
	}

	bool visit_boolean(bool value) {
		return insert_value(&value, asTYPEID_BOOL);
	}
	
	bool visit_positive_integer(uint64_t value) {
		return insert_value(&value, asTYPEID_INT64);
	}
	
	bool visit_negative_integer(int64_t value) {
		return insert_value(&value, asTYPEID_INT64);
	}
	
	bool visit_str(const char* value, uint32_t size) {
		std::string string_value(value, size);
		return insert_value((void*)&string_value, m_ti_string->GetTypeId());
	}

	bool visit_float32(float value) {
		return insert_value(&value, asTYPEID_FLOAT);
	}

	bool visit_float64(double value) {
		return insert_value(&value, asTYPEID_DOUBLE);
	}

	bool start_array(uint32_t /*num_elements*/) {
		m_parent_is_dictionary.push_back(m_current_is_dictionary);
		m_current_is_dictionary = false;
		m_parent_array.push_back(m_current_array);
		return (m_current_array = CScriptArray::Create(m_ti_any_array));
	}

	bool end_array() {
		bool r;
		CScriptArray* temp = m_current_array;
		m_current_is_dictionary = m_parent_is_dictionary.back();
		m_parent_is_dictionary.pop_back();
		m_current_array = m_parent_array.back();
		m_parent_array.pop_back();
		r = insert_value(temp, m_ti_any_array->GetTypeId());
		temp->Release();
		return r;
	}

	bool start_map(uint32_t /*num_kv_pairs*/) {
		m_parent_is_dictionary.push_back(m_current_is_dictionary);
		m_current_is_dictionary = true;
		m_parent_dictionary.push_back(m_current_dictionary);
		return (m_current_dictionary = CScriptDictionary::Create(ETHScriptWrapper::m_pASEngine));
	}

	bool start_map_key() {
		m_is_key = true;
		return true;
	}

	bool end_map_key() {
		m_is_key = false;
		return true;
	}

	bool end_map_value() {
		return true;
	}

	bool end_map() {
		CScriptDictionary* temp = m_current_dictionary;
		m_current_is_dictionary = m_parent_is_dictionary.back();
		m_parent_is_dictionary.pop_back();
		m_current_dictionary = m_parent_dictionary.back();
		m_parent_dictionary.pop_back();
		return insert_value(temp, m_ti_dictionary->GetTypeId());
	}

	void parse_error(size_t /*parsed_offset*/, size_t error_offset) {
		// report error.
		std::stringstream ss; ss << "MsgPack Parse error -> error_offset: " << error_offset;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_WARNING);
	}

	void insufficient_bytes(size_t /*parsed_offset*/, size_t error_offset) {
		std::stringstream ss; ss << "MsgPack Insufficient Bytes -> error_offset: " << error_offset;
		ETHResourceProvider::Log(ss.str(), Platform::Logger::LT_WARNING);

	}

	void set_referenced(bool ref) { m_ref = ref; }
	bool referenced() const { return m_ref; }
	bool m_ref;

	bool m_is_key;
	std::vector<bool> m_parent_is_dictionary;
	bool m_current_is_dictionary;
	asITypeInfo* m_ti_any;
	asITypeInfo* m_ti_string;
	asITypeInfo* m_ti_any_array;
	asITypeInfo* m_ti_dictionary;
	std::vector<CScriptArray*> m_parent_array;
	CScriptArray* m_current_array;
	CScriptAny* const m_any;
	std::vector<dictKey_t> m_current_key;
	std::vector<CScriptDictionary*>  m_parent_dictionary;
	CScriptDictionary* m_current_dictionary;
};

struct do_nothing {
	void operator()(char* /*buffer*/) {
	}
};

// Serializer methods
void WebsocketClient::Pack(bool value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(uint8_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(uint16_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(uint32_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(uint64_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(int8_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(int16_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(int32_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(int64_t value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(float value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(double value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(const std::string& value)
{
	m_msgOut.pack(value);
}

void WebsocketClient::Pack(const void* address, int type_id)
{
	if (!address)
	{
		PackNil();
		return;
	}

	switch (type_id)
	{
	case asTYPEID_DOUBLE:
		Pack(*(double*)address);
		break;
	case asTYPEID_FLOAT:
		Pack(*(float*)address);
		break;
	case asTYPEID_INT64:
		Pack(*(int64_t*)address);
		break;
	case asTYPEID_INT32:
		Pack(*(int32_t*)address);
		break;
	case asTYPEID_INT16:
		Pack(*(int16_t*)address);
		break;
	case asTYPEID_INT8:
		Pack(*(int8_t*)address);
		break;
	case asTYPEID_UINT64:
		Pack(*(uint64_t*)address);
		break;
	case asTYPEID_UINT32:
		Pack(*(uint32_t*)address);
		break;
	case asTYPEID_UINT16:
		Pack(*(uint16_t*)address);
		break;
	case asTYPEID_UINT8:
		Pack(*(uint8_t*)address);
		break;
	case asTYPEID_BOOL:
		Pack(*(bool*)address);
		break;
	// Can not pack unknown type, so pack nil.
	case asTYPEID_VOID:
		PackNil();
		break;

	// If it is not a const value, use cached type_id (type_id defined at AS engine runtime)
	default:
		if (type_id == m_stringTypeId)
			Pack(*(std::string*)address);
		else if (type_id == m_vector2TypeId)
			Pack(*(Vector2*)address);
		else if (type_id == m_vector3TypeId)
			Pack(*(Vector3*)address);
		// check if type_id match with any of the template specialization
		else if (IsCScriptArray(type_id))
			Pack(*(CScriptArray*)address);
		else if (type_id == m_dictionaryTypeId)
			Pack((CScriptDictionary*)address);
		else if (type_id == m_anyTypeId)
			Pack((CScriptAny*)address);
	}
}

void WebsocketClient::Pack(CScriptDictionary* dictionary)
{
	if (dictionary->IsEmpty())
		return;

	asUINT size = dictionary->GetSize();
	PackMap(size);
	
	for (CScriptDictionary::CIterator it : *dictionary)
	{
		dictKey_t key = it.GetKey();
		const void* value_address = it.GetAddressOfValue();
		Pack(key);
		Pack(value_address, it.GetTypeId());
	}
}

void WebsocketClient::Pack(const CScriptAny* any)
{
	Pack(any->GetAddressOfValue(), any->GetTypeId());
}

void WebsocketClient::Pack(const CScriptArray& array)
{
	asUINT array_size = array.GetSize();
	PackArray(array_size);
	for (asUINT i = 0; i < array_size; i++)
		Pack(array.At(i), array.GetElementTypeId());
}

void WebsocketClient::Pack(const gs2d::math::Vector2& vector)
{
	PackArray(2);
	Pack(vector.x);
	Pack(vector.y);
}

void WebsocketClient::Pack(const gs2d::math::Vector2i& vector)
{
	PackArray(2);
	Pack(vector.x);
	Pack(vector.y);
}

void WebsocketClient::Pack(const gs2d::math::Vector3& vector)
{
	PackArray(3);
	Pack(vector.x);
	Pack(vector.y);
	Pack(vector.z);
}

void WebsocketClient::PackNil()
{
	m_msgOut.pack_nil();
}

// Create the array header on message pack, informing array size
void WebsocketClient::PackArray(uint32_t length)
{
	m_msgOut.pack_array(length);
}

// Create the map header on message pack, informing map size
void WebsocketClient::PackMap(uint32_t length)
{
	m_msgOut.pack_map(length);
}

// Use parse object to call message callback for each any object in stream
bool WebsocketClient::ParseMsgPack(CScriptAny* any, const char * data, const size_t size, std::size_t& offset)
{
	as_any_visitor visitor(any);
#ifdef _DEBUG
	std::cout << "\nMessage size: " << size << "\n";
#endif

	if (msgpack::v2::parse(data, size, offset, visitor))
	{
		return true;
	}
	
	return false;
}
