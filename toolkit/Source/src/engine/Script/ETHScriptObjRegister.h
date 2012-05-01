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

#ifndef ETH_SCRIPT_OBJECT_REGISTER_H_
#define ETH_SCRIPT_OBJECT_REGISTER_H_

#include "../Scene/ETHScene.h"
#include "../Util/ETHDateTime.h"
#include <ostream>
#include <string>
#include <enml/enml.h>

namespace ETHGlobal
{
	void DateTimeConstructor(ETHDateTime *self);
	ETHDateTime *DateTimeFactory();
	float Matrix4x4Getter(const unsigned int i, const unsigned int j, Matrix4x4 *p);
	void Matrix4x4Setter(const unsigned int i, const unsigned int j, const float value, Matrix4x4 *p);
	void Matrix4x4DefaultConstructor(Matrix4x4 *self);
	void Matrix4x4CopyConstructor(const Matrix4x4 &other, Matrix4x4 *self);
	void CollisionBoxDefaultConstructor(ETHCollisionBox *self);
	void CollisionBoxCopyConstructor(const ETHCollisionBox &other, ETHCollisionBox *self);
	void CollisionBoxInitConstructor(const Vector3 &pos, const Vector3 &size, ETHCollisionBox *self);
	void DummyAddRef();
	void DummyRelease();
	enml::File *EnmlFileFactory();
	enml::Entity *EnmlEntityFactory();
	ETHEntityArray *ETHEntityArrayFactory();
	void EnmlEntityCopyConstructor(const enml::Entity &other, enml::Entity *self);
	bool CheckASError(const int r, const str_type::string &description);
	bool RegisterInputObject(asIScriptEngine *pASEngine);
	bool RegisterENMLObject(asIScriptEngine *pASEngine);
	bool RegisterEntityObject(asIScriptEngine *pASEngine);
	bool RegisterCollisionObject(asIScriptEngine *pASEngine);
	bool RegisterMathObjectsAndFunctions(asIScriptEngine *pASEngine);
	bool RegisterEntityArrayObject(asIScriptEngine *pASEngine);
	void RegisterEnumTypes(asIScriptEngine *pASEngine);
	void RegisterGlobalProperties(asIScriptEngine *pASEngine);
	bool RegisterVideoModeObject(asIScriptEngine *pASEngine);
	bool RegisterDateTimeObject(asIScriptEngine *pASEngine);

	void RegisterAllObjects(asIScriptEngine *pASEngine);
}

#endif