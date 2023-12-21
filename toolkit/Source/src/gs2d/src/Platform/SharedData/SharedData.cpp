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
	m_data.set(data);
}

std::string SharedData::Get() const
{
	return m_data.get();
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
	std::lock_guard<std::mutex> lock(m_mtx);
	SharedData::Set(data);
	m_hash = GenerateHash();
}

std::string SharedDataSecured::GenerateHash() const
{
	std::string key(m_key_function ? (*m_key_function)() : "");
	return Platform::GetMD5HashFromString(m_data.get() + key);
}

bool SharedDataSecured::NonAtomicIsValid() const
{
	return (m_hash == GenerateHash());
}

bool SharedDataSecured::IsValid() const
{
	std::lock_guard<std::mutex> lock(m_mtx);
	return NonAtomicIsValid();
}

std::string SharedDataSecured::Get() const
{
	std::lock_guard<std::mutex> lock(m_mtx);
	if (NonAtomicIsValid())
		return SharedData::Get();
	else
		return "";
}

} // namespace Platform
