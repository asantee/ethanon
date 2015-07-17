#include <assert.h>
// #include <string.h> // strstr
#include <new> // new()
#include <math.h>
#include "scriptmath3d.h"

#include <Math/GameMath.h>

using gs2d::math::Vector3;
using gs2d::math::PODVector3;

BEGIN_AS_NAMESPACE

//-----------------------
// AngelScript functions
//-----------------------

static void Vector3DefaultConstructor(Vector3 *self)
{
	new(self) Vector3();
}

static void Vector3CopyConstructor(const Vector3 &other, Vector3 *self)
{
	new(self) Vector3(other);
}

static void Vector3InitConstructor(float x, float y, float z, Vector3 *self)
{
	new(self) Vector3(x,y,z);
}

static void Vector3InitConstructorV2(gs2d::math::Vector2 &xy, float z, Vector3 *self)
{
	new(self) Vector3(xy.x,xy.y,z);
}

static void Vector3InitConstructor1F(float v, Vector3 *self)
{
	new(self) Vector3(v, v, v);
}

//-----------------------
// Generic calling convention
//-----------------------

static void Vector3DefaultConstructor_Generic(asIScriptGeneric *gen)
{
	Vector3 *self = (Vector3*)gen->GetObject();
	new(self) Vector3();
}

static void Vector3CopyConstructor_Generic(asIScriptGeneric *gen)
{
	Vector3 *other = (Vector3*)gen->GetArgObject(0);
	Vector3 *self = (Vector3*)gen->GetObject();
	Vector3CopyConstructor(*other, self);
}

static void Vector3InitConstructor_Generic(asIScriptGeneric *gen)
{
	float x = gen->GetArgFloat(0);
	float y = gen->GetArgFloat(1);
	float z = gen->GetArgFloat(2);
	Vector3 *self = (Vector3*)gen->GetObject();
	Vector3InitConstructor(x,y,z,self);
}

static void Vector3InitConstructorV2_Generic(asIScriptGeneric *gen)
{
	float x = static_cast<gs2d::math::Vector2*>(gen->GetArgAddress(0))->x;
	float y = static_cast<gs2d::math::Vector2*>(gen->GetArgAddress(0))->y;
	float z = gen->GetArgFloat(1);
	Vector3 *self = (Vector3*)gen->GetObject();
	Vector3InitConstructor(x,y,z,self);
}

static void Vector3InitConstructor1F_Generic(asIScriptGeneric *gen)
{
	float v = gen->GetArgFloat(0);
	Vector3 *self = (Vector3*)gen->GetObject();
	Vector3InitConstructor1F(v,self);
}

static void Vector3Equal_Generic(asIScriptGeneric *gen)
{
	Vector3 *a = (Vector3*)gen->GetObject();
	Vector3 *b = (Vector3*)gen->GetArgAddress(0);
	bool r = *a == *b;
    *(bool*)gen->GetAddressOfReturnLocation() = r;
}

static void Vector3Length_Generic(asIScriptGeneric *gen)
{
	Vector3 *s = (Vector3*)gen->GetObject();
	gen->SetReturnFloat(s->Length());
}

static void Vector3AddAssign_Generic(asIScriptGeneric *gen)
{
	Vector3 *a = (Vector3*)gen->GetArgAddress(0);
	Vector3 *thisPointer = (Vector3*)gen->GetObject();
	*thisPointer += *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3SubAssign_Generic(asIScriptGeneric *gen)
{
	Vector3 *a = (Vector3*)gen->GetArgAddress(0);
	Vector3 *thisPointer = (Vector3*)gen->GetObject();
	*thisPointer -= *a;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3MulAssign_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector3 *thisPointer = (Vector3*)gen->GetObject();
	*thisPointer *= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3DivAssign_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector3 *thisPointer = (Vector3*)gen->GetObject();
	*thisPointer /= s;
	gen->SetReturnAddress(thisPointer);
}

static void Vector3Add_Generic(asIScriptGeneric *gen)
{
	Vector3 *a = (Vector3*)gen->GetObject();
	Vector3 *b = (Vector3*)gen->GetArgAddress(0);
	Vector3 res = *a + *b;
	gen->SetReturnObject(&res);
}

static void Vector3Sub_Generic(asIScriptGeneric *gen)
{
	Vector3 *a = (Vector3*)gen->GetObject();
	Vector3 *b = (Vector3*)gen->GetArgAddress(0);
	Vector3 res = *a - *b;
	gen->SetReturnObject(&res);
}

static void Vector3FloatMulVector3_Generic(asIScriptGeneric *gen)
{
	float s = gen->GetArgFloat(0);
	Vector3 *v = (Vector3*)gen->GetObject();
	Vector3 res = (*v) * s;
	gen->SetReturnObject(&res);
}

static void Vector3Vector3MulFloat_Generic(asIScriptGeneric *gen)
{
	Vector3 *v = (Vector3*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector3 res = (*v) * s;
	gen->SetReturnObject(&res);
}

static void Vector3Vector3MulVector3_Generic(asIScriptGeneric *gen)
{
	Vector3 *v = (Vector3*)gen->GetObject();
	Vector3 *s = static_cast<Vector3*>(gen->GetArgObject(0));
	Vector3 res = (*v) * (*s);
	gen->SetReturnObject(&res);
}

static void Vector3Vector3DivFloat_Generic(asIScriptGeneric *gen)
{
	Vector3 *v = (Vector3*)gen->GetObject();
	float s = gen->GetArgFloat(0);
	Vector3 res = (*v) / s;
	gen->SetReturnObject(&res);
}

//--------------------------------
// Registration
//-------------------------------------

void RegisterScriptMath3D_Native(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("vector3", sizeof(Vector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("vector3", "float x", offsetof(PODVector3, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("vector3", "float y", offsetof(PODVector3, y)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("vector3", "float z", offsetof(PODVector3, z)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,  "void f()",                         asFUNCTION(Vector3DefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,  "void f(const vector3 &in)",        asFUNCTION(Vector3CopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,  "void f(float, float, float)",      asFUNCTION(Vector3InitConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,  "void f(const vector2 &in, float)", asFUNCTION(Vector3InitConstructorV2), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,  "void f(float)",                    asFUNCTION(Vector3InitConstructor1F), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("vector3", "vector3 &opAddAssign(const vector3 &in)", asMETHODPR(Vector3, operator+=, (const Vector3 &), Vector3&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 &opSubAssign(const vector3 &in)", asMETHODPR(Vector3, operator-=, (const Vector3 &), Vector3&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 &opMulAssign(float)", asMETHODPR(Vector3, operator*=, (float), Vector3&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 &opDivAssign(float)", asMETHODPR(Vector3, operator/=, (float), Vector3&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "bool opEquals(const vector3 &in) const", asMETHODPR(Vector3, operator==, (const Vector3&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opAdd(const vector3 &in) const", asMETHODPR(Vector3, operator+, (const Vector3&) const, Vector3), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opSub(const vector3 &in) const", asMETHODPR(Vector3, operator-, (const Vector3&) const, Vector3), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opMul(float) const", asMETHODPR(Vector3, operator*, (float) const, Vector3), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opMul_r(float) const", asMETHODPR(Vector3, operator*, (float) const, Vector3), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opMul(const vector3 &in) const", asMETHODPR(Vector3, operator*, (const Vector3&) const, Vector3), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opDiv(float) const", asMETHODPR(Vector3, operator/, (float) const, Vector3), asCALL_THISCALL); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("vector3", "float length() const", asMETHOD(Vector3,Length), asCALL_THISCALL); assert( r >= 0 );
}

void RegisterScriptMath3D_Generic(asIScriptEngine *engine)
{
	int r;

	// Register the type
	r = engine->RegisterObjectType("vector3", sizeof(Vector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert( r >= 0 );

	// Register the object properties
	r = engine->RegisterObjectProperty("vector3", "float x", offsetof(PODVector3, x)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("vector3", "float y", offsetof(PODVector3, y)); assert( r >= 0 );
	r = engine->RegisterObjectProperty("vector3", "float z", offsetof(PODVector3, z)); assert( r >= 0 );

	// Register the constructors
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,    "void f()",                         asFUNCTION(Vector3DefaultConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,    "void f(const vector3 &in)",        asFUNCTION(Vector3CopyConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,    "void f(float, float, float)",      asFUNCTION(Vector3InitConstructor_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,    "void f(const vector2 &in, float)", asFUNCTION(Vector3InitConstructorV2_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("vector3", asBEHAVE_CONSTRUCT,    "void f(float)",                    asFUNCTION(Vector3InitConstructor1F_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the operator overloads
	r = engine->RegisterObjectMethod("vector3", "vector3 &opAddAssign(const vector3 &in)", asFUNCTION(Vector3AddAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 &opSubAssign(const vector3 &in)", asFUNCTION(Vector3SubAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 &opMulAssign(float)", asFUNCTION(Vector3MulAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 &opDivAssign(float)", asFUNCTION(Vector3DivAssign_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "bool opEquals(const vector3 &in) const", asFUNCTION(Vector3Equal_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opAdd(const vector3 &in) const", asFUNCTION(Vector3Add_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opSub(const vector3 &in) const", asFUNCTION(Vector3Sub_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opMul_r(float) const", asFUNCTION(Vector3FloatMulVector3_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opMul(float) const", asFUNCTION(Vector3Vector3MulFloat_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opMul(const vector3 &in) const", asFUNCTION(Vector3Vector3MulVector3_Generic), asCALL_GENERIC); assert( r >= 0 );
	r = engine->RegisterObjectMethod("vector3", "vector3 opDiv(float) const", asFUNCTION(Vector3Vector3DivFloat_Generic), asCALL_GENERIC); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("vector3", "float length() const", asFUNCTION(Vector3Length_Generic), asCALL_GENERIC); assert( r >= 0 );
}

void RegisterScriptMath3D(asIScriptEngine *engine)
{
	if( strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY") )
		RegisterScriptMath3D_Generic(engine);
	else
		RegisterScriptMath3D_Native(engine);
}

END_AS_NAMESPACE


