#include "ETHScriptWrapper.h"

#include <boost/shared_ptr.hpp>

#include <math.h>

#if defined(_MSC_VER)
	#pragma warning (disable : 4512)
#endif

#include <hashlibpp.h>

str_type::string ETHScriptWrapper::Math::GetHashFromString(const str_type::string& type, const str_type::string& str)
{
	wrapperfactory factory;
	boost::shared_ptr<hashwrapper> wrapper(factory.create(type));
	return wrapper->getHashFromString(str);
}

str_type::string ETHScriptWrapper::Math::GetMD5HashFromString(const str_type::string& str)
{
	return GetHashFromString(GS_L("MD5"), str);
}

str_type::string ETHScriptWrapper::Math::GetSHA1HashFromString(const str_type::string& str)
{
	return GetHashFromString(GS_L("SHA1"), str);
}

float ETHScriptWrapper::Math::Length(const Vector3& v)
{
	return v.Length();
}

float ETHScriptWrapper::Math::Length(const Vector2& v)
{
	return v.Length();
}

float ETHScriptWrapper::Math::Sign(const float v)
{
	return gs2d::math::Sign(v);
}

int ETHScriptWrapper::Math::Sign(const int v)
{
	return gs2d::math::Sign(v);
}

float ETHScriptWrapper::Math::Distance(const Vector2& a, const Vector2& b)
{
	return Length(a - b);
}

float ETHScriptWrapper::Math::Distance(const Vector3& a, const Vector3& b)
{
	return Length(a - b);
}
