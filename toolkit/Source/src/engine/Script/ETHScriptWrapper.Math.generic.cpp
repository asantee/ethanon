/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2012 Andre Santee
 http://www.asantee.net/ethanon/

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
#include <math.h>
#include "../addons/aswrappedcall.h"

asDECLARE_FUNCTION_WRAPPERPR(__length2, ETHScriptWrapper::Math::Length, (const Vector2&), float);
asDECLARE_FUNCTION_WRAPPERPR(__length3, ETHScriptWrapper::Math::Length, (const Vector3&), float);

asDECLARE_FUNCTION_WRAPPERPR(__signF, ETHScriptWrapper::Math::Sign, (const float), float);
asDECLARE_FUNCTION_WRAPPERPR(__signI, ETHScriptWrapper::Math::Sign, (const int),   int);

asDECLARE_FUNCTION_WRAPPERPR(__distance2, ETHScriptWrapper::Math::Distance, (const Vector2 &, const Vector2 &), float);
asDECLARE_FUNCTION_WRAPPERPR(__distance3, ETHScriptWrapper::Math::Distance, (const Vector3 &, const Vector3 &), float);

asDECLARE_FUNCTION_WRAPPERPR(__multiplyv2, gs2d::math::Multiply, (const Vector2&, const Matrix4x4&), Vector2);
asDECLARE_FUNCTION_WRAPPERPR(__multiplyv3, gs2d::math::Multiply, (const Vector3&, const Matrix4x4&), Vector3);

void ETHScriptWrapper::Math::RegisterGlobals(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterGlobalFunction("float length(const vector2 &in)",                      asFUNCTION(__length2),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float length(const vector3 &in)",                      asFUNCTION(__length3),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float sign(const float)",                              asFUNCTION(__signF),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int sign(const int)",                                  asFUNCTION(__signI),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float distance(const vector2 &in, const vector2 &in)", asFUNCTION(__distance2), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float distance(const vector3 &in, const vector3 &in)", asFUNCTION(__distance3), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("vector3 multiply(const vector3 &in, const matrix4x4 &in)", asFUNCTION(__multiplyv3), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 multiply(const vector2 &in, const matrix4x4 &in)", asFUNCTION(__multiplyv2), asCALL_GENERIC); assert(r >= 0);
}

