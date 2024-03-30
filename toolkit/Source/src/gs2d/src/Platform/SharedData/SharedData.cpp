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
SharedDataSecured::SharedDataSecured(const std::string& data, std::string(*key_function)())
{
	m_key_function = key_function;
	Set(data);
}

void SharedDataSecured::Set(const std::string& data)
{
	boost::unique_lock<boost::shared_mutex> lock(m_mtx);
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
	boost::shared_lock<boost::shared_mutex> lock(m_mtx);
	return NonAtomicIsValid();
}

std::string SharedDataSecured::Get() const
{
	boost::shared_lock<boost::shared_mutex> lock(m_mtx);
	if (NonAtomicIsValid())
		return m_data;
	else
		return "";
}

} // namespace Platform
