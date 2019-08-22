#include "ETHScriptWrapper.h"

#include <boost/shared_ptr.hpp>

#include <math.h>

#if defined(_MSC_VER)
	#pragma warning (disable : 4512)
#endif

#include <hashlibpp.h>

std::string ETHScriptWrapper::Math::GetHashFromString(const std::string& type, const std::string& str)
{
	wrapperfactory factory;
	boost::shared_ptr<hashwrapper> wrapper(factory.create(type));
	return wrapper->getHashFromString(str);
}

std::string ETHScriptWrapper::Math::GetMD5HashFromString(const std::string& str)
{
	return GetHashFromString(("MD5"), str);
}

std::string ETHScriptWrapper::Math::GetSHA1HashFromString(const std::string& str)
{
	return GetHashFromString(("SHA1"), str);
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
