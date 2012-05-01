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
#include "ETHInput.h"

namespace ETHGlobal {

void RegisterInputMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHInput", asBEHAVE_ADDREF, "void f()", asFUNCTION(AddRef),  asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHInput", asBEHAVE_RELEASE, "void f()", asFUNCTION(Release),  asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetLeftClickState()", asMETHOD(ETHInput,GetLeftClickState), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetRightClickState()", asMETHOD(ETHInput,GetRightClickState), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetMiddleClickState()", asMETHOD(ETHInput,GetMiddleClickState), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetMouseMove()", asMETHOD(ETHInput,GetMouseMove), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "float GetWheelState()", asMETHOD(ETHInput,GetWheelState), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetKeyState(const KEY)", asMETHOD(ETHInput,GetKeyState), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "bool KeyDown(const KEY)", asMETHOD(ETHInput,KeyDown), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE JoyButtonState(const uint, const J_KEY) const", asMETHOD(ETHInput,JoyButtonState), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "bool JoyButtonDown(const uint, const J_KEY) const", asMETHOD(ETHInput,JoyButtonDown), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "bool DetectJoysticks() const", asMETHOD(ETHInput,DetectJoysticks), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "J_STATUS GetJoystickStatus(const uint) const", asMETHOD(ETHInput,GetJoystickStatus), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "uint GetNumJoyButtons(const uint) const", asMETHOD(ETHInput,GetNumJoyButtons), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetJoystickXY(const uint) const", asMETHOD(ETHInput,GetJoystickXY), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "float GetJoystickZ(const uint) const", asMETHOD(ETHInput,GetJoystickZ), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "float GetJoystickRudder(const uint) const", asMETHOD(ETHInput,GetJoystickRudder), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetJoystickUV(const uint) const", asMETHOD(ETHInput,GetJoystickUV), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "J_KEY GetFirstButtonDown(const uint) const", asMETHOD(ETHInput,GetFirstButtonDown), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "uint GetMaxJoysticks() const", asMETHOD(ETHInput,GetMaxJoysticks), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetTouchPos(const uint) const",     asMETHOD(ETHInput,GetTouchPos),          asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "KEY_STATE GetTouchState(const uint) const", asMETHOD(ETHInput,GetTouchState),        asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "uint GetMaxTouchCount() const",             asMETHOD(ETHInput,GetMaxTouchCount),     asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetTouchMove(const uint) const",    asMETHOD(ETHInput,GetTouchMove),         asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector3 GetAccelerometerData() const",      asMETHOD(ETHInput,GetAccelerometerData), asCALL_THISCALL); assert(r >= 0);
}

void RegisterENMLMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_FACTORY, "enmlEntity@ f()", asFUNCTION(EnmlEntityFactory), asCALL_CDECL); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_ADDREF, "void f()", asMETHOD(enml::Entity,addRef), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_RELEASE, "void f()", asMETHOD(enml::Entity,release), asCALL_THISCALL); assert(r >= 0);
	//r = pASEngine->RegisterObjectBehaviour("enmlEntity", asBEHAVE_CONSTRUCT, "void f(const enmlEntity &in)", asFUNCTION(EnmlEntityCopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// methods
	r = pASEngine->RegisterObjectMethod("enmlEntity", "void clear()", asMETHOD(enml::Entity,clear), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlEntity", "void add(const string &in, string)", asMETHOD(enml::Entity,add), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlEntity", "string get(const string &in)", asMETHOD(enml::Entity,get), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlEntity", "string getAttributeNames() const", asMETHOD(enml::Entity,getAttributeNames), asCALL_THISCALL); assert(r >= 0);

	// register the ENML file object
	r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_FACTORY, "enmlFile@ f()", asFUNCTION(EnmlFileFactory), asCALL_CDECL); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_ADDREF, "void f()", asMETHOD(enml::File,addRef), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_RELEASE, "void f()", asMETHOD(enml::File,release), asCALL_THISCALL); assert(r >= 0);
	//r = pASEngine->RegisterObjectBehaviour("enmlFile", asBEHAVE_CONSTRUCT,  "void f(const string &in)", asFUNCTION(EnmlFileConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// methods
	r = pASEngine->RegisterObjectMethod("enmlFile", "void clear()", asMETHOD(enml::File,clear), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "void addEntity(const string &in, const enmlEntity &in)", asMETHOD(enml::File,addEntity), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string generateString() const", asMETHOD(enml::File,generateString), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "uint parseString(const string &in)", asMETHOD(enml::File,parseString), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool exists(const string &in) const", asMETHOD(enml::File,exists), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string get(const string &in, const string &in) const", asMETHOD(enml::File,get), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getDouble(const string &in, const string &in, double &out) const", asMETHOD(enml::File,getDouble), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getInt(const string &in, const string &in, int &out) const", asMETHOD(enml::File,getInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getUInt(const string &in, const string &in, uint &out) const", asMETHOD(enml::File,getUInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool getFloat(const string &in, const string &in, float &out) const", asMETHOD(enml::File,getFloat), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string getErrorString() const", asMETHOD(enml::File,getErrorString), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string getEntityNames() const", asMETHOD(enml::File,getEntityNames), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "string getAttributeNames(const string &in) const", asMETHOD(enml::File,getAttributeNames), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "void writeToFile(const string &in) const", asMETHOD(enml::File,writeToFile), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "bool parseFromFile(const string &in)", asMETHOD(enml::File,parseFromFile), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("enmlFile", "void addValue(const string& in, const string& in, const string& in)", asMETHOD(enml::File,addValue), asCALL_THISCALL); assert(r >= 0);

	// util globals
	r = pASEngine->RegisterGlobalFunction("string GetStringFromFile(const string &in)", asFUNCTION(enml::getStringFromAnsiFile), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SaveStringToFile(const string &in, const string &in)", asFUNCTION(enml::saveStringToAnsiFile), asCALL_CDECL); assert(r >= 0);
}

void RegisterEntityArrayMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHEntityArray", asBEHAVE_FACTORY, "ETHEntityArray@ f()", asFUNCTION(ETHEntityArrayFactory), asCALL_CDECL); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("ETHEntityArray", asBEHAVE_ADDREF, "void f()", asMETHOD(ETHEntityArray,AddRef), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHEntityArray", asBEHAVE_RELEASE, "void f()", asMETHOD(ETHEntityArray,Release), asCALL_THISCALL); assert(r >= 0);

	// methods and operators
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "ETHEntity@ &opIndex(uint)", asMETHODPR(ETHEntityArray, operator[], (const unsigned int), ETHEntityRawPtr &), asCALL_THISCALL); assert( r >= 0 );
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "const ETHEntity@ &opIndex(uint) const", asMETHODPR(ETHEntityArray, operator[], (const unsigned int), ETHEntityRawPtr &), asCALL_THISCALL); assert( r >= 0 );

	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "ETHEntityArray &opAddAssign(const ETHEntityArray &in)", asMETHOD(ETHEntityArray, operator+=), asCALL_THISCALL); assert( r >= 0 );
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void push_back(const ETHEntity &in)", asMETHOD(ETHEntityArray,push_back), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "uint size() const", asMETHOD(ETHEntityArray,size), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void clear()", asMETHOD(ETHEntityArray,clear), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void unique()", asMETHOD(ETHEntityArray,unique), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntityArray", "void removeDeadEntities()", asMETHOD(ETHEntityArray,removeDeadEntities), asCALL_THISCALL); assert(r >= 0);
}

void RegisterEntityMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("ETHEntity", asBEHAVE_ADDREF, "void f()", asMETHOD(ETHScriptEntity,AddRef), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("ETHEntity", asBEHAVE_RELEASE, "void f()", asMETHOD(ETHScriptEntity,Release), asCALL_THISCALL); assert(r >= 0);

	// register ETHEntity methods
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetPosition() const", asMETHOD(ETHScriptEntity,GetPosition), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetPositionXY() const", asMETHOD(ETHScriptEntity,GetPositionXY), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetSize() const", asMETHOD(ETHScriptEntity,GetCurrentSize), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "uint GetFrame() const", asMETHOD(ETHScriptEntity,GetFrame), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetFrame(const uint)", asMETHODPR(ETHScriptEntity,SetFrame,(const unsigned int),bool), asCALL_THISCALL);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetFrame(const uint, const uint)", asMETHODPR(ETHScriptEntity,SetFrame,(const unsigned int,const unsigned int),bool), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetScreenRectMin() const", asMETHOD(ETHScriptEntity,GetScreenRectMin), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetScreenRectMax() const", asMETHOD(ETHScriptEntity,GetScreenRectMax), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "int GetID() const", asMETHOD(ETHScriptEntity,GetID), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetAngle() const", asMETHOD(ETHScriptEntity,GetAngle), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetAngle(const float)", asMETHOD(ETHScriptEntity,SetAngle), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsStatic() const", asMETHOD(ETHScriptEntity,IsStatic), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void TurnDynamic()", asMETHOD(ETHScriptEntity,TurnDynamic), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SilenceParticleSystems(const bool)", asMETHOD(ETHScriptEntity,SilenceParticleSystems), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetSoundVolume(const float)", asMETHOD(ETHScriptEntity,SetSoundVolume), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "collisionBox GetCollisionBox() const", asMETHOD(ETHScriptEntity,GetCollisionBox), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "string GetEntityName() const", asMETHOD(ETHScriptEntity,GetEntityName), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool Collidable() const", asMETHOD(ETHScriptEntity, IsCollidable), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "DATA_TYPE CheckCustomData(const string &in) const", asMETHOD(ETHScriptEntity,CheckCustomData), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasCustomData() const", asMETHOD(ETHScriptEntity,HasCustomData), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void DebugPrintCustomData() const", asMETHOD(ETHScriptEntity,DebugPrintCustomData), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "uint GetNumFrames() const", asMETHOD(ETHScriptEntity,GetNumFrames), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "ENTITY_TYPE GetType() const", asMETHOD(ETHScriptEntity,GetType), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetSprite(const string &in)", asMETHODPR(ETHScriptEntity,SetSprite,(const str_type::string&), bool), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetNormal(const string &in)", asMETHODPR(ETHScriptEntity,SetNormal,(const str_type::string&), bool), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsInvisible() const", asMETHOD(ETHScriptEntity,IsInvisible), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasParticleSystem() const",     asMETHODPR(ETHScriptEntity,HasParticleSystems, (void) const, bool), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasParticleSystem(uint) const", asMETHODPR(ETHScriptEntity,HasParticleSystem, (const unsigned int) const, bool), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasLightSource() const", asMETHOD(ETHScriptEntity,HasLightSource), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasSoundEffect() const", asMETHOD(ETHScriptEntity,HasSoundEffect), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool PlayParticleSystem(const uint)", asMETHOD(ETHScriptEntity,PlayParticleSystem), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetLightRange(const float)", asMETHOD(ETHScriptEntity,SetLightRange), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetLightRange() const", asMETHOD(ETHScriptEntity,GetLightRange), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetLightColor(const vector3 &in)", asMETHOD(ETHScriptEntity,SetLightColor), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetLightColor() const", asMETHOD(ETHScriptEntity,GetLightColor), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetEmissiveColor(const vector3 &in)", asMETHOD(ETHScriptEntity,SetEmissiveColor), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetEmissiveColor() const", asMETHOD(ETHScriptEntity,GetEmissiveColor), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 ComputeParallaxOffset() const", asMETHOD(ETHScriptEntity,ComputeParallaxOffset), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void Scale(const float)",          asMETHODPR(ETHScriptEntity, Scale, (const float),    void), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void Scale(const vector2& in)",    asMETHODPR(ETHScriptEntity, Scale, (const Vector2&), void), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetScale(const vector2& in)", asMETHOD(ETHScriptEntity, SetScale),                        asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetScale() const",         asMETHOD(ETHScriptEntity, GetScale),                        asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void KillParticleSystem(const uint)", asMETHOD(ETHScriptEntity,KillParticleSystem), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool ParticlesKilled(const uint) const", asMETHOD(ETHScriptEntity,ParticlesKilled), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool EraseData(const string &in)", asMETHOD(ETHScriptEntity,EraseData), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetColor(const vector3 &in)", asMETHOD(ETHScriptEntity,SetColor), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetAlpha(const float)", asMETHOD(ETHScriptEntity,SetAlpha), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetColor()", asMETHOD(ETHScriptEntity,GetColor), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetAlpha()", asMETHOD(ETHScriptEntity,GetAlpha), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsAlive() const", asMETHOD(ETHScriptEntity,IsAlive), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool IsHidden() const", asMETHOD(ETHScriptEntity,IsHidden), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void Hide(const bool)", asMETHOD(ETHScriptEntity,Hide), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetShadowZ() const", asMETHOD(ETHScriptEntity,GetShadowZ), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetShadowZ(const float)", asMETHOD(ETHScriptEntity,SetShadowZ), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool AreParticlesOver() const", asMETHOD(ETHScriptEntity,AreParticlesOver), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasShadow() const", asMETHOD(ETHScriptEntity,HasShadow), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool HasHalo() const", asMETHOD(ETHScriptEntity,HasHalo), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetLayerDepth(const float)", asMETHOD(ETHScriptEntity,SetLayerDepth), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetLayerDepth() const", asMETHOD(ETHScriptEntity,GetLayerDepth), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToAngle(const float)", asMETHOD(ETHScriptEntity,AddToAngle), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetSpriteCut(const uint col, const uint row)", asMETHOD(ETHScriptEntity,SetSpriteCut), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetCollision(const bool)", asMETHOD(ETHScriptEntity,SetCollision), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetParticlePosition(const uint n, const vector3 &in)", asMETHOD(ETHScriptEntity,SetParticlePosition), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void ScaleParticleSystem(const uint n, const float)", asMETHOD(ETHScriptEntity,ScaleParticleSystem), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool MirrorParticleSystemX(const uint n, const bool)", asMETHOD(ETHScriptEntity,MirrorParticleSystemX), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool MirrorParticleSystemY(const uint n, const bool)", asMETHOD(ETHScriptEntity,MirrorParticleSystemY), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetFloat(const string &in, const float &in)", asMETHOD(ETHScriptEntity,SetFloat), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetInt(const string &in, const int &in)", asMETHOD(ETHScriptEntity,SetInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetUInt(const string &in, const uint &in)", asMETHOD(ETHScriptEntity,SetUInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetString(const string &in, const string &in)", asMETHOD(ETHScriptEntity,SetString), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetVector2(const string &in, const vector2 &in)", asMETHOD(ETHScriptEntity,SetVector2), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "bool SetVector3(const string &in, const vector3 &in)", asMETHOD(ETHScriptEntity,SetVector3), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "float GetFloat(const string &in) const", asMETHOD(ETHScriptEntity,GetFloat), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "int GetInt(const string &in) const", asMETHOD(ETHScriptEntity,GetInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "uint GetUInt(const string &in) const", asMETHOD(ETHScriptEntity,GetUInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "string GetString(const string &in) const", asMETHOD(ETHScriptEntity,GetString), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetVector2(const string &in) const", asMETHOD(ETHScriptEntity,GetVector2), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector3 GetVector3(const string &in) const", asMETHOD(ETHScriptEntity,GetVector3), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToFloat(const string &in, const float &in)", asMETHOD(ETHScriptEntity,AddToFloat), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToInt(const string &in, const int &in)", asMETHOD(ETHScriptEntity,AddToInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToUInt(const string &in, const uint &in)", asMETHOD(ETHScriptEntity,AddToUInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToVector2(const string &in, const vector2 &in)", asMETHOD(ETHScriptEntity,AddToVector2), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToVector3(const string &in, const vector3 &in)", asMETHOD(ETHScriptEntity,AddToVector3), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyFloat(const string &in, const float &in)", asMETHOD(ETHScriptEntity,MultiplyFloat), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyInt(const string &in, const int &in)", asMETHOD(ETHScriptEntity,MultiplyInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyUInt(const string &in, const uint &in)", asMETHOD(ETHScriptEntity,MultiplyUInt), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyVector2(const string &in, const float &in)", asMETHOD(ETHScriptEntity,MultiplyVector2), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void MultiplyVector3(const string &in, const float &in)", asMETHOD(ETHScriptEntity,MultiplyVector3), asCALL_THISCALL); assert(r >= 0);
}

void RegisterCollisionMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("collisionBox", asBEHAVE_CONSTRUCT,  "void f()",                                     asFUNCTION(CollisionBoxDefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("collisionBox", asBEHAVE_CONSTRUCT,  "void f(const collisionBox &in)",               asFUNCTION(CollisionBoxCopyConstructor),    asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("collisionBox", asBEHAVE_CONSTRUCT,  "void f(const vector3 &in, const vector3 &in)", asFUNCTION(CollisionBoxInitConstructor),    asCALL_CDECL_OBJLAST); assert( r >= 0 );
}

void RegisterMathFunctionsAndMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("matrix4x4", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(Matrix4x4DefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("matrix4x4", asBEHAVE_CONSTRUCT,  "void f(const matrix4x4 &in)", asFUNCTION(Matrix4x4CopyConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// matrix functions
	r = pASEngine->RegisterGlobalFunction("matrix4x4 scale(const float, const float, const float)", asFUNCTION(Scale), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 translate(const float, const float, const float)", asFUNCTION(Translate), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 rotateX(const float)", asFUNCTION(RotateX), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 rotateY(const float)", asFUNCTION(RotateY), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 rotateZ(const float)", asFUNCTION(RotateZ), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("matrix4x4 multiply(const matrix4x4 &in, const matrix4x4 &in)", asFUNCTIONPR(Multiply, (const Matrix4x4&, const Matrix4x4&), Matrix4x4), asCALL_CDECL); assert(r >= 0);

	// methods from globals
	r = pASEngine->RegisterObjectMethod("matrix4x4", "float get(const uint i, const uint j) const", asFUNCTION(Matrix4x4Getter), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("matrix4x4", "void set(const uint i, const uint j, const float)", asFUNCTION(Matrix4x4Setter), asCALL_CDECL_OBJLAST); assert(r >= 0);

	// misc functions:
	r = pASEngine->RegisterGlobalFunction("float getAngle(const vector2 &in)", asFUNCTION(GetAngle), asCALL_CDECL); assert(r >= 0);
}

void RegisterDateTimeMethods(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectBehaviour("dateTime", asBEHAVE_FACTORY, "dateTime@ f()", asFUNCTION(DateTimeFactory), asCALL_CDECL); assert( r >= 0 );
	r = pASEngine->RegisterObjectBehaviour("dateTime", asBEHAVE_ADDREF, "void f()", asMETHOD(ETHDateTime,AddRef), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectBehaviour("dateTime", asBEHAVE_RELEASE, "void f()", asMETHOD(ETHDateTime,Release), asCALL_THISCALL); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("dateTime", "uint update()", asMETHOD(ETHDateTime,Update), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getDay() const", asMETHOD(ETHDateTime,GetDay), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getMonth() const", asMETHOD(ETHDateTime,GetMonth), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getYear() const", asMETHOD(ETHDateTime,GetYear), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getHours() const", asMETHOD(ETHDateTime,GetHours), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getMinutes() const", asMETHOD(ETHDateTime,GetMinutes), asCALL_THISCALL); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("dateTime", "uint getSeconds() const", asMETHOD(ETHDateTime,GetSeconds), asCALL_THISCALL); assert(r >= 0);
}

} // namespace ETHGlobal