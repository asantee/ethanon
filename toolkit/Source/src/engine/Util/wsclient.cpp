#include "wsclient.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Report a failure
void fail(beast::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

WebsocketClient::WebsocketClient() : m_resolver(net::make_strand(m_ioc))
, m_ws(net::make_strand(m_ioc))
, m_keepalive_timeout(30)
, m_ref(1)
, m_gc_flag(false)
, m_msg_out(&m_output_pd)
, m_on_connect_callback(0)
, m_on_connect_callbackObject(0)
, m_on_connect_callbackObjectType(0)
, m_on_message_callback(0)
, m_on_message_callbackObject(0)
, m_on_message_callbackObjectType(0)
, m_on_disconnect_callback(0)
, m_on_disconnect_callbackObject(0)
, m_on_disconnect_callbackObjectType(0)
{
	// Create a new context for execution
	m_as_ctx = ETHScriptWrapper::m_pASEngine->CreateContext();

	// Cache extra asITypeId for types used in ehtanon engine
	m_vector2_type_id = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("vector2");
	m_vector3_type_id = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("vector3");
	m_string_type_id = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("string");
	m_dictionary_type_id = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("dictionary");
	m_any_type_id = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("any");

	// get all the array type ids.
	m_array_type_ids[0] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int>");
	m_array_type_ids[1] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int8>");
	m_array_type_ids[2] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int16>");
	m_array_type_ids[3] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<int64>");
	m_array_type_ids[4] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint>");
	m_array_type_ids[5] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint8>");
	m_array_type_ids[6] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint16>");
	m_array_type_ids[7] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<uint64>");
	m_array_type_ids[8] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<float>");
	m_array_type_ids[9] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<double>");
	m_array_type_ids[10] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<string>");
	m_array_type_ids[11] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<dictionary>");
	m_array_type_ids[12] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<vector2>");
	m_array_type_ids[13] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<vector3>");
	m_array_type_ids[14] = ETHScriptWrapper::m_pASEngine->GetTypeIdByDecl("array<any>");
}

//////
//  Garbage Collect extra support behaviors

void WebsocketClient::SetGCFlag()
{
	// Set the gc flag as the high bit in the reference counter
	//m_ref |= 0x80000000;
	m_gc_flag = true;
}

bool WebsocketClient::GetGCFlag()
{
	// Return the gc flag
	//return (m_ref & 0x80000000) ? true : false;
	return m_gc_flag;
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
	engine->GCEnumCallback(m_on_connect_callback);
	engine->GCEnumCallback(m_on_connect_callbackObject);
}

void WebsocketClient::ReleaseAllReferences(asIScriptEngine* engine)
{
	// When we receive this call, we are as good as dead, but
	// the garbage collector will still hold a references to us, so we
	// cannot just delete ourself yet. Just free all references to other
	// objects that we hold

	if (m_on_connect_callback)
		m_on_connect_callback->Release();
	if (m_on_connect_callbackObject)
		engine->ReleaseScriptObject(m_on_connect_callbackObject, m_on_connect_callbackObjectType);
	m_on_connect_callback = 0;
	m_on_connect_callbackObject = 0;
	m_on_connect_callbackObjectType = 0;
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

void WebsocketClient::Update() {
	m_ioc.poll();
};

void WebsocketClient::Connect(const std::string& host, const std::string& port)
{
	Connect(host.c_str(), port.c_str());
}

void WebsocketClient::SetOnConnectCallback(asIScriptFunction* cb)
{

	// Release the previous callback, if any
	if (m_on_connect_callback)
		m_on_connect_callback->Release();
	if (m_on_connect_callbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_on_connect_callbackObject, m_on_connect_callbackObjectType);
	m_on_connect_callback = 0;
	m_on_connect_callbackObject = 0;
	m_on_connect_callbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_on_connect_callbackObject = cb->GetDelegateObject();
		m_on_connect_callbackObjectType = cb->GetDelegateObjectType();
		m_on_connect_callback = cb->GetDelegateFunction();

		// Hold on to the object and method

		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_on_connect_callbackObject, m_on_connect_callbackObjectType);
		m_on_connect_callback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();

	}
	else
	{
		// Store the received handle for later use
		m_on_connect_callback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

void WebsocketClient::SetOnMessageCallback(asIScriptFunction* cb)
{
	// Release the previous callback, if any
	if (m_on_message_callback)
		m_on_message_callback->Release();
	if (m_on_message_callbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_on_message_callbackObject, m_on_message_callbackObjectType);
	m_on_message_callback = 0;
	m_on_message_callbackObject = 0;
	m_on_message_callbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_on_message_callbackObject = cb->GetDelegateObject();
		m_on_message_callbackObjectType = cb->GetDelegateObjectType();
		m_on_message_callback = cb->GetDelegateFunction();

		// Hold on to the object and method
		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_on_message_callbackObject, m_on_message_callbackObjectType);
		m_on_message_callback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();

	}
	else
	{
		// Store the received handle for later use
		m_on_message_callback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

// Start the asynchronous operation
void WebsocketClient::Connect(char const* host, char const* port)
{
	if( m_ws.is_open() )
		return;
	// Save these for later
	m_host = host;
	m_port = port;
	Connect();
}

void WebsocketClient::Connect()
{
	if( m_ws.is_open() )
		return;

	// Look up the domain name
	m_resolver.async_resolve( m_host, m_port,
		beast::bind_front_handler(&WebsocketClient::OnResolve, shared_from_this()));
}

void WebsocketClient::OnResolve(beast::error_code ec, tcp::resolver::results_type results)
{
	if(ec)
		return fail(ec, "resolve");

	// Set the timeout for the operation
	beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

	// Make the connection on the IP address we get from a lookup
	beast::get_lowest_layer(m_ws).async_connect(
		results, beast::bind_front_handler(&WebsocketClient::OnConnect,shared_from_this()));
}

void WebsocketClient::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
	if(ec)
		return fail(ec, "connect");

	m_connected_at = boost::chrono::steady_clock::now();

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
			if(kind == websocket::frame_type::ping)
			{
				std::cout << "<<< received a ping: "<< data << std::endl;
			}
			else if(kind == websocket::frame_type::pong)
			{
				std::cout << "<<< received a pong: "<< data << std::endl;
				if(m_waiting_pong)
				{
					boost::chrono::duration<double> elapsed;
					elapsed = boost::chrono::steady_clock::now() - m_ping_time;
					m_latency.update(elapsed.count());
					m_waiting_pong = false;
				}
			}
			else if(kind == websocket::frame_type::close)
			{
				std::cout << "<<< received a close request: " << data << std::endl;
			}
		});

	// Perform the websocket handshake
	m_ws.async_handshake(m_host, "/", beast::bind_front_handler(&WebsocketClient::OnHandshake, shared_from_this()));
}

void WebsocketClient::OnHandshake(beast::error_code ec)
{
	if(ec)
		return fail(ec, "handshake");
	
	std::cerr << "Websocket handshake completed!\n";

	// call AS OnConnect method
	if(m_on_connect_callback)
	{
		m_as_ctx->Prepare(m_on_connect_callback);
		m_as_ctx->SetObject(m_on_connect_callbackObject);

		// Set the function arguments
		//m_pScriptContext->SetArgDWord(...);
		int r = m_as_ctx->Execute();
		if (r == asEXECUTION_FINISHED)
		{
			// The return value is only valid if the execution finished successfully
			//asDWORD ret = m_as_ctx->GetReturnDWord();
			std::cout << ">>> Execution finished!!!\n";
		}
	}

	// Start reading incomming messages
	m_ws.async_read(m_input_buffer, beast::bind_front_handler(&WebsocketClient::OnRead, shared_from_this()));
}

void WebsocketClient::OnWrite(beast::error_code ec, std::size_t bytes_transferred)
{
	if(ec)
		return fail(ec, "write");
	
	// msg sent, so -"the queue walks"- consume buffer.
	m_output_buffer.consume(bytes_transferred);

	if( m_output_buffer.size() <= 0 )
		return;

	// send message while buffer not empty
	if( m_ws.is_open() )
	{
		m_ws.binary(true);
		m_ws.async_write(m_output_buffer.cdata(), [sp = shared_from_this()](beast::error_code ec, std::size_t bytes){
			sp->OnWrite(ec, bytes);
		});
	}
}

void WebsocketClient::OnRead(beast::error_code ec, std::size_t bytes_transferred)
{
	if(ec)
	{
		return fail(ec, "read");
	}

	// if( !m_ws.is_open() ){
	// 	std::cerr << "Connection closed!" <<  std::endl;
	// 	return;
	// }

	std::string tipo("texto");
	// do something with it
	if(m_ws.got_binary())
	{
		tipo = "binario";
	}

	msgpack::unpacker pac;
	pac.reserve_buffer(m_input_buffer.size());
	memcpy(pac.buffer(), net::buffer_cast<char const *>(m_input_buffer.data()), m_input_buffer.size());
	pac.buffer_consumed(m_input_buffer.size());

	m_input_buffer.consume(m_input_buffer.size());
	m_ws.async_read(m_input_buffer, beast::bind_front_handler(&WebsocketClient::OnRead, shared_from_this()));

	msgpack::object_handle inputOH;
	while(pac.next(inputOH))
	{
		msgpack::object inputObj = inputOH.get();

		std::cout << "<<< " << tipo << " " << inputObj.type << ": " << inputObj << std::endl;
	}

	// Close the WebSocket connection... nooooooo! not now.
	//m_ws.async_close(websocket::close_code::normal, beast::bind_front_handler(&WebsocketClient::on_close, shared_from_this()));
}

void WebsocketClient::OnClose(beast::error_code ec)
{
	if(ec)
		return fail(ec, "close");

	// If we get here then the connection is closed gracefully

	// The make_printable() function helps print a ConstBufferSequence
	if( m_input_buffer.size() > 0 )
	{
		msgpack::object_handle inputOH;
		msgpack::unpack(inputOH, net::buffer_cast<char const *>(m_input_buffer.data()), m_input_buffer.size());
		msgpack::object inputObj = inputOH.get();
		m_input_buffer.consume(m_input_buffer.size());

		std::cout << "Last input message: " << inputObj << std::endl;
	}
}

void WebsocketClient::Ping()
{
	if(!m_ws.is_open()){
		m_waiting_pong = false;
		return;
	}

	if(m_waiting_pong)
	{
		boost::chrono::duration<double> elapsed;
		elapsed = boost::chrono::steady_clock::now() - m_ping_time;
		m_latency.update(elapsed.count());
	}
	m_ping_time = boost::chrono::steady_clock::now();
	m_ws.async_ping("ping",[sp = shared_from_this()](beast::error_code ec){
		std::cout << "ping sent!\n";
		sp->m_waiting_pong = true;
	});
}

void WebsocketClient::ClearBuffer()
{
	m_output_pd.clear();
}

// function to send a messages
void WebsocketClient::Send()
{
	SendRaw(m_output_pd.data(), m_output_pd.size());
	ClearBuffer();
}

void WebsocketClient::SendRaw(char* data, size_t size)
{

	// reserve space and copy data to output buffer
	net::buffer_copy(m_output_buffer.prepare(size),net::buffer(data,size));
	m_output_buffer.commit(size);

	/// Print package content as test ///
	msgpack::unpacker pac;

	pac.reserve_buffer(size);
	memcpy(pac.buffer(), data, size);
	pac.buffer_consumed(size);

	msgpack::object_handle inputOH;
	while(pac.next(inputOH)) {
		msgpack::object inputObj = inputOH.get();
		std::cout << ">>>" << " : " << inputObj << std::endl;
	}
	/// End print test ///

	if( m_ws.is_open() && m_output_buffer.size() > 0 )
	{
		m_ws.binary(true);
		// send (write) output buffer
		m_ws.async_write(m_output_buffer.cdata(), [sp = shared_from_this()](beast::error_code ec, std::size_t bytes){
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

	if( !m_ws.is_open() )
		return;

	m_ws.async_close(beast::websocket::close_code::normal,[sp = shared_from_this()](beast::error_code ec){
		sp->OnClose(ec);
	});	

}

double WebsocketClient::GetLatency()
{
	return m_latency.get_average();
}

double WebsocketClient::GetUptime()
{
	boost::chrono::duration<double> ets = boost::chrono::steady_clock::now() - m_connected_at;
	return ets.count();
}

////////////////
//
//  Message Pack Serializer/Desserializer
//
///////////////

void WebsocketClient::Pack(bool value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(uint8_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(uint16_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(uint32_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(uint64_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(int8_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(int16_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(int32_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(int64_t value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(float value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(double value)
{
	m_msg_out.pack(value);
}

void WebsocketClient::Pack(const std::string& value)
{
	m_msg_out.pack(value);
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
		if (type_id == m_string_type_id)
			Pack(*(std::string*)address);
		else if (type_id == m_vector2_type_id)
			Pack(*(Vector2*)address);
		else if (type_id == m_vector3_type_id)
			Pack(*(Vector3*)address);
		// check if type_id match with any of the template specialization
		else if (isCScriptArray(type_id))
			Pack(*(CScriptArray*)address);
		else if (type_id == m_dictionary_type_id)
			Pack((CScriptDictionary*)address);
		else if (type_id == m_any_type_id)
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

void WebsocketClient::Pack(const CScriptAny& any)
{
	void* address;
	any.Retrieve(address, asTYPEID_VOID);
	Pack(address, any.GetTypeId());
}

void WebsocketClient::Pack(const CScriptArray& array)
{
	array.GetSize();
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
	m_msg_out.pack_nil();
}

// Create the array header on message pack, informing array size
void WebsocketClient::PackArray(uint32_t length)
{
	m_msg_out.pack_array(length);
}

// Create the map header on message pack, informing map size
void WebsocketClient::PackMap(uint32_t length)
{
	m_msg_out.pack_map(length);
}
