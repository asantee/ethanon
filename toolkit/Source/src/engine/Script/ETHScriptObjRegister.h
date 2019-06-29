#ifndef ETH_SCRIPT_OBJECT_REGISTER_H_
#define ETH_SCRIPT_OBJECT_REGISTER_H_

#include "../Scene/ETHScene.h"
#include "../Util/ETHDateTime.h"
#include <ostream>
#include <string>
#include <Enml/Enml.h>

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
