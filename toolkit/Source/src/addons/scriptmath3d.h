#ifndef SCRIPTMATH3D_H
#define SCRIPTMATH3D_H

#include "../angelscript/include/angelscript.h"

BEGIN_AS_NAMESPACE

// This function will determine the configuration of the engine
// and use one of the two functions below to register the string type
void RegisterScriptMath3D(asIScriptEngine *engine);

// Call this function to register the math functions using native calling conventions
void RegisterScriptMath3D_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptMath3D_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
