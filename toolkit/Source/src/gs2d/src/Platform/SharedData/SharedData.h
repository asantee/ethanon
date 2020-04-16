#ifndef SHARED_DATA_H_
#define SHARED_DATA_H_

#include <string>

#include <boost/shared_ptr.hpp>

namespace Platform {

class SharedData
{
	std::string m_data;
	bool m_constant;

public:
	SharedData(const bool constant);
	bool Set(const std::string& data, const bool forceValue = false);
	std::string Get() const;
	bool IsConstant() const;
	operator std::string () const;
};

typedef boost::shared_ptr<SharedData> SharedDataPtr;

} // namespace Platform

#endif
