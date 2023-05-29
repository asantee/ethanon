#include "SharedData.h"

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
SharedDataSecured::SharedDataSecured()
{
	wrapperfactory factory;
	m_wrapper = boost::shared_ptr<hashwrapper>(factory.create(HL_SHA1));
}

void SharedDataSecured::Set(const std::string& data)
{
	SharedData::Set(data);
	m_hash = GenerateHash();
}

std::string SharedDataSecured::GenerateHash() const
{
	return m_wrapper->getHashFromString(m_data + KeyProvider::ProvideKey());
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
