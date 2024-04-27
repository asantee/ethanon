#include "SharedData.h"

#include "../Platform.h"

namespace Platform {

//
//  SharedData
//////////////////////////////////////////////////////////////////////////////////////////
bool SharedData::IsValid() const
{
	return true;
}

void SharedData::Set(const std::string& data)
{
	m_data = data;
}

std::string SharedData::Get() const
{
	return m_data;
}

//
//  SharedDataSecured
//////////////////////////////////////////////////////////////////////////////////////////
SharedDataSecured::SharedDataSecured(const std::string& data, std::string(*key_function)()) :
	SharedData(),
	m_key_function(key_function)
{
	Set(data);
}

void SharedDataSecured::Set(const std::string& data)
{
#if defined(USE_MUTEX_ON_SHARED_DATA)
	boost::unique_lock<boost::shared_mutex> lock(m_mtx);
#endif
	m_data = data;
	m_hash = GenerateHash();
}

uint32_t SharedDataSecured::GenerateHash() const
{
	std::string key(m_key_function ? (*m_key_function)() : "");
	return Platform::GetFastHashFromString(m_data + key);
}

bool SharedDataSecured::NonAtomicIsValid() const
{
	return (m_hash == GenerateHash());
}

bool SharedDataSecured::IsValid() const
{
#if defined(USE_MUTEX_ON_SHARED_DATA)
	boost::shared_lock<boost::shared_mutex> lock(m_mtx);
#endif
	return NonAtomicIsValid();
}

std::string SharedDataSecured::Get() const
{
#if defined(USE_MUTEX_ON_SHARED_DATA)
	boost::shared_lock<boost::shared_mutex> lock(m_mtx);
#endif
	if (NonAtomicIsValid())
		return m_data;
	else
		return "";
}

} // namespace Platform
