#include "AtomicString.h"

namespace Platform {

AtomicString::AtomicString() : m_value("")
{
}

AtomicString::AtomicString(const std::string& initValue)
	: m_value(initValue)
{
}

void AtomicString::set(const std::string& newValue)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_value = newValue;
}

std::string AtomicString::get() const
{
	std::lock_guard<std::mutex> lock(m_mtx);
	return m_value;
}

}
