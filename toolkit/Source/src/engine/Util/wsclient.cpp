#include "wsclient.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

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
, m_on_websocket_fail_callback(0)
, m_on_websocket_fail_callbackObject(0)
, m_on_websocket_fail_callbackObjectType(0)
{
	// Create a new context for execution
	m_as_ctx = ETHScriptWrapper::m_pASEngine->CreateContext();

	// Cache extra asITypeInfo for types used in ehtanon engine
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

	m_any_array_type_info = ETHScriptWrapper::m_pASEngine->GetTypeInfoByDecl("array<any>");
}

// Report a failure
void WebsocketClient::fail(beast::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
	std::string reason(ec.message());
	std::string what_str(what);
	if (m_on_websocket_fail_callback)
	{
		m_as_ctx->Prepare(m_on_websocket_fail_callback);
		m_as_ctx->SetObject(m_on_websocket_fail_callbackObject);

		// Set the function arguments (reason string)
		m_as_ctx->SetArgObject(0, &what_str);
		m_as_ctx->SetArgObject(1, &reason);
		int r = m_as_ctx->Execute();
	}
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

void WebsocketClient::SetOnDisconnectCallback(asIScriptFunction* cb)
{

	// Release the previous callback, if any
	if (m_on_disconnect_callback)
		m_on_disconnect_callback->Release();
	if (m_on_disconnect_callbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_on_disconnect_callbackObject, m_on_disconnect_callbackObjectType);
	m_on_disconnect_callback = 0;
	m_on_disconnect_callbackObject = 0;
	m_on_disconnect_callbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_on_disconnect_callbackObject = cb->GetDelegateObject();
		m_on_disconnect_callbackObjectType = cb->GetDelegateObjectType();
		m_on_disconnect_callback = cb->GetDelegateFunction();

		// Hold on to the object and method

		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_on_disconnect_callbackObject, m_on_disconnect_callbackObjectType);
		m_on_disconnect_callback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();

	}
	else
	{
		// Store the received handle for later use
		m_on_disconnect_callback = cb;

		// Do not release the received script function 
		// until it won't be used any more
	}
}

void WebsocketClient::SetOnWebsocketFailCallback(asIScriptFunction* cb)
{

	// Release the previous callback, if any
	if (m_on_websocket_fail_callback)
		m_on_websocket_fail_callback->Release();
	if (m_on_websocket_fail_callbackObject)
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(m_on_websocket_fail_callbackObject, m_on_websocket_fail_callbackObjectType);
	m_on_websocket_fail_callback = 0;
	m_on_websocket_fail_callbackObject = 0;
	m_on_websocket_fail_callbackObjectType = 0;

	if (cb && cb->GetFuncType() == asFUNC_DELEGATE)
	{
		m_on_websocket_fail_callbackObject = cb->GetDelegateObject();
		m_on_websocket_fail_callbackObjectType = cb->GetDelegateObjectType();
		m_on_websocket_fail_callback = cb->GetDelegateFunction();

		// Hold on to the object and method

		ETHScriptWrapper::m_pASEngine->AddRefScriptObject(m_on_websocket_fail_callbackObject, m_on_websocket_fail_callbackObjectType);
		m_on_websocket_fail_callback->AddRef();

		// Release the delegate, since it won't be used anymore
		ETHScriptWrapper::m_pASEngine->ReleaseScriptObject(cb->GetDelegateObject(), cb->GetDelegateObjectType());
		cb->Release();

	}
	else
	{
		// Store the received handle for later use
		m_on_websocket_fail_callback = cb;

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

	// Look up the domain name
	m_resolver.async_resolve( m_host, m_port,
		beast::bind_front_handler(&WebsocketClient::OnResolve, shared_from_this()));
}

void WebsocketClient::OnResolve(beast::error_code ec, tcp::resolver::results_type results)
{
	if (ec)
		return fail(ec, "resolve");

	// Set the timeout for the operation
	beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

	// Make the connection on the IP address we get from a lookup
	beast::get_lowest_layer(m_ws).async_connect(
		results, beast::bind_front_handler(&WebsocketClient::OnConnect,shared_from_this()));
}

void WebsocketClient::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
	if (ec)
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
	//m_ws.control_callback([sp = shared_from_this()](websocket::frame_type kind, beast::string_view data)
	m_ws.control_callback([this](websocket::frame_type kind, beast::string_view data)
	{
			if(kind == websocket::frame_type::ping)
			{
				std::cout << "<<< received a ping: "<< data << std::endl;
			}
			else if(kind == websocket::frame_type::pong)
			{
				if(m_waiting_pong)
				{
					boost::chrono::duration<double> elapsed;
					elapsed = boost::chrono::steady_clock::now() - m_ping_time;
					m_latency.update((double)elapsed.count());
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

	// call AS OnConnect callback
	if(m_on_connect_callback)
	{
		m_as_ctx->Prepare(m_on_connect_callback);
		m_as_ctx->SetObject(m_on_connect_callbackObject);

		// Set the function arguments
		//m_pScriptContext->SetArgDWord(...);
		int r = m_as_ctx->Execute();
		/*if (r == asEXECUTION_FINISHED)
		{
			// The return value is only valid if the execution finished successfully
			//asDWORD ret = m_as_ctx->GetReturnDWord();
		}*/
	}

	// Start reading incomming messages
	m_ws.async_read(m_input_buffer, beast::bind_front_handler(&WebsocketClient::OnRead, shared_from_this()));
}

void WebsocketClient::OnWrite(beast::error_code ec, std::size_t bytes_transferred)
{
	if(ec)
		return	fail(ec, "write");
	
	// msg sent, consume output buffer.
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
		// If error on reading, close conection
		OnClose(ec);
		//fail(ec, "read");
		return;
	}

	if( !m_ws.is_open() ){
		return;
	}

	CScriptAny* parsed_data = new CScriptAny(ETHScriptWrapper::m_pASEngine);
	size_t size = m_input_buffer.size();
	size_t offset = 0;
	
	while(offset < size)
	{
		if (ParseMsgPack(parsed_data, net::buffer_cast<char const*>(m_input_buffer.data()), size, offset))
		{
			// call AS OnMessage Callback
			if (m_on_message_callback)
			{
				m_as_ctx->Prepare(m_on_message_callback);
				m_as_ctx->SetObject(m_on_message_callbackObject);

				// Set the function arguments
				m_as_ctx->SetArgObject(0, parsed_data);
				int r = m_as_ctx->Execute();
				/*if (r == asEXECUTION_FINISHED)
				{
					// The return value is only valid if the execution finished successfully
					//asDWORD ret = m_as_ctx->GetReturnDWord();
				}*/
			}
		}
		else
		{
			std::cout << "\n\nParseMsgPack Failed.\n\n";
			/*
				If parse failed, m_input_buffer may need to be consumed, or before that, can be send
				to a raw data callback.
			*/
		}
	}
	m_input_buffer.consume(size);

	// Read again, and again...
	m_ws.async_read(m_input_buffer, beast::bind_front_handler(&WebsocketClient::OnRead, shared_from_this()));

	// Close the WebSocket connection... nooooooo! not now.
	//m_ws.async_close(websocket::close_code::normal, beast::bind_front_handler(&WebsocketClient::on_close, shared_from_this()));
}

void WebsocketClient::OnClose(beast::error_code ec)
{
	// call AS OnDisconnect callback
	if (m_on_disconnect_callback)
	{
		m_as_ctx->Prepare(m_on_disconnect_callback);
		m_as_ctx->SetObject(m_on_disconnect_callbackObject);

		// Set the function arguments
		//m_pScriptContext->SetArgDWord(...);
		int r = m_as_ctx->Execute();
		/*if (r == asEXECUTION_FINISHED)
		{
			// The return value is only valid if the execution finished successfully
			//asDWORD ret = m_as_ctx->GetReturnDWord();
		}*/
	}

	// The make_printable() function helps print a ConstBufferSequence
	if( m_input_buffer.size() > 0 )
	{
		msgpack::object_handle inputOH;
		msgpack::unpack(inputOH, net::buffer_cast<char const *>(m_input_buffer.data()), m_input_buffer.size());
		msgpack::object inputObj = inputOH.get();
		m_input_buffer.consume(m_input_buffer.size());

		std::cout << "Last input message: " << inputObj << std::endl;
	}

	if (ec)
		return fail(ec, "close");

	// If we get here then the connection is closed gracefully
}

void WebsocketClient::Ping()
{
	if(!m_ws.is_open()){
		m_waiting_pong = false;
		return;
	}

	if (!m_waiting_pong)
	{
		m_ws.async_ping("ping", [this](beast::error_code ec){
				std::cout << "ping sent!\n";
			m_waiting_pong = true;
			m_ping_time = boost::chrono::steady_clock::now();
		});
	}
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
			}
			// at this point m_current_array may be null, check that again.
			if (m_current_array != nullptr)
				m_current_array->InsertLast(value);
			// All that could be done with 'm_any->CopyFrom(value);'
			// but that copies a lot of things, and why not mess with pointers?
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
		CScriptArray* temp = m_current_array;
		m_current_is_dictionary = m_parent_is_dictionary.back();
		m_parent_is_dictionary.pop_back();
		m_current_array = m_parent_array.back();
		m_parent_array.pop_back();
		return insert_value(temp, m_ti_any_array->GetTypeId());
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
		std::cout << "\n ### \n MsgPack Parse error -> error_offset: " << error_offset << "\n ### \n";
	}

	void insufficient_bytes(size_t /*parsed_offset*/, size_t error_offset) {
		std::cout << "\n ### \n MsgPack Insufficient Bytes -> error_offset: " << error_offset << "\n ### \n";
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

// Use parse object to call message callback for each any object in stream
bool WebsocketClient::ParseMsgPack(CScriptAny* any, const char * data, const size_t size, std::size_t& offset)
{
	as_any_visitor visitor(any);
	std::cout << "\nMessage size: " << size << "\n";

	if (msgpack::v2::parse(data, size, offset, visitor))
	{
		return true;
	}
	
	return false;
}
