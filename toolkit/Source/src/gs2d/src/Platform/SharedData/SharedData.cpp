#include "SharedData.h"

#include "../Platform.h"
#include "../KeyProvider.h"

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
SharedDataSecured::SharedDataSecured(const std::string& data)
{
	Set(data);
}

void SharedDataSecured::Set(const std::string& data)
{
	SharedData::Set(data);
	m_hash = GenerateHash();
}

std::string SharedDataSecured::GenerateHash() const
{
	return Platform::GetMD5HashFromString(m_data + KeyProvider::ProvideKey());
}

bool SharedDataSecured::IsValid() const
{
	return (m_hash == GenerateHash());
}

std::string SharedDataSecured::Get() const
{
	if (IsValid())
		return SharedData::Get();
	else
		return "";
}

} // namespace Platform
