/*
This source code was orginally written as a Vector3 class binder
and was edited by Santee in 2010 to bind a ScriptVector2 class instead. 
The original source code for the vector3 binder comes with the AngelScript SDK.
*/

#ifndef SCRIPTMATH2D_H
#define SCRIPTMATH2D_H

#include "../angelscript/include/angelscript.h"

BEGIN_AS_NAMESPACE

// This function will determine the configuration of the engine
// and use one of the two functions below to register the string type
void RegisterScriptMath2D(asIScriptEngine *engine);

// Call this function to register the math functions using native calling conventions
void RegisterScriptMath2D_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptMath2D_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
