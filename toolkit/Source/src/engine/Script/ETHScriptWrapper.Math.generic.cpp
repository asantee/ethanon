#include "ETHScriptWrapper.h"

#include <Math/Matrix4x4.h>

#include <math.h>

#include "../addons/aswrappedcall.h"

asDECLARE_FUNCTION_WRAPPERPR(__length2, ETHScriptWrapper::Math::Length, (const Vector2&), float);
asDECLARE_FUNCTION_WRAPPERPR(__length3, ETHScriptWrapper::Math::Length, (const Vector3&), float);

asDECLARE_FUNCTION_WRAPPERPR(__signF, ETHScriptWrapper::Math::Sign, (const float), float);
asDECLARE_FUNCTION_WRAPPERPR(__signI, ETHScriptWrapper::Math::Sign, (const int),   int);

asDECLARE_FUNCTION_WRAPPERPR(__distance2, ETHScriptWrapper::Math::Distance, (const Vector2 &, const Vector2 &), float);
asDECLARE_FUNCTION_WRAPPERPR(__distance3, ETHScriptWrapper::Math::Distance, (const Vector3 &, const Vector3 &), float);

asDECLARE_FUNCTION_WRAPPERPR(__multiplyv2, Matrix4x4::Multiply, (const Vector2&, const Matrix4x4&), Vector2);
asDECLARE_FUNCTION_WRAPPERPR(__multiplyv3, Matrix4x4::Multiply, (const Vector3&, const Matrix4x4&), Vector3);

asDECLARE_FUNCTION_WRAPPER(__getHashFromString,     ETHScriptWrapper::Math::GetHashFromString);
asDECLARE_FUNCTION_WRAPPER(__getMD5HashFromString,  ETHScriptWrapper::Math::GetMD5HashFromString);
asDECLARE_FUNCTION_WRAPPER(__getSHA1HashFromString, ETHScriptWrapper::Math::GetSHA1HashFromString);

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

	r = pASEngine->RegisterGlobalFunction("string getHashFromString(const string &in, const string &in)", asFUNCTION(__getHashFromString),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string getMD5HashFromString(const string &in)",                asFUNCTION(__getMD5HashFromString), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string getSHA1HashFromString(const string &in)",               asFUNCTION(__getSHA1HashFromString), asCALL_GENERIC); assert(r >= 0);
}
