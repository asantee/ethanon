#include "SharedData.h"

namespace Platform {

SharedData::SharedData(const bool constant) :
	m_constant(constant)
{
}

bool SharedData::Set(const gs2d::str_type::string& data, const bool forceValue)
{
	const bool approved = (!IsConstant() || forceValue);
	if (approved)
	{
		m_data = data;
	}
	return approved;
}

gs2d::str_type::string SharedData::Get() const
{
	return m_data;
}

bool SharedData::IsConstant() const
{
	return m_constant;
}

SharedData::operator gs2d::str_type::string () const
{
	return m_data;
}

} // namespace Platform
