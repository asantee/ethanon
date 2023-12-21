#ifndef ATOMIC_STRING_H_
#define ATOMIC_STRING_H_

#include <string>
#include <mutex>

namespace Platform {

class AtomicString
{
	mutable std::mutex m_mtx;
	std::string m_value;

public:
	AtomicString();
	explicit AtomicString(const std::string& initValue);
	void set(const std::string& newValue);
	std::string get() const;
};

}

#endif
