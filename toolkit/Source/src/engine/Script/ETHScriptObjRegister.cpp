#include "ETHScriptObjRegister.h"

#include "../Util/ETHInput.h"

#include <assert.h>

#include <Enml/Enml.h>

#include "ETHScriptWrapper.h"

#include "../../addons/scriptmath3d.h"
#include "../../addons/scriptmath2d.h"
#include "../../addons/scriptarray.h"
#include "../../addons/scriptmath.h"
#include "../../addons/scriptdictionary.h"
#include "../../addons/scriptany.h"

#include "../../addons/scriptfile.h"
#include "../../addons/scriptstdstring.h"

#include "../../engine/Util/JSONObject.h"

namespace ETHGlobal {

void Matrix4x4DefaultConstructor(Matrix4x4 *self)
{
	new(self) Matrix4x4();
}

void Matrix4x4CopyConstructor(const Matrix4x4 &other, Matrix4x4 *self)
{
	new(self) Matrix4x4(other);
}

float Matrix4x4Getter(const unsigned int i, const unsigned int j, Matrix4x4 *p)
{
	assert(i < 4);
	assert(j < 4);
	return p->m[i][j];
}

void Matrix4x4Setter(const unsigned int i, const unsigned int j, const float value, Matrix4x4 *p)
{
	assert(i < 4);
	assert(j < 4);
	p->m[i][j] = value;
}

void CollisionBoxDefaultConstructor(ETHCollisionBox *self)
{
	new(self) ETHCollisionBox();
}

void CollisionBoxCopyConstructor(const ETHCollisionBox &other, ETHCollisionBox *self)
{
	new(self) ETHCollisionBox(other);
}

void CollisionBoxInitConstructor(const Vector3 &pos, const Vector3 &size, ETHCollisionBox *self)
{
	new(self) ETHCollisionBox(pos,size);
}

void EnmlEntityCopyConstructor(const enml::Entity &other, enml::Entity *self)
{
	new(self) enml::Entity(other);
}

void EnmlFileConstructor(const std::string &str, enml::File *self)
{
	new(self) enml::File(str);
}

void DateTimeConstructor(ETHDateTime *self)
{
	new(self) ETHDateTime();
}

ETHDateTime *DateTimeFactory()
{
	return new ETHDateTime();
}

float GetAngle(const Vector2 &v2)
{
	const float r = atan2f(v2.x, v2.y);
	return (r < 0) ? r + (2 * gs2d::math::constant::PI) : r;
}

void DummyAddRef()
{
	// dummy...
}

void DummyRelease()
{
	// dummy...
}

ETHEntityArray *ETHEntityArrayFactory()
{
	return new ETHEntityArray();
}

enml::File *EnmlFileFactory()
{
	return new enml::File();
}

enml::Entity *EnmlEntityFactory()
{
	return new enml::Entity();
}

Matrix4x4 Scale(const float x, const float y, const float z)
{
	return Matrix4x4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	);
}

Matrix4x4 Translate(const float x, const float y, const float z)
{
	return Matrix4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	);
}

Matrix4x4 RotateX(const float theta)
{
	const float fSin=sinf(theta), fCos=cosf(theta);
	return Matrix4x4(
		1, 0,    0,    0,
		0, fCos, fSin, 0,
		0,-fSin, fCos, 0,
		0, 0,    0,    1
	);
}

Matrix4x4 RotateY(const float theta)
{
	const float fSin=sinf(theta), fCos=cosf(theta);
	return Matrix4x4(
		fCos, 0,-fSin, 0,
		0,    1,    0, 0,
		fSin, 0, fCos, 0,
		0,    0,    0, 1
	);
}

Matrix4x4 RotateZ(const float theta)
{
	const float fSin=sinf(theta), fCos=cosf(theta);
	return Matrix4x4(
		fCos, fSin, 0, 0,
		-fSin,fCos, 0, 0,
		0,    0,    1, 0,
		0,    0,    0, 1
	);
}

Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
	Matrix4x4 r;
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			r.m[i][j] = 0;
			for (int k = 0; k < 4; k++)
			{
				r.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return r;
}

void RegisterInputMethods(asIScriptEngine *pASEngine);
bool RegisterInputObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("ETHInput", 0, asOBJ_REF); assert(r >= 0);
	RegisterInputMethods(pASEngine);
	return true;
}

void RegisterENMLMethods(asIScriptEngine *pASEngine);
bool RegisterENMLObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("enmlEntity", 0, asOBJ_REF); assert(r >= 0);
	r = pASEngine->RegisterObjectType("enmlFile", 0, asOBJ_REF); assert(r >= 0);
	RegisterENMLMethods(pASEngine);
	return true;
}

void RegisterJSONObjectMethods(asIScriptEngine *pASEngine);
bool RegisterJSONObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("JSONObject", sizeof(JSONObject), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert(r >= 0);
	RegisterJSONObjectMethods(pASEngine);
	return true;
}

void RegisterEntityArrayMethods(asIScriptEngine *pASEngine);
bool RegisterEntityArrayObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("ETHEntityArray", 0, asOBJ_REF); assert(r >= 0);
	RegisterEntityArrayMethods(pASEngine);
	return true;
}

void RegisterEntityMethods(asIScriptEngine *pASEngine);
bool RegisterEntityObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("ETHEntity", 0, asOBJ_REF); assert(r >= 0);
	RegisterEntityMethods(pASEngine);
	return true;
}

struct ETH_COLLISION_BOX_POD
{
	float posx, posy, posz, sizex, sizey, sizez;
};

void RegisterCollisionMethods(asIScriptEngine *pASEngine);
bool RegisterCollisionObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("collisionBox", sizeof(ETH_COLLISION_BOX_POD), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("collisionBox", "vector3 pos",  offsetof(ETH_COLLISION_BOX_POD, posx)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("collisionBox", "vector3 size", offsetof(ETH_COLLISION_BOX_POD, sizex)); assert(r >= 0);
	RegisterCollisionMethods(pASEngine);
	return true;
}

void RegisterDateTimeMethods(asIScriptEngine *pASEngine);
bool RegisterDateTimeObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("dateTime", 0, asOBJ_REF); assert(r >= 0);
	RegisterDateTimeMethods(pASEngine);
	return true;
}

void RegisterPhysicsControllerMethods(asIScriptEngine *pASEngine);
bool RegisterPhysicsControllerObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("ETHPhysicsController", 0, asOBJ_REF); assert(r >= 0);
	RegisterPhysicsControllerMethods(pASEngine);
	return true;
}

void RegisterRevoluteJointMethods(asIScriptEngine *pASEngine);
bool RegisterRevoluteJointObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("ETHRevoluteJoint", 0, asOBJ_REF); assert(r >= 0);
	RegisterRevoluteJointMethods(pASEngine);
	return true;
}

void RegisterMathFunctionsAndMethods(asIScriptEngine *pASEngine);
bool RegisterMathObjectsAndFunctions(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("matrix4x4", sizeof(Matrix4x4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert(r >= 0);

	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a11", offsetof(PODMatrix4x4, mat.a11)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a12", offsetof(PODMatrix4x4, mat.a12)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a13", offsetof(PODMatrix4x4, mat.a13)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a14", offsetof(PODMatrix4x4, mat.a14)); assert(r >= 0);

	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a21", offsetof(PODMatrix4x4, mat.a21)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a22", offsetof(PODMatrix4x4, mat.a22)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a23", offsetof(PODMatrix4x4, mat.a23)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a24", offsetof(PODMatrix4x4, mat.a24)); assert(r >= 0);

	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a31", offsetof(PODMatrix4x4, mat.a31)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a32", offsetof(PODMatrix4x4, mat.a32)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a33", offsetof(PODMatrix4x4, mat.a33)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a34", offsetof(PODMatrix4x4, mat.a34)); assert(r >= 0);

	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a41", offsetof(PODMatrix4x4, mat.a41)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a42", offsetof(PODMatrix4x4, mat.a42)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a43", offsetof(PODMatrix4x4, mat.a43)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("matrix4x4", "float a44", offsetof(PODMatrix4x4, mat.a44)); assert(r >= 0);
	RegisterMathFunctionsAndMethods(pASEngine);
	return true;
}

void RegisterEnumTypes(asIScriptEngine *pASEngine)
{
	int r;
	// Shape types
	r = pASEngine->RegisterEnum("BODY_SHAPE"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("BODY_SHAPE", "BS_NONE",    ETHEntityProperties::BS_NONE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("BODY_SHAPE", "BS_BOX",     ETHEntityProperties::BS_BOX); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("BODY_SHAPE", "BS_CIRCLE",  ETHEntityProperties::BS_CIRCLE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("BODY_SHAPE", "BS_POLYGON", ETHEntityProperties::BS_POLYGON); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("BODY_SHAPE", "BS_COMPOUND", ETHEntityProperties::BS_COMPOUND); assert(r >= 0);

	// Entity type
	r = pASEngine->RegisterEnum("ENTITY_TYPE"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("ENTITY_TYPE", "ET_HORIZONTAL", ETHEntityProperties::ET_HORIZONTAL); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("ENTITY_TYPE", "ET_LAYERABLE", ETHEntityProperties::ET_LAYERABLE); assert(r >= 0);

	// Custom data type
	r = pASEngine->RegisterEnum("DATA_TYPE"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_NODATA", ETHCustomData::DT_NODATA); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_INT", ETHCustomData::DT_INT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_UINT", ETHCustomData::DT_UINT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_FLOAT", ETHCustomData::DT_FLOAT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_STRING", ETHCustomData::DT_STRING); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_VECTOR2", ETHCustomData::DT_VECTOR2); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("DATA_TYPE", "DT_VECTOR3", ETHCustomData::DT_VECTOR3); assert(r >= 0);

	// Pixel format
	r = pASEngine->RegisterEnum("PIXEL_FORMAT"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("PIXEL_FORMAT", "PF32BIT", Texture::PF_32BIT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("PIXEL_FORMAT", "PF24BIT", Texture::PF_24BIT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("PIXEL_FORMAT", "PF16BIT", Texture::PF_16BIT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("PIXEL_FORMAT", "PFUNKNOWN", Texture::PF_UNKNOWN); assert(r >= 0);

	// GS_KEY_STATE
	r = pASEngine->RegisterEnum("KEY_STATE"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY_STATE", "KS_HIT", GSKS_HIT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY_STATE", "KS_UP", GSKS_UP); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY_STATE", "KS_DOWN", GSKS_DOWN); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY_STATE", "KS_RELEASE", GSKS_RELEASE); assert(r >= 0);

	// Joystick status
	r = pASEngine->RegisterEnum("J_STATUS"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_STATUS", "JS_DETECTED", GSJS_DETECTED); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_STATUS", "JS_NOTDETECTED", GSJS_NOTDETECTED); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_STATUS", "JS_INVALID", GSJS_INVALID); assert(r >= 0);

	// Joystick keys
	r = pASEngine->RegisterEnum("J_KEY"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_01", GSB_01); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_02", GSB_02); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_03", GSB_03); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_04", GSB_04); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_05", GSB_05); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_06", GSB_06); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_07", GSB_07); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_08", GSB_08); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_09", GSB_09); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_10", GSB_10); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_11", GSB_11); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_12", GSB_12); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_13", GSB_13); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_14", GSB_14); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_15", GSB_15); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_16", GSB_16); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_17", GSB_17); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_18", GSB_18); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_19", GSB_19); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_20", GSB_20); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_21", GSB_21); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_22", GSB_22); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_23", GSB_23); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_24", GSB_24); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_25", GSB_25); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_26", GSB_26); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_27", GSB_27); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_28", GSB_28); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_29", GSB_29); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_30", GSB_30); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_31", GSB_31); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_32", GSB_32); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_UP", GSB_UP); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_DOWN", GSB_DOWN); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_LEFT", GSB_LEFT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_RIGHT", GSB_RIGHT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("J_KEY", "JK_NONE", GSB_NONE); assert(r >= 0);

	// GS_KEY
	r = pASEngine->RegisterEnum("KEY"); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_UP", GSK_UP); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_DOWN", GSK_DOWN); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_LEFT", GSK_LEFT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_RIGHT", GSK_RIGHT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_PAGEDOWN", GSK_PAGEDOWN); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_PAGEUP", GSK_PAGEUP); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_SPACE", GSK_SPACE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_ENTER", GSK_ENTER); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_RETURN", GSK_ENTER); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_DELETE", GSK_DELETE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_HOME", GSK_HOME); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_END", GSK_END); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_INSERT", GSK_INSERT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_PAUSE", GSK_PAUSE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_ESC", GSK_ESC); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_BACK", GSK_BACK); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_BACKSPACE", GSK_BACK); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_TAB", GSK_TAB); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_PRINTSCREEN", GSK_PRINTSCREEN); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_SUBTRACT", GSK_SUBTRACT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_ADD", GSK_ADD); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F1", GSK_F1); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F2", GSK_F2); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F3", GSK_F3); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F4", GSK_F4); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F5", GSK_F5); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F6", GSK_F6); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F7", GSK_F7); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F8", GSK_F8); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F9", GSK_F9); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F10", GSK_F10); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F11", GSK_F11); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F12", GSK_F12); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F13", GSK_F13); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F14", GSK_F14); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F15", GSK_F15); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F16", GSK_F16); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F17", GSK_F17); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F18", GSK_F18); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F19", GSK_F19); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F20", GSK_F20); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F21", GSK_F21); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F22", GSK_F22); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F23", GSK_F23); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F24", GSK_F24); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_A", GSK_A); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_B", GSK_B); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_C", GSK_C); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_D", GSK_D); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_E", GSK_E); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_F", GSK_F); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_G", GSK_G); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_H", GSK_H); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_I", GSK_I); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_J", GSK_J); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_K", GSK_K); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_L", GSK_L); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_M", GSK_M); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_N", GSK_N); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_O", GSK_O); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_P", GSK_P); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_Q", GSK_Q); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_R", GSK_R); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_S", GSK_S); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_T", GSK_T); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_U", GSK_U); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_V", GSK_V); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_W", GSK_W); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_X", GSK_X); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_Y", GSK_Y); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_Z", GSK_Z); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_0", GSK_0); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_1", GSK_1); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_2", GSK_2); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_3", GSK_3); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_4", GSK_4); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_5", GSK_5); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_6", GSK_6); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_7", GSK_7); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_8", GSK_8); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_9", GSK_9); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_MINUS", GSK_MINUS); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_PLUS", GSK_PLUS); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_COMMA", GSK_COMMA); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_DOT", GSK_DOT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_CTRL", GSK_CTRL); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_ALT", GSK_ALT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_SHIFT", GSK_SHIFT); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_META", GSK_META); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_RMOUSE", GSK_RMOUSE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_MMOUSE", GSK_MMOUSE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_LMOUSE", GSK_LMOUSE); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD0", GSK_NUMPAD0); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD1", GSK_NUMPAD1); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD2", GSK_NUMPAD2); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD3", GSK_NUMPAD3); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD4", GSK_NUMPAD4); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD5", GSK_NUMPAD5); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD6", GSK_NUMPAD6); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD7", GSK_NUMPAD7); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD8", GSK_NUMPAD8); assert(r >= 0);
	r = pASEngine->RegisterEnumValue("KEY", "K_NUMPAD9", GSK_NUMPAD9); assert(r >= 0);
}

void RegisterGlobalProperties(asIScriptEngine *pASEngine)
{
	int r;
	static float pi = gs2d::math::constant::PI;
	static float pib = gs2d::math::constant::PIb;
	static std::size_t stdNpos = std::string::npos;
	r = pASEngine->RegisterGlobalProperty("const float PI", &pi); assert( r >= 0 );
	r = pASEngine->RegisterGlobalProperty("const float PIb", &pib); assert( r >= 0 );

	std::string nposDecl;
	std::string sizeTypeDecl;
	
	#pragma warning( push )
	#pragma warning( disable : 4127 )
	// Register the object methods
	if( sizeof(std::string::npos) == 4 )
	#pragma warning( pop )
	{
		nposDecl = "const uint NPOS";
		sizeTypeDecl = "uint";
	}
	else
	{
		sizeTypeDecl = "uint64";
		nposDecl = "const uint64 NPOS";
	}
	r = pASEngine->RegisterGlobalProperty(nposDecl.c_str(), &stdNpos); assert( r >= 0 );

	pASEngine->RegisterTypedef("size_t", sizeTypeDecl.c_str());
}

bool RegisterVideoModeObject(asIScriptEngine *pASEngine)
{
	int r;
	r = pASEngine->RegisterObjectType("videoMode", sizeof(ETHScriptWrapper::ETH_VIDEO_MODE), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA); assert(r >= 0);

	// Register the object properties
	r = pASEngine->RegisterObjectProperty("videoMode", "uint width", offsetof(ETHScriptWrapper::ETH_VIDEO_MODE_POD, width)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("videoMode", "uint height", offsetof(ETHScriptWrapper::ETH_VIDEO_MODE_POD, height)); assert(r >= 0);
	r = pASEngine->RegisterObjectProperty("videoMode", "PIXEL_FORMAT format", offsetof(ETHScriptWrapper::ETH_VIDEO_MODE_POD, pf)); assert(r >= 0);

	return true;
}

void RegisterAllObjects(asIScriptEngine *pASEngine)
{
	// Register some useful scripts
	RegisterScriptArray(pASEngine, true);
	RegisterScriptMath2D(pASEngine);
	RegisterScriptMath3D(pASEngine);
	RegisterStdString(pASEngine);
	RegisterScriptMath(pASEngine);
	RegisterScriptFile(pASEngine);
	RegisterScriptDictionary(pASEngine);
	RegisterScriptAny(pASEngine);

	RegisterWebSocketClient(pASEngine);

	ETHGlobal::RegisterCollisionObject(pASEngine);
	ETHGlobal::RegisterVideoModeObject(pASEngine);
	ETHGlobal::RegisterMathObjectsAndFunctions(pASEngine);
	ETHGlobal::RegisterENMLObject(pASEngine);
	ETHGlobal::RegisterDateTimeObject(pASEngine);
	ETHGlobal::RegisterRevoluteJointObject(pASEngine);
	ETHGlobal::RegisterPhysicsControllerObject(pASEngine);
	ETHGlobal::RegisterEntityObject(pASEngine);
	ETHGlobal::RegisterInputObject(pASEngine);
	ETHGlobal::RegisterEntityArrayObject(pASEngine);
	ETHGlobal::RegisterJSONObject(pASEngine);
}

}
