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

#include "ETHScriptObjRegister.h"
#include "../Util/ETHInput.h"
#include "../Physics/ETHPhysicsController.h"
#include "../../addons/aswrappedcall.h"

namespace ETHGlobal {

asDECLARE_FUNCTION_WRAPPER(__DummyAddRef,  DummyAddRef);
asDECLARE_FUNCTION_WRAPPER(__DummyRelease, DummyRelease);

asDECLARE_METHOD_WRAPPERPR(__GetLeftClickState,   ETHInput, GetLeftClickState,   (void),         GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__GetRightClickState,  ETHInput, GetRightClickState,  (void),         GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__GetMiddleClickState, ETHInput, GetMiddleClickState, (void),         GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__GetMouseMove,        ETHInput, GetMouseMove,        (void),         Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetWheelState,       ETHInput, GetWheelState,       (void),         float);
asDECLARE_METHOD_WRAPPERPR(__GetKeyState,         ETHInput, GetKeyState,         (const GS_KEY), GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__KeyDown,             ETHInput, KeyDown,             (const GS_KEY), bool);

asDECLARE_METHOD_WRAPPERPR(__JoyButtonState,     ETHInput, JoyButtonState,     (const unsigned int, const GS_JOYSTICK_BUTTON) const, GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__JoyButtonDown,      ETHInput, JoyButtonDown,      (const unsigned int, const GS_JOYSTICK_BUTTON) const, bool);

asDECLARE_METHOD_WRAPPERPR(__DetectJoysticks,    ETHInput, DetectJoysticks,    (void) const,               GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__GetJoystickStatus,  ETHInput, GetJoystickStatus,  (const unsigned int) const, GS_JOYSTICK_STATUS);
asDECLARE_METHOD_WRAPPERPR(__GetNumJoyButtons,   ETHInput, GetNumJoyButtons,   (const unsigned int) const, unsigned int);
asDECLARE_METHOD_WRAPPERPR(__GetJoystickXY,      ETHInput, GetJoystickXY,      (const unsigned int) const, Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetJoystickZ,       ETHInput, GetJoystickZ,       (const unsigned int) const, float);
asDECLARE_METHOD_WRAPPERPR(__GetJoystickRudder,  ETHInput, GetJoystickRudder,  (const unsigned int) const, float);
asDECLARE_METHOD_WRAPPERPR(__GetJoystickUV,      ETHInput, GetJoystickUV,      (const unsigned int) const, Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetFirstButtonDown, ETHInput, GetFirstButtonDown, (const unsigned int) const, GS_JOYSTICK_BUTTON);
asDECLARE_METHOD_WRAPPERPR(__GetMaxJoysticks,    ETHInput, GetMaxJoysticks,    (void) const,               unsigned int);

asDECLARE_METHOD_WRAPPERPR(__GetTouchPos,          ETHInput, GetTouchPos,          (const unsigned int n) const, Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetTouchState,        ETHInput, GetTouchState,        (const unsigned int n) const, GS_KEY_STATE);
asDECLARE_METHOD_WRAPPERPR(__GetMaxTouchCount,     ETHInput, GetMaxTouchCount,     (void) const,                 unsigned int);
asDECLARE_METHOD_WRAPPERPR(__GetTouchMove,         ETHInput, GetTouchMove,         (const unsigned int n) const, Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetAccelerometerData, ETHInput, GetAccelerometerData, (void) const,                 Vector3);

void RegisterInputMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHInput", asBEHAVE_ADDREF,  "void f()", asFUNCTION(__DummyAddRef),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHInput", asBEHAVE_RELEASE, "void f()", asFUNCTION(__DummyRelease),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetLeftClickState()",    asFUNCTION(__GetLeftClickState),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetRightClickState()",   asFUNCTION(__GetRightClickState),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetMiddleClickState()",  asFUNCTION(__GetMiddleClickState), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetMouseMove()",           asFUNCTION(__GetMouseMove),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "float GetWheelState()",            asFUNCTION(__GetWheelState),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetKeyState(const KEY)", asFUNCTION(__GetKeyState),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "bool KeyDown(const KEY)",          asFUNCTION(__KeyDown),             asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE JoyButtonState(const uint, const J_KEY) const", asFUNCTION(__JoyButtonState), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "bool JoyButtonDown(const uint, const J_KEY) const",       asFUNCTION(__JoyButtonDown),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "bool DetectJoysticks() const",                 asFUNCTION(__DetectJoysticks),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "J_STATUS GetJoystickStatus(const uint) const", asFUNCTION(__GetJoystickStatus),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "uint GetNumJoyButtons(const uint) const",      asFUNCTION(__GetNumJoyButtons),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetJoystickXY(const uint) const",      asFUNCTION(__GetJoystickXY),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "float GetJoystickZ(const uint) const",         asFUNCTION(__GetJoystickZ),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "float GetJoystickRudder(const uint) const",    asFUNCTION(__GetJoystickRudder),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetJoystickUV(const uint) const",      asFUNCTION(__GetJoystickUV),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "J_KEY GetFirstButtonDown(const uint) const",   asFUNCTION(__GetFirstButtonDown), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "uint GetMaxJoysticks() const",                 asFUNCTION(__GetMaxJoysticks),    asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetTouchPos(const uint) const",     asFUNCTION(__GetTouchPos),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetTouchState(const uint) const", asFUNCTION(__GetTouchState),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "uint GetMaxTouchCount() const",             asFUNCTION(__GetMaxTouchCount),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetTouchMove(const uint) const",    asFUNCTION(__GetTouchMove),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector3 GetAccelerometerData() const",      asFUNCTION(__GetAccelerometerData), asCALL_GENERIC); assert(r >= 0);
}

asDECLARE_FUNCTION_WRAPPER(__EnmlFileFactory,   EnmlFileFactory);
asDECLARE_FUNCTION_WRAPPER(__EnmlEntityFactory, EnmlEntityFactory);
//asDECLARE_FUNCTION_OBJ_WRAPPER(__EnmlEntityCopyConstructor, EnmlEntityCopyConstructor, false);

asDECLARE_METHOD_WRAPPERPR(__addRefEntity,  enml::Entity, AddRef,  (void), void);
asDECLARE_METHOD_WRAPPERPR(__releaseEntity, enml::Entity, Release, (void), void);

asDECLARE_METHOD_WRAPPERPR(__addRefFile,  enml::File, AddRef,  (void), void);
asDECLARE_METHOD_WRAPPERPR(__releaseFile, enml::File, Release, (void), void);

asDECLARE_METHOD_WRAPPERPR(__clearEntity,        enml::Entity, Clear,             (void),                                      void);
asDECLARE_METHOD_WRAPPERPR(__add,                enml::Entity, Add,               (const str_type::string&, str_type::string), void);
asDECLARE_METHOD_WRAPPERPR(__getEntity,          enml::Entity, Get,               (const str_type::string&),                   str_type::string);
asDECLARE_METHOD_WRAPPERPR(__getAttributeNamesE, enml::Entity, GetAttributeNames, (void) const,                                str_type::string);

asDECLARE_METHOD_WRAPPERPR(__clearFile,         enml::File, Clear,             (void),																	void);
asDECLARE_METHOD_WRAPPERPR(__addEntity,         enml::File, AddEntity,         (const str_type::string&, const enml::Entity&),							void);
asDECLARE_METHOD_WRAPPERPR(__generateString,    enml::File, GenerateString,    (void) const,															str_type::string);
asDECLARE_METHOD_WRAPPERPR(__parseString,       enml::File, ParseString,       (const str_type::string&),												unsigned int);
asDECLARE_METHOD_WRAPPERPR(__exists,            enml::File, Exists,            (const str_type::string&) const,											bool);
asDECLARE_METHOD_WRAPPERPR(__getFile,           enml::File, Get,               (const str_type::string&, const str_type::string&) const,				str_type::string);
asDECLARE_METHOD_WRAPPERPR(__getDouble,         enml::File, GetDouble,         (const str_type::string&, const str_type::string&, double&) const,		bool);
asDECLARE_METHOD_WRAPPERPR(__getInt,            enml::File, GetInt,            (const str_type::string&, const str_type::string&, int&) const,			bool);
asDECLARE_METHOD_WRAPPERPR(__getUInt,           enml::File, GetUInt,           (const str_type::string&, const str_type::string&, unsigned int&) const,	bool);
asDECLARE_METHOD_WRAPPERPR(__getFloat,          enml::File, GetFloat,          (const str_type::string&, const str_type::string&, float&) const,		bool);
asDECLARE_METHOD_WRAPPERPR(__getErrorString,    enml::File, GetErrorString,    (void) const,															str_type::string);
asDECLARE_METHOD_WRAPPERPR(__getEntityNames,    enml::File, GetEntityNames,    (void) const,															str_type::string);
asDECLARE_METHOD_WRAPPERPR(__getAttributeNamesF,enml::File, GetAttributeNames, (const str_type::string&) const,											str_type::string);
asDECLARE_METHOD_WRAPPERPR(__writeToFile,       enml::File, WriteToFile,       (const str_type::string&) const,											bool);
asDECLARE_METHOD_WRAPPERPR(__parseFromFile,     enml::File, ParseFromFile,     (const str_type::string&),												bool);
asDECLARE_METHOD_WRAPPERPR(__addValue,          enml::File, AddValue,          (const str_type::string&, const str_type::string&, const str_type::string&), void);

asDECLARE_FUNCTION_WRAPPER(__GetStringFromFile, enml::GetStringFromAnsiFile);
asDECLARE_FUNCTION_WRAPPER(__SaveStringToFile,  enml::SaveStringToAnsiFile);

void RegisterENMLMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_FACTORY, "enmlEntity@ f()", asFUNCTION(__EnmlEntityFactory), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_ADDREF,  "void f()",        asFUNCTION(__addRefEntity),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_RELEASE, "void f()",        asFUNCTION(__releaseEntity),     asCALL_GENERIC); assert(r >= 0);
	//r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_CONSTRUCT, "void f(const enmlEntity &in)", asFUNCTION(__EnmlEntityCopyConstructor), asCALL_GENERIC); assert( r >= 0 );

	// methods
	r = pASEngine->RegisterObjectMethod("enmlEntity", "void clear()",                       asFUNCTION(__clearEntity),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlEntity", "void add(const string &in, string)", asFUNCTION(__add),               asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlEntity", "string get(const string &in)",       asFUNCTION(__getEntity),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlEntity", "string getAttributeNames() const",   asFUNCTION(__getAttributeNamesE),asCALL_GENERIC); assert(r >= 0);

	// register the ENML file object
	r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_FACTORY, "enmlFile@ f()", asFUNCTION(__EnmlFileFactory), asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_ADDREF,  "void f()",      asFUNCTION(__addRefFile),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_RELEASE, "void f()",      asFUNCTION(__releaseFile),     asCALL_GENERIC); assert(r >= 0);

	// methods
	r = pASEngine->RegisterObjectMethod("enmlFile", "void clear()",															asFUNCTION(__clearFile),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "void addEntity(const string &in, const enmlEntity &in)",				asFUNCTION(__addEntity),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string generateString() const",										asFUNCTION(__generateString),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "uint parseString(const string &in)",									asFUNCTION(__parseString),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool exists(const string &in) const",									asFUNCTION(__exists),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string get(const string &in, const string &in) const",					asFUNCTION(__getFile),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getDouble(const string &in, const string &in, double &out) const",asFUNCTION(__getDouble),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getInt(const string &in, const string &in, int &out) const",		asFUNCTION(__getInt),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getUInt(const string &in, const string &in, uint &out) const",	asFUNCTION(__getUInt),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getFloat(const string &in, const string &in, float &out) const",	asFUNCTION(__getFloat),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string getErrorString() const",										asFUNCTION(__getErrorString),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string getEntityNames() const",										asFUNCTION(__getEntityNames) ,   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string getAttributeNames(const string &in) const",						asFUNCTION(__getAttributeNamesF),asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "void writeToFile(const string &in) const",								asFUNCTION(__writeToFile),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool parseFromFile(const string &in)",									asFUNCTION(__parseFromFile),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "void addValue(const string& in, const string& in, const string& in)",  asFUNCTION(__addValue),			 asCALL_GENERIC); assert(r >= 0);

	// util globals
	r = pASEngine->RegisterGlobalFunction("string GetStringFromFile(const string &in)",                asFUNCTION(__GetStringFromFile), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SaveStringToFile(const string &in, const string &in)", asFUNCTION(__SaveStringToFile),  asCALL_GENERIC); assert(r >= 0);
}

asDECLARE_FUNCTION_WRAPPER(__ETHEntityArrayFactory, ETHEntityArrayFactory);
asDECLARE_METHOD_WRAPPERPR(__AddRefArray,  ETHEntityArray, AddRef,  (void), void);
asDECLARE_METHOD_WRAPPERPR(__ReleaseArray, ETHEntityArray, Release, (void), void);

asDECLARE_METHOD_WRAPPERPR(__operatorIdx,       ETHEntityArray, operator[], (const unsigned int),       ETHEntityRawPtr&);
asDECLARE_METHOD_WRAPPERPR(__operatorIdxConst,  ETHEntityArray, operator[], (const unsigned int) const, const ETHEntityRawPtr&);
asDECLARE_METHOD_WRAPPERPR(__operatorAddAssign, ETHEntityArray, operator+=, (const ETHEntityArray&),    ETHEntityArray&);

asDECLARE_METHOD_WRAPPERPR(__push_back,          ETHEntityArray, push_back,          (ETHEntityRawPtr), void);
asDECLARE_METHOD_WRAPPERPR(__size,               ETHEntityArray, size,               (void) const,      unsigned int);
asDECLARE_METHOD_WRAPPERPR(__clear,              ETHEntityArray, clear,              (void),            void);
asDECLARE_METHOD_WRAPPERPR(__unique,             ETHEntityArray, unique,             (void),            void);
asDECLARE_METHOD_WRAPPERPR(__removeDeadEntities, ETHEntityArray, removeDeadEntities, (void),            void);

void RegisterEntityArrayMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHEntityArray", asBEHAVE_FACTORY, "ETHEntityArray@ f()", asFUNCTION(__ETHEntityArrayFactory), asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("ETHEntityArray", asBEHAVE_ADDREF,  "void f()",            asFUNCTION(__AddRefArray),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHEntityArray", asBEHAVE_RELEASE, "void f()",            asFUNCTION(__ReleaseArray),          asCALL_GENERIC); assert(r >= 0);

	// methods and operators
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "ETHEntity@ &opIndex(uint)",                             asFUNCTION(__operatorIdx),       asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "const ETHEntity@ &opIndex(uint) const",                 asFUNCTION(__operatorIdxConst),  asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "ETHEntityArray &opAddAssign(const ETHEntityArray &in)", asFUNCTION(__operatorAddAssign), asCALL_GENERIC); assert( r >= 0 );

	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void Insert(const ETHEntity &in)", asFUNCTION(__push_back),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "uint Size() const",                asFUNCTION(__size),               asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void Clear()",                     asFUNCTION(__clear),              asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void Unique()",                    asFUNCTION(__unique),             asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void RemoveDeadEntities()",        asFUNCTION(__removeDeadEntities), asCALL_GENERIC); assert(r >= 0);

	#ifdef ETH_DEFINE_DEPRECATED_SIGNATURES_FROM_0_9_5
	{
		r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void push_back(const ETHEntity &in)", asFUNCTION(__push_back),          asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterObjectMethod("ETHEntityArray", "uint size() const",                   asFUNCTION(__size),               asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void clear()",                        asFUNCTION(__clear),              asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void unique()",                       asFUNCTION(__unique),             asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void removeDeadEntities()",           asFUNCTION(__removeDeadEntities), asCALL_GENERIC); assert(r >= 0);
	}
	#endif
}

asDECLARE_METHOD_WRAPPERPR(__AddRefEntity,  ETHScriptEntity, AddRef,  (void), void);
asDECLARE_METHOD_WRAPPERPR(__ReleaseEntity, ETHScriptEntity, Release, (void), void);

asDECLARE_METHOD_WRAPPERPR(__GetPosition,   ETHScriptEntity, GetPosition,    (void) const,         Vector3);
asDECLARE_METHOD_WRAPPERPR(__GetPositionXY, ETHScriptEntity, GetPositionXY,  (void) const,         Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetPositionX,  ETHScriptEntity, GetPositionX,   (void) const,         float);
asDECLARE_METHOD_WRAPPERPR(__GetPositionY,  ETHScriptEntity, GetPositionY,   (void) const,         float);
asDECLARE_METHOD_WRAPPERPR(__GetPositionZ,  ETHScriptEntity, GetPositionZ,   (void) const,         float);
asDECLARE_METHOD_WRAPPERPR(__GetSize,       ETHScriptEntity, GetCurrentSize, (void) const,         Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetFrame,      ETHScriptEntity, GetFrame,       (void) const,         unsigned int);
asDECLARE_METHOD_WRAPPERPR(__SetFrameN,     ETHScriptEntity, SetFrame,       (const unsigned int), bool);
asDECLARE_METHOD_WRAPPERPR(__SetFrameXY,    ETHScriptEntity, SetFrame,       (const unsigned int, const unsigned int), bool);

asDECLARE_METHOD_WRAPPERPR(__GetID,            ETHScriptEntity, GetID,            (void) const,  int);
asDECLARE_METHOD_WRAPPERPR(__GetAngleEnt,      ETHScriptEntity, GetAngle,         (void) const,  float);
asDECLARE_METHOD_WRAPPERPR(__SetAngle,         ETHScriptEntity, SetAngle,         (const float), void);
asDECLARE_METHOD_WRAPPERPR(__IsStatic,         ETHScriptEntity, IsStatic,         (void) const,  bool);

asDECLARE_METHOD_WRAPPERPR(__SilenceParticleSystems, ETHScriptEntity, SilenceParticleSystems, (const bool),                    void);
asDECLARE_METHOD_WRAPPERPR(__SetSoundVolume,         ETHScriptEntity, SetSoundVolume,         (const float),                   void);
asDECLARE_METHOD_WRAPPERPR(__GetCollisionBox,        ETHScriptEntity, GetCollisionBox,        (void) const,                    ETHCollisionBox);
asDECLARE_METHOD_WRAPPERPR(__GetEntityName,          ETHScriptEntity, GetEntityName,          (void) const,                    str_type::string);
asDECLARE_METHOD_WRAPPERPR(__Collidable,             ETHScriptEntity, IsCollidable,           (void) const,                    bool);
asDECLARE_METHOD_WRAPPERPR(__CheckCustomData,        ETHScriptEntity, CheckCustomData,        (const str_type::string&) const, ETHCustomData::DATA_TYPE);
asDECLARE_METHOD_WRAPPERPR(__HasCustomData,          ETHScriptEntity, HasCustomData,          (void) const,                    bool);
asDECLARE_METHOD_WRAPPERPR(__DebugPrintCustomData,   ETHScriptEntity, DebugPrintCustomData,   (void) const,                    void);
asDECLARE_METHOD_WRAPPERPR(__GetNumFrames,           ETHScriptEntity, GetNumFrames,           (void) const,                    unsigned int);
asDECLARE_METHOD_WRAPPERPR(__GetType,                ETHScriptEntity, GetType,                (void) const,                    ETHEntityProperties::ENTITY_TYPE);
asDECLARE_METHOD_WRAPPERPR(__ComputeParallaxOffset,  ETHScriptEntity, ComputeParallaxOffset,  (void) const,                    Vector2);

asDECLARE_METHOD_WRAPPERPR(__SetSprite,          ETHScriptEntity, SetSprite,           (const str_type::string&), bool);
asDECLARE_METHOD_WRAPPERPR(__SetNormal,          ETHScriptEntity, SetNormal,           (const str_type::string&), bool);
asDECLARE_METHOD_WRAPPERPR(__IsInvisible,        ETHScriptEntity, IsInvisible,         (void) const,              bool);
asDECLARE_METHOD_WRAPPERPR(__HasParticleSystem0, ETHScriptEntity, HasParticleSystem,   (void) const,              bool);
asDECLARE_METHOD_WRAPPERPR(__HasParticleSystem1, ETHScriptEntity, HasParticleSystem,   (unsigned int) const,      bool);
asDECLARE_METHOD_WRAPPERPR(__HasLightSource,     ETHScriptEntity, HasLightSource,      (void) const,              bool);
asDECLARE_METHOD_WRAPPERPR(__HasSoundEffect,     ETHScriptEntity, HasSoundEffect,      (void) const,              bool);
asDECLARE_METHOD_WRAPPERPR(__SetLightRange,      ETHScriptEntity, SetLightRange,       (const float),             void);
asDECLARE_METHOD_WRAPPERPR(__GetLightRange,      ETHScriptEntity, GetLightRange,       (void) const,              float);
asDECLARE_METHOD_WRAPPERPR(__SetLightColor,      ETHScriptEntity, SetLightColor,       (const Vector3&),          void);
asDECLARE_METHOD_WRAPPERPR(__GetLightColor,      ETHScriptEntity, GetLightColor,       (void) const,              Vector3);
asDECLARE_METHOD_WRAPPERPR(__SetEmissiveColor,   ETHScriptEntity, SetEmissiveColor,    (const Vector3&),          void);
asDECLARE_METHOD_WRAPPERPR(__GetEmissiveColor,   ETHScriptEntity, GetEmissiveColor,    (void) const,              Vector3);
asDECLARE_METHOD_WRAPPERPR(__DisableLightSource, ETHScriptEntity, DisableLightSource,  (void),                    void);

asDECLARE_METHOD_WRAPPERPR(__KillParticleSystem, ETHScriptEntity, KillParticleSystem, (const unsigned int),       void);
asDECLARE_METHOD_WRAPPERPR(__ParticlesKilled,    ETHScriptEntity, ParticlesKilled,    (const unsigned int) const, bool);
asDECLARE_METHOD_WRAPPERPR(__EraseData,          ETHScriptEntity, EraseData,          (const str_type::string&),  bool);
asDECLARE_METHOD_WRAPPERPR(__SetColor,           ETHScriptEntity, SetColor,           (const Vector3&),           void);
asDECLARE_METHOD_WRAPPERPR(__SetAlpha,           ETHScriptEntity, SetAlpha,           (const float),              void);
asDECLARE_METHOD_WRAPPERPR(__GetColor,           ETHScriptEntity, GetColor,           (void) const,               Vector3);
asDECLARE_METHOD_WRAPPERPR(__GetAlpha,           ETHScriptEntity, GetAlpha,           (void) const,               float);
asDECLARE_METHOD_WRAPPERPR(__IsAlive,            ETHScriptEntity, IsAlive,            (void) const,               bool);
asDECLARE_METHOD_WRAPPERPR(__IsHidden,           ETHScriptEntity, IsHidden,           (void) const,               bool);
asDECLARE_METHOD_WRAPPERPR(__Hide,               ETHScriptEntity, Hide,               (const bool),               void);
asDECLARE_METHOD_WRAPPERPR(__GetShadowZ,         ETHScriptEntity, GetShadowZ,         (void) const,               float);
asDECLARE_METHOD_WRAPPERPR(__SetShadowZ,         ETHScriptEntity, SetShadowZ,         (const float),              void);
asDECLARE_METHOD_WRAPPERPR(__AreParticlesOver,   ETHScriptEntity, AreParticlesOver,   (void) const,               bool);
asDECLARE_METHOD_WRAPPERPR(__HasShadow,          ETHScriptEntity, HasShadow,          (void) const,               bool);
asDECLARE_METHOD_WRAPPERPR(__HasHalo,            ETHScriptEntity, HasHalo,            (void) const,               bool);
asDECLARE_METHOD_WRAPPERPR(__SetPivotAdjust,     ETHScriptEntity, SetPivotAdjust,     (const Vector2&),           void);
asDECLARE_METHOD_WRAPPERPR(__GetPivotAdjust,     ETHScriptEntity, GetPivotAdjust,     (void) const,               Vector2);

asDECLARE_METHOD_WRAPPERPR(__SetLayerDepth,         ETHScriptEntity, SetLayerDepth,         (const float),                                    void);
asDECLARE_METHOD_WRAPPERPR(__GetLayerDepth,         ETHScriptEntity, GetLayerDepth,         (void) const,                                     float);
asDECLARE_METHOD_WRAPPERPR(__AddToAngle,            ETHScriptEntity, AddToAngle,            (const float),                                    void);
asDECLARE_METHOD_WRAPPERPR(__SetSpriteCut,          ETHScriptEntity, SetSpriteCut,          (const unsigned int col, const unsigned int row), bool);
asDECLARE_METHOD_WRAPPERPR(__GetSpriteCut,          ETHScriptEntity, GetSpriteCut,          (void) const,                                     Vector2);
asDECLARE_METHOD_WRAPPERPR(__SetParticlePosition,   ETHScriptEntity, SetParticlePosition,   (const unsigned int n, const Vector3&),           void);
asDECLARE_METHOD_WRAPPERPR(__ScaleParticleSystem,   ETHScriptEntity, ScaleParticleSystem,   (const unsigned int n, const float),              void);
asDECLARE_METHOD_WRAPPERPR(__MirrorParticleSystemX, ETHScriptEntity, MirrorParticleSystemX, (const unsigned int n, const bool),               bool);
asDECLARE_METHOD_WRAPPERPR(__MirrorParticleSystemY, ETHScriptEntity, MirrorParticleSystemY, (const unsigned int n, const bool),               bool);

asDECLARE_METHOD_WRAPPERPR(__SetFloat,   ETHScriptEntity, SetFloat,   (const str_type::string&, const float&),            bool);
asDECLARE_METHOD_WRAPPERPR(__SetInt,     ETHScriptEntity, SetInt,     (const str_type::string&, const int&),              bool);
asDECLARE_METHOD_WRAPPERPR(__SetUInt,    ETHScriptEntity, SetUInt,    (const str_type::string&, const unsigned int&),     bool);
asDECLARE_METHOD_WRAPPERPR(__SetString,  ETHScriptEntity, SetString,  (const str_type::string&, const str_type::string&), bool);
asDECLARE_METHOD_WRAPPERPR(__SetVector2, ETHScriptEntity, SetVector2, (const str_type::string&, const Vector2&),          bool);
asDECLARE_METHOD_WRAPPERPR(__SetVector3, ETHScriptEntity, SetVector3, (const str_type::string&, const Vector3&),          bool);
asDECLARE_METHOD_WRAPPERPR(__GetFloat,   ETHScriptEntity, GetFloat,   (const str_type::string&) const,                    float);
asDECLARE_METHOD_WRAPPERPR(__GetInt,     ETHScriptEntity, GetInt,     (const str_type::string&) const,                    int);
asDECLARE_METHOD_WRAPPERPR(__GetUInt,    ETHScriptEntity, GetUInt,    (const str_type::string&) const,                    unsigned int);
asDECLARE_METHOD_WRAPPERPR(__GetString,  ETHScriptEntity, GetString,  (const str_type::string&) const,                    str_type::string);
asDECLARE_METHOD_WRAPPERPR(__GetVector2, ETHScriptEntity, GetVector2, (const str_type::string&) const,                    Vector2);
asDECLARE_METHOD_WRAPPERPR(__GetVector3, ETHScriptEntity, GetVector3, (const str_type::string&) const,                    Vector3);

void __SetObject(asIScriptGeneric *gen)
{
	ETHScriptEntity *entity = (ETHScriptEntity*)gen->GetObject();
	str_type::string *key = *(str_type::string**)gen->GetAddressOfArg(0);
	void *ref = *(void**)gen->GetAddressOfArg(1);
	int typeId = gen->GetArgTypeId(1);
	entity->SetAngelScriptObject(*key, ref, typeId);
}

void __GetObject(asIScriptGeneric *gen)
{
	ETHScriptEntity *entity = (ETHScriptEntity*)gen->GetObject();
	str_type::string *key = *(str_type::string**)gen->GetAddressOfArg(0);
	void *ref = *(void**)gen->GetAddressOfArg(1);
	int typeId = gen->GetArgTypeId(1);
	*(bool*)gen->GetAddressOfReturnLocation() = entity->GetAngelScriptObject(*key, ref, typeId);
}

asDECLARE_METHOD_WRAPPERPR(__AddToFloat,      ETHScriptEntity, AddToFloat,      (const str_type::string&, const float&),        void);
asDECLARE_METHOD_WRAPPERPR(__AddToInt,        ETHScriptEntity, AddToInt,        (const str_type::string&, const int&),          void);
asDECLARE_METHOD_WRAPPERPR(__AddToUInt,       ETHScriptEntity, AddToUInt,       (const str_type::string&, const unsigned int&), void);
asDECLARE_METHOD_WRAPPERPR(__AddToVector2,    ETHScriptEntity, AddToVector2,    (const str_type::string&, const Vector2&),      void);
asDECLARE_METHOD_WRAPPERPR(__AddToVector3,    ETHScriptEntity, AddToVector3,    (const str_type::string&, const Vector3&),      void);
asDECLARE_METHOD_WRAPPERPR(__MultiplyFloat,   ETHScriptEntity, MultiplyFloat,   (const str_type::string&, const float&),        void);
asDECLARE_METHOD_WRAPPERPR(__MultiplyInt,     ETHScriptEntity, MultiplyInt,     (const str_type::string&, const int&),          void);
asDECLARE_METHOD_WRAPPERPR(__MultiplyUInt,    ETHScriptEntity, MultiplyUInt,    (const str_type::string&, const unsigned int&), void);
asDECLARE_METHOD_WRAPPERPR(__MultiplyVector2, ETHScriptEntity, MultiplyVector2, (const str_type::string&, const float&),        void);
asDECLARE_METHOD_WRAPPERPR(__MultiplyVector3, ETHScriptEntity, MultiplyVector3, (const str_type::string&, const float&),        void);

asDECLARE_METHOD_WRAPPERPR(__SetScale, ETHScriptEntity, SetScale, (const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(__ScaleV2,  ETHScriptEntity, Scale,    (const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(__ScaleF,   ETHScriptEntity, Scale,    (const float),    void);
asDECLARE_METHOD_WRAPPERPR(__GetScale, ETHScriptEntity, GetScale, (void) const,     Vector2);

asDECLARE_METHOD_WRAPPERPR(__GetPhysicsController, ETHScriptEntity, GetPhysicsController, (void), ETHPhysicsController*);

void RegisterEntityMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHEntity", asBEHAVE_ADDREF,  "void f()", asFUNCTION(__AddRefEntity),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHEntity", asBEHAVE_RELEASE, "void f()", asFUNCTION(__ReleaseEntity), asCALL_GENERIC); assert(r >= 0);

	// register ETHEntity methods
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetPosition() const",           asFUNCTION(__GetPosition),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetPositionXY() const",         asFUNCTION(__GetPositionXY), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetPositionX() const",            asFUNCTION(__GetPositionX),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetPositionY() const",            asFUNCTION(__GetPositionY),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetPositionZ() const",            asFUNCTION(__GetPositionZ),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetSize() const",               asFUNCTION(__GetSize),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "uint GetFrame() const",                 asFUNCTION(__GetFrame),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetFrame(const uint)",             asFUNCTION(__SetFrameN),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetFrame(const uint, const uint)", asFUNCTION(__SetFrameXY),    asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "int GetID() const",                asFUNCTION(__GetID),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetAngle() const",           asFUNCTION(__GetAngleEnt),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetAngle(const float)",       asFUNCTION(__SetAngle),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsStatic() const",            asFUNCTION(__IsStatic),         asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SilenceParticleSystems(const bool)",           asFUNCTION(__SilenceParticleSystems), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetSoundVolume(const float)",                  asFUNCTION(__SetSoundVolume),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "collisionBox GetCollisionBox() const",              asFUNCTION(__GetCollisionBox),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "string GetEntityName() const",                      asFUNCTION(__GetEntityName),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool Collidable() const",                           asFUNCTION(__Collidable),             asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "DATA_TYPE CheckCustomData(const string &in) const", asFUNCTION(__CheckCustomData),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasCustomData() const",                        asFUNCTION(__HasCustomData),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void DebugPrintCustomData() const",                 asFUNCTION(__DebugPrintCustomData),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "uint GetNumFrames() const",                         asFUNCTION(__GetNumFrames),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "ENTITY_TYPE GetType() const",                       asFUNCTION(__GetType),                asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 ComputeParallaxOffset() const",             asFUNCTION(__ComputeParallaxOffset),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetSprite(const string &in)",         asFUNCTION(__SetSprite),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetNormal(const string &in)",         asFUNCTION(__SetNormal),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsInvisible() const",                 asFUNCTION(__IsInvisible),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasParticleSystem() const",           asFUNCTION(__HasParticleSystem0), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasParticleSystem(uint) const",       asFUNCTION(__HasParticleSystem1), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasLightSource() const",              asFUNCTION(__HasLightSource),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasSoundEffect() const",              asFUNCTION(__HasSoundEffect),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetLightRange(const float)",          asFUNCTION(__SetLightRange),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetLightRange() const",              asFUNCTION(__GetLightRange),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetLightColor(const vector3 &in)",    asFUNCTION(__SetLightColor),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetLightColor() const",            asFUNCTION(__GetLightColor),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetEmissiveColor(const vector3 &in)", asFUNCTION(__SetEmissiveColor),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetEmissiveColor() const",         asFUNCTION(__GetEmissiveColor),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void DisableLightSource()",                asFUNCTION(__DisableLightSource), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void Scale(const float)",          asFUNCTION(__ScaleF),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void Scale(const vector2& in)",    asFUNCTION(__ScaleV2),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetScale(const vector2& in)", asFUNCTION(__SetScale), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetScale() const",         asFUNCTION(__GetScale), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void KillParticleSystem(const uint)",    asFUNCTION(__KillParticleSystem), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool ParticlesKilled(const uint) const", asFUNCTION(__ParticlesKilled),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool EraseData(const string &in)",       asFUNCTION(__EraseData),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetColor(const vector3 &in)",       asFUNCTION(__SetColor),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetAlpha(const float)",             asFUNCTION(__SetAlpha),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetColor()",                     asFUNCTION(__GetColor),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetAlpha()",                       asFUNCTION(__GetAlpha),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsAlive() const",                   asFUNCTION(__IsAlive),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsHidden() const",                  asFUNCTION(__IsHidden),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void Hide(const bool)",                  asFUNCTION(__Hide),               asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetShadowZ() const",               asFUNCTION(__GetShadowZ),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetShadowZ(const float)",           asFUNCTION(__SetShadowZ),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool AreParticlesOver() const",          asFUNCTION(__AreParticlesOver),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasShadow() const",                 asFUNCTION(__HasShadow),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasHalo() const",                   asFUNCTION(__HasHalo),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPivotAdjust(const vector2& in)", asFUNCTION(__SetPivotAdjust),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetPivotAdjust() const",         asFUNCTION(__GetPivotAdjust),     asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetLayerDepth(const float)",                           asFUNCTION(__SetLayerDepth),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetLayerDepth() const",                               asFUNCTION(__GetLayerDepth),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToAngle(const float)",                              asFUNCTION(__AddToAngle),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetSpriteCut(const uint col, const uint row)",         asFUNCTION(__SetSpriteCut),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetSpriteCut() const",                              asFUNCTION(__GetSpriteCut),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetParticlePosition(const uint n, const vector3 &in)", asFUNCTION(__SetParticlePosition),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void ScaleParticleSystem(const uint n, const float)",       asFUNCTION(__ScaleParticleSystem),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool MirrorParticleSystemX(const uint n, const bool)",      asFUNCTION(__MirrorParticleSystemX), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool MirrorParticleSystemY(const uint n, const bool)",      asFUNCTION(__MirrorParticleSystemY), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetFloat(const string &in, const float &in)",     asFUNCTION(__SetFloat),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetInt(const string &in, const int &in)",         asFUNCTION(__SetInt),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetUInt(const string &in, const uint &in)",       asFUNCTION(__SetUInt),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetString(const string &in, const string &in)",   asFUNCTION(__SetString),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetVector2(const string &in, const vector2 &in)", asFUNCTION(__SetVector2), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetVector3(const string &in, const vector3 &in)", asFUNCTION(__SetVector3), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetFloat(const string &in) const",               asFUNCTION(__GetFloat),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "int GetInt(const string &in) const",                   asFUNCTION(__GetInt),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "uint GetUInt(const string &in) const",                 asFUNCTION(__GetUInt),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "string GetString(const string &in) const",             asFUNCTION(__GetString),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetVector2(const string &in) const",           asFUNCTION(__GetVector2), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetVector3(const string &in) const",           asFUNCTION(__GetVector3), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetObject(const string &in, ?&in)",               asFUNCTION(__SetObject),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool GetObject(const string &in, ?&out)",              asFUNCTION(__GetObject),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToFloat(const string &in, const float &in)",      asFUNCTION(__AddToFloat),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToInt(const string &in, const int &in)",          asFUNCTION(__AddToInt),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToUInt(const string &in, const uint &in)",        asFUNCTION(__AddToUInt),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToVector2(const string &in, const vector2 &in)",  asFUNCTION(__AddToVector2),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToVector3(const string &in, const vector3 &in)",  asFUNCTION(__AddToVector3),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyFloat(const string &in, const float &in)",   asFUNCTION(__MultiplyFloat),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyInt(const string &in, const int &in)",       asFUNCTION(__MultiplyInt),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyUInt(const string &in, const uint &in)",     asFUNCTION(__MultiplyUInt),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyVector2(const string &in, const float &in)", asFUNCTION(__MultiplyVector2), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyVector3(const string &in, const float &in)", asFUNCTION(__MultiplyVector3), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "ETHPhysicsController@ GetPhysicsController()", asFUNCTION(__GetPhysicsController), asCALL_GENERIC); assert(r >= 0);
}

asDECLARE_FUNCTION_OBJ_WRAPPER(__CollisionBoxDefaultConstructor, CollisionBoxDefaultConstructor, false);
asDECLARE_FUNCTION_OBJ_WRAPPER(__CollisionBoxCopyConstructor,    CollisionBoxCopyConstructor,    false);
asDECLARE_FUNCTION_OBJ_WRAPPER(__CollisionBoxInitConstructor,    CollisionBoxInitConstructor,    false);

void RegisterCollisionMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("collisionBox", asBEHAVE_CONSTRUCT,  "void f()",                                     asFUNCTION(__CollisionBoxDefaultConstructor), asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("collisionBox", asBEHAVE_CONSTRUCT,  "void f(const collisionBox &in)",               asFUNCTION(__CollisionBoxCopyConstructor),    asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("collisionBox", asBEHAVE_CONSTRUCT,  "void f(const vector3 &in, const vector3 &in)", asFUNCTION(__CollisionBoxInitConstructor),    asCALL_GENERIC); assert( r >= 0 );
}

asDECLARE_FUNCTION_OBJ_WRAPPER(__Matrix4x4DefaultConstructor, Matrix4x4DefaultConstructor, false);
asDECLARE_FUNCTION_OBJ_WRAPPER(__Matrix4x4CopyConstructor,    Matrix4x4CopyConstructor,    false);

asDECLARE_FUNCTION_WRAPPER(__scale,      Scale);
asDECLARE_FUNCTION_WRAPPER(__translate,  Translate);
asDECLARE_FUNCTION_WRAPPER(__rotateX,    RotateX);
asDECLARE_FUNCTION_WRAPPER(__rotateY,    RotateY);
asDECLARE_FUNCTION_WRAPPER(__rotateZ,    RotateZ);
asDECLARE_FUNCTION_WRAPPERPR(__multiply, Multiply, (const Matrix4x4&, const Matrix4x4&), Matrix4x4);

static void __Matrix4x4Getter(asIScriptGeneric *gen)
{
	Matrix4x4* s = (Matrix4x4*)gen->GetObject();
	const unsigned int i = gen->GetArgDWord(0);
	const unsigned int j = gen->GetArgDWord(1);
	const float r = Matrix4x4Getter(i, j, s);
	gen->SetReturnFloat(r);
}

asDECLARE_FUNCTION_OBJ_WRAPPERPR(__Matrix4x4Setter, Matrix4x4Setter, false, (const unsigned int i, const unsigned int j, const float value, Matrix4x4 *p), void);

asDECLARE_FUNCTION_WRAPPER(__GetAngle, GetAngle);

void RegisterMathFunctionsAndMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("matrix4x4", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(__Matrix4x4DefaultConstructor), asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("matrix4x4", asBEHAVE_CONSTRUCT,  "void f(const matrix4x4 &in)", asFUNCTION(__Matrix4x4CopyConstructor),    asCALL_GENERIC); assert( r >= 0 );

	// matrix functions
	r = pASEngine->RegisterGlobalFunction("matrix4x4 scale(const float, const float, const float)",       asFUNCTION(__scale),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 translate(const float, const float, const float)",   asFUNCTION(__translate), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 rotateX(const float)",                               asFUNCTION(__rotateX),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 rotateY(const float)",                               asFUNCTION(__rotateY),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 rotateZ(const float)",                               asFUNCTION(__rotateZ),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 multiply(const matrix4x4 &in, const matrix4x4 &in)", asFUNCTION(__multiply),  asCALL_GENERIC); assert(r >= 0);

	// methods from globals
	r = pASEngine->RegisterObjectMethod("matrix4x4", "float get(const uint i, const uint j) const",       asFUNCTION(__Matrix4x4Getter), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("matrix4x4", "void set(const uint i, const uint j, const float)", asFUNCTION(__Matrix4x4Setter), asCALL_GENERIC); assert(r >= 0);

	// misc functions:
	r = pASEngine->RegisterGlobalFunction("float getAngle(const vector2 &in)", asFUNCTION(__GetAngle), asCALL_GENERIC); assert(r >= 0);
}

asDECLARE_FUNCTION_WRAPPER(__DateTimeFactory, DateTimeFactory);

asDECLARE_METHOD_WRAPPERPR(__addRefDateTime,  ETHDateTime, AddRef,  (void), void);
asDECLARE_METHOD_WRAPPERPR(__releaseDateTime, ETHDateTime, Release, (void), void);

asDECLARE_METHOD_WRAPPERPR(__update,     ETHDateTime, Update,     (void),       unsigned int);
asDECLARE_METHOD_WRAPPERPR(__getDay,     ETHDateTime, GetDay,     (void) const, unsigned int);
asDECLARE_METHOD_WRAPPERPR(__getMonth,   ETHDateTime, GetMonth,   (void) const, unsigned int);
asDECLARE_METHOD_WRAPPERPR(__getYear,    ETHDateTime, GetYear,    (void) const, unsigned int);
asDECLARE_METHOD_WRAPPERPR(__getHours,   ETHDateTime, GetHours,   (void) const, unsigned int);
asDECLARE_METHOD_WRAPPERPR(__getMinutes, ETHDateTime, GetMinutes, (void) const, unsigned int);
asDECLARE_METHOD_WRAPPERPR(__getSeconds, ETHDateTime, GetSeconds, (void) const, unsigned int);

void RegisterDateTimeMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("dateTime", asBEHAVE_FACTORY, "dateTime@ f()", asFUNCTION(__DateTimeFactory), asCALL_GENERIC); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("dateTime", asBEHAVE_ADDREF,  "void f()",      asFUNCTION(__addRefDateTime),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("dateTime", asBEHAVE_RELEASE, "void f()",      asFUNCTION(__releaseDateTime), asCALL_GENERIC); assert(r >= 0);
 
	r = pASEngine->RegisterObjectMethod("dateTime", "uint update()",           asFUNCTION(__update),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getDay() const",     asFUNCTION(__getDay),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getMonth() const",   asFUNCTION(__getMonth),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getYear() const",    asFUNCTION(__getYear),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getHours() const",   asFUNCTION(__getHours),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getMinutes() const", asFUNCTION(__getMinutes), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getSeconds() const", asFUNCTION(__getSeconds), asCALL_GENERIC); assert(r >= 0);
}

asDECLARE_METHOD_WRAPPERPR(__AddRefPC,  ETHPhysicsController, AddRef,  (void), void);
asDECLARE_METHOD_WRAPPERPR(__ReleasePC, ETHPhysicsController, Release, (void), void);

asDECLARE_METHOD_WRAPPERPR(__GetShape,            ETHPhysicsController, GetShape,            (void) const, ETHEntityProperties::BODY_SHAPE);
asDECLARE_METHOD_WRAPPERPR(__SetGravityScale,     ETHPhysicsController, SetGravityScale,     (const float), void);
asDECLARE_METHOD_WRAPPERPR(__GetGravityScale,     ETHPhysicsController, GetGravityScale,     (void) const, float);
asDECLARE_METHOD_WRAPPERPR(__SetAngularVelocity,  ETHPhysicsController, SetAngularVelocity,  (const float), void);
asDECLARE_METHOD_WRAPPERPR(__SetAwake,            ETHPhysicsController, SetAwake,            (const bool), void);
asDECLARE_METHOD_WRAPPERPR(__SetBullet,           ETHPhysicsController, SetBullet,           (const bool), void);
asDECLARE_METHOD_WRAPPERPR(__IsAwake,             ETHPhysicsController, IsAwake,             (void) const, bool);
asDECLARE_METHOD_WRAPPERPR(__IsBullet,            ETHPhysicsController, IsBullet,            (void) const, bool);
asDECLARE_METHOD_WRAPPERPR(__GetMass,             ETHPhysicsController, GetMass,             (void) const, float);
asDECLARE_METHOD_WRAPPERPR(__GetAngularVelocity,  ETHPhysicsController, GetAngularVelocity,  (void) const, float);
asDECLARE_METHOD_WRAPPERPR(__GetLinearVelocity,   ETHPhysicsController, GetLinearVelocity,   (void) const, Vector2);
asDECLARE_METHOD_WRAPPERPR(__ApplyTorque,         ETHPhysicsController, ApplyTorque,         (const float), void);
asDECLARE_METHOD_WRAPPERPR(__ApplyForce,          ETHPhysicsController, ApplyForce,          (const Vector2&, const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(__ApplyForceToCenter,  ETHPhysicsController, ApplyForceToCenter,  (const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(__SetLinearVelocity,   ETHPhysicsController, SetLinearVelocity,   (const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(__ApplyAngularImpulse, ETHPhysicsController, ApplyAngularImpulse, (const float), void);
asDECLARE_METHOD_WRAPPERPR(__ApplyLinearImpulse,  ETHPhysicsController, ApplyLinearImpulse,  (const Vector2&, const Vector2&), void);
asDECLARE_METHOD_WRAPPERPR(__IsSensor,            ETHPhysicsController, IsSensor,            (void) const, bool);
asDECLARE_METHOD_WRAPPERPR(__IsFixedRotation,     ETHPhysicsController, IsFixedRotation,     (void) const, bool);
asDECLARE_METHOD_WRAPPERPR(__SetFriction,         ETHPhysicsController, SetFriction,         (const float), void);
asDECLARE_METHOD_WRAPPERPR(__GetFriction,         ETHPhysicsController, GetFriction,         (void) const, float);
asDECLARE_METHOD_WRAPPERPR(__GetRestitution,      ETHPhysicsController, GetRestitution,      (void) const, float);
asDECLARE_METHOD_WRAPPERPR(__SetRestitution,      ETHPhysicsController, SetRestitution,      (const float), void);
asDECLARE_METHOD_WRAPPERPR(__GetRevoluteJoint,    ETHPhysicsController, GetRevoluteJoint,    (const unsigned int), b2RevoluteJoint*);

void RegisterPhysicsControllerMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHPhysicsController", asBEHAVE_ADDREF,  "void f()", asFUNCTION(__AddRefPC),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHPhysicsController", asBEHAVE_RELEASE, "void f()", asFUNCTION(__ReleasePC),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "uint update()", asFUNCTION(__update), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "BODY_SHAPE GetShape() const",                                   asFUNCTION(__GetShape),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetGravityScale(const float)",                             asFUNCTION(__SetGravityScale),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "float GetGravityScale() const",                                 asFUNCTION(__GetGravityScale),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetAngularVelocity(const float)",                          asFUNCTION(__SetAngularVelocity),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetAwake(const bool)",                                     asFUNCTION(__SetAwake),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetBullet(const bool)",                                    asFUNCTION(__SetBullet),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "bool IsAwake() const",                                          asFUNCTION(__IsAwake),             asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "bool IsBullet() const",                                         asFUNCTION(__IsBullet),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "float GetMass() const",                                         asFUNCTION(__GetMass),             asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "float GetAngularVelocity() const",                              asFUNCTION(__GetAngularVelocity),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "vector2 GetLinearVelocity() const",                             asFUNCTION(__GetLinearVelocity),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void ApplyTorque(const float)",                                 asFUNCTION(__ApplyTorque),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void ApplyForce(const vector2 &in, const vector2 &in)",         asFUNCTION(__ApplyForce),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void ApplyForceToCenter(const vector2 &in)",                    asFUNCTION(__ApplyForceToCenter),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetLinearVelocity(const vector2 &in)",                     asFUNCTION(__SetLinearVelocity),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void ApplyAngularImpulse(const float)",                         asFUNCTION(__ApplyAngularImpulse), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void ApplyLinearImpulse(const vector2 &in, const vector2 &in)", asFUNCTION(__ApplyLinearImpulse),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "bool IsSensor() const",                                         asFUNCTION(__IsSensor),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "bool IsFixedRotation() const",                                  asFUNCTION(__IsFixedRotation),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "float GetFriction() const",                                     asFUNCTION(__GetFriction),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetFriction(const float)",                                 asFUNCTION(__SetFriction),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "float GetRestitution() const",                                  asFUNCTION(__GetRestitution),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "void SetRestitution(const float)",                              asFUNCTION(__SetRestitution),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHPhysicsController", "ETHRevoluteJoint@ GetRevoluteJoint(const uint)",                asFUNCTION(__GetRevoluteJoint),    asCALL_GENERIC); assert(r >= 0);
}

//asDECLARE_METHOD_WRAPPERPR(__GetReactionForce,  b2RevoluteJoint, GetReactionForce,  (void), b2Vec2);
//asDECLARE_METHOD_WRAPPERPR(__GetReactionTorque, b2RevoluteJoint, GetReactionTorque, (void), float32);
asDECLARE_METHOD_WRAPPERPR(__GetJointAngle,     b2RevoluteJoint, GetJointAngle,     (void) const, float32);
asDECLARE_METHOD_WRAPPERPR(__GetJointSpeed,     b2RevoluteJoint, GetJointSpeed,     (void) const, float32);
//asDECLARE_METHOD_WRAPPERPR(__GetMotorTorque,    b2RevoluteJoint, GetMotorTorque,    (void) const, float32);
asDECLARE_METHOD_WRAPPERPR(__SetMotorSpeed,     b2RevoluteJoint, SetMotorSpeed,     (float32),    void);
asDECLARE_METHOD_WRAPPERPR(__GetMotorSpeed,     b2RevoluteJoint, GetMotorSpeed,     (void) const, float32);
asDECLARE_METHOD_WRAPPERPR(__SetMaxMotorTorque, b2RevoluteJoint, SetMaxMotorTorque, (float32),    void);

void RegisterRevoluteJointMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHRevoluteJoint", asBEHAVE_ADDREF,  "void f()", asFUNCTION(__DummyAddRef),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHRevoluteJoint", asBEHAVE_RELEASE, "void f()", asFUNCTION(__DummyRelease),   asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHRevoluteJoint", "float GetJointAngle() const",   asFUNCTION(__GetJointAngle),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHRevoluteJoint", "float GetJointSpeed() const",   asFUNCTION(__GetJointSpeed),     asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHRevoluteJoint", "void SetMotorSpeed(float)",     asFUNCTION(__SetMotorSpeed),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHRevoluteJoint", "float GetMotorSpeed() const",   asFUNCTION(__GetMotorSpeed),     asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHRevoluteJoint", "void SetMaxMotorTorque(float)",   asFUNCTION(__SetMaxMotorTorque), asCALL_GENERIC); assert(r >= 0);
}

} // namespace ETHGlobal