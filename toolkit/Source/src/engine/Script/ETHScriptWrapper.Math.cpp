#include "ETHScriptWrapper.h"

#include <Platform/Platform.h>

#include <boost/shared_ptr.hpp>

#include <math.h>

#if defined(_MSC_VER)
	#pragma warning (disable : 4512)
#endif

#include <hashlibpp.h>

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
