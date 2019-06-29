#ifndef SHARED_DATA_H_
#define SHARED_DATA_H_

#include "../../Types.h"

namespace Platform {

class SharedData
{
	gs2d::str_type::string m_data;
	bool m_constant;

public:
	SharedData(const bool constant);
	bool Set(const gs2d::str_type::string& data, const bool forceValue = false);
	gs2d::str_type::string Get() const;
	bool IsConstant() const;
	operator gs2d::str_type::string () const;
};

typedef boost::shared_ptr<SharedData> SharedDataPtr;

} // namespace Platform

#endif
