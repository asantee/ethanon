/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

    Permission is hereby granted, free of charge, to any person obtaining a copy of this
    software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
--------------------------------------------------------------------------------------*/

#include "ETHScriptWrapper.h"

#include <boost/shared_ptr.hpp>

#include <hashlibpp.h>

#include <math.h>

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
