/*
This source code was orginally written to be a Vector3 class binder
and was edited by Santee in 2010 to bind a Vector2 class instead. 
The original source code for the vector3 binder comes with the AngelScript SDK.
*/

#include <assert.h>
#include <string.h> // strstr
#include <stdio.h>
#include <new> // new()
#include <math.h>
#include "scriptmath2d.h"

#include <Math/Vector2.h>

using gs2d::math::Vector2;
using gs2d::math::PODVector2;

BEGIN_AS_NAMESPACE

//-----------------------
// AngelScript functions
//-----------------------

static void Vector2DefaultConstructor(Vector2 *self)
{
	new(self) Vector2();
}

static void Vector2CopyConstructor(const Vector2 &other, Vector2 *self)
{
	new(self) Vector2(other);
}

static void Vector2InitConstructor(float x, float y, Vector2 *self)
{
	new(self) Vector2(x,y);
}

static void Vector2InitConstructor1F(float v, Vector2 *self)
{
	new(self) Vector2(v,v);
}

//-----------------------
// Generic calling convention
//-----------------------

static void Vector2DefaultConstructor_Generic(asIScriptGeneric *gen)
{
	Vector2 *self = (Vector2*)gen->GetObject();
	new(self) Vector2();
}

static void Vector2CopyConstructor_Generic(asIScriptGeneric *gen)
{
	Vector2 *other = (Vector2*)gen->GetArgObject(0);
	Vector2 *self = (Vector2*)gen->GetObject();
	Vector2CopyConstructor(*other, self);
}

static void Vector2InitConstructor_Generic(asIScriptGeneric *gen)
{
	float x = gen->GetArgFloat(0);
	float y = gen->GetArgFloat(1);
	Vector2 *self = (Vector2*)gen->GetObject();
	Vector2InitConstructor(x,y,self);
}

static void Vector2InitConstructor1F_Generic(asIScriptGeneric *gen)
{
	float v = gen->GetArgFloat(0);
	Vector2 *self = (Vector2*)gen->GetObject();
	Vector2InitConstructor1F(v,self);
}

static void Vector2Equal_Generic(asIScriptGeneric *gen)
{
	Vector2 *a = (Vector2*)gen->GetObject();
	Vector2 *b = (Vector2*)gen->GetArgAddress(0);
	bool r = *a == *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void Vector2Length_Generic(asIScriptGeneric *gen)
{
	Vector2 *s = (Vector2*)gen->GetObject();
	gen->SetReturnFloat(s->Length());
}

static void Vector2AddAssign_Generic(asIScriptGeneric *gen)
{
	Vector2 *a = (Vector2*)gen->GetArgAddress(0);
	Vector2 *thisPointer = (Vector2*)gen->GetObject();
	*thisPointer += *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2SubAssign_Generic(asIScriptGeneric *gen)
{
	Vector2 *a = (Vector2*)gen->GetArgAddress(0);
	Vector2 *thisPointer = (Vector2*)gen->GetObject();
	*thisPointer -= *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2MulAssign_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector2 *thisPointer = (Vector2*)gen->GetObject();
	*thisPointer *= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2DivAssign_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector2 *thisPointer = (Vector2*)gen->GetObject();
	*thisPointer /= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector2Add_Generic(asIScriptGeneric *gen)
{
	Vector2 *a = (Vector2*)gen->GetObject();
	Vector2 *b = (Vector2*)gen->GetArgAddress(0);
	Vector2 res = *a + *b;
	gen->SetReturnObject(&res);
}

static void Vector2Sub_Generic(asIScriptGeneric *gen)
{
	Vector2 *a = (Vector2*)gen->GetObject();
	Vector2 *b = (Vector2*)gen->GetArgAddress(0);
	Vector2 res = *a - *b;
	gen->SetReturnObject(&res);
}

static void Vector2FloatMulVector2_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector2 *v = (Vector2*)gen->GetObject();
	Vector2 res = (*v) * s;
	gen->SetReturnObject(&res);
}

static void Vector2Vector2MulFloat_Generic(asIScriptGeneric *gen)
{
	Vector2 *v = (Vector2*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector2 res = (*v) * s;
	gen->SetReturnObject(&res);
}

static void Vector2Vector2MulVector2_Generic(asIScriptGeneric *gen)
{
	Vector2 *v = (Vector2*)gen->GetObject();
	Vector2 *s = static_cast<Vector2*>(gen->GetArgObject(0));
	Vector2 res = (*v) * (*s);
	gen->SetReturnObject(&res);
}

static void Vector2Vector2DivFloat_Generic(asIScriptGeneric *gen)
{
	Vector2 *v = (Vector2*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector2 res = (*v) / s;
	gen->SetReturnObject(&res);
}

//--------------------------------
// Registration
//-------------------------------------

void RegisterScriptMath2D_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("vector2", "float x", offsetof(PODVector2, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("vector2", "float y", offsetof(PODVector2, y)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,  "void f()",                  asFUNCTION(Vector2DefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,  "void f(const vector2 &in)", asFUNCTION(Vector2CopyConstructor),    asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,  "void f(float, float)",      asFUNCTION(Vector2InitConstructor),    asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,  "void f(float)",             asFUNCTION(Vector2InitConstructor1F),  asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("vector2", "vector2 &opAddAssign(const vector2 &in)", asMETHODPR(Vector2, operator+=, (const Vector2 &), Vector2&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 &opSubAssign(const vector2 &in)", asMETHODPR(Vector2, operator-=, (const Vector2 &), Vector2&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 &opMulAssign(float)", asMETHODPR(Vector2, operator*=, (float), Vector2&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 &opDivAssign(float)", asMETHODPR(Vector2, operator/=, (float), Vector2&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "bool opEquals(const vector2 &in) const", asMETHODPR(Vector2, operator==, (const Vector2&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opAdd(const vector2 &in) const", asMETHODPR(Vector2, operator+, (const Vector2&) const, Vector2), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opSub(const vector2 &in) const", asMETHODPR(Vector2, operator-, (const Vector2&) const, Vector2), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opMul(float) const", asMETHODPR(Vector2, operator*, (float) const, Vector2), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opMul_r(float) const", asMETHODPR(Vector2, operator*, (float) const, Vector2), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opMul(const vector2 &in) const", asMETHODPR(Vector2, operator*, (const Vector2&) const, Vector2), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opDiv(float) const", asMETHODPR(Vector2, operator/, (float) const, Vector2), asCALL_THISCALL); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("vector2", "float length() const", asMETHOD(Vector2,Length), asCALL_THISCALL); assert( r >= 0 );
}

void RegisterScriptMath2D_Generic(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("vector2", sizeof(Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("vector2", "float x", offsetof(PODVector2, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("vector2", "float y", offsetof(PODVector2, y)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,    "void f()",                  asFUNCTION(Vector2DefaultConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,    "void f(const vector2 &in)", asFUNCTION(Vector2CopyConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,    "void f(float, float)",      asFUNCTION(Vector2InitConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector2", asBEHAVE_CONSTRUCT,    "void f(float)",             asFUNCTION(Vector2InitConstructor1F_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("vector2", "vector2 &opAddAssign(const vector2 &in)", asFUNCTION(Vector2AddAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 &opSubAssign(const vector2 &in)", asFUNCTION(Vector2SubAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 &opMulAssign(float)", asFUNCTION(Vector2MulAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 &opDivAssign(float)", asFUNCTION(Vector2DivAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "bool opEquals(const vector2 &in) const", asFUNCTION(Vector2Equal_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opAdd(const vector2 &in) const", asFUNCTION(Vector2Add_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opSub(const vector2 &in) const", asFUNCTION(Vector2Sub_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opMul_r(float) const", asFUNCTION(Vector2FloatMulVector2_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opMul(float) const", asFUNCTION(Vector2Vector2MulFloat_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opMul(const vector2 &in) const", asFUNCTION(Vector2Vector2MulVector2_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector2", "vector2 opDiv(float) const", asFUNCTION(Vector2Vector2DivFloat_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("vector2", "float length() const", asFUNCTION(Vector2Length_Generic), asCALL_GENERIC); assert( r >= 0 );
}

void RegisterScriptMath2D(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
		RegisterScriptMath2D_Generic(engine);
	else
		RegisterScriptMath2D_Native(engine);
}

END_AS_NAMESPACE


