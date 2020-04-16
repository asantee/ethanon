#include "SharedData.h"

namespace Platform {

SharedData::SharedData(const bool constant) :
	m_constant(constant)
{
}

bool SharedData::Set(const std::string& data, const bool forceValue)
{
	const bool approved = (!IsConstant() || forceValue);
	if (approved)
	{
		m_data = data;
	}
	return approved;
}

std::string SharedData::Get() const
{
	return m_data;
}

bool SharedData::IsConstant() const
{
	return m_constant;
}

SharedData::operator std::string () const
{
	return m_data;
}

} // namespace Platform
