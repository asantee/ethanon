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

void ETHScriptWrapper::Math::RegisterGlobals(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterGlobalFunction("float length(const vector2 &in)", asFUNCTIONPR(Length, (const Vector2 &), float), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float length(const vector3 &in)", asFUNCTIONPR(Length, (const Vector3 &), float), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float sign(const float)", asFUNCTIONPR(Sign, (const float), float), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int sign(const int)", asFUNCTIONPR(Sign, (const int), int), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float distance(const vector2 &in, const vector2 &in)", asFUNCTIONPR(Distance, (const Vector2 &, const Vector2 &), float), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float distance(const vector3 &in, const vector3 &in)", asFUNCTIONPR(Distance, (const Vector3 &, const Vector3 &), float), asCALL_CDECL); assert(r >= 0);
}

