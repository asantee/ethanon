#ifndef ETH_AS_UTIL_H_
#define ETH_AS_UTIL_H_

#include "../../angelscript/include/angelscript.h"
#include "../ETHTypes.h"
#include "../Entity/ETHScriptEntity.h"

namespace ETHGlobal {
	void CheckFunctionSeekError(const int id, const str_type::string& function);
	void PrintException(asIScriptContext *pContext);
	void ExecuteContext(asIScriptContext *pContext, asIScriptFunction* func, const bool prepare = true);
	asIScriptFunction* FindCallbackFunction(asIScriptModule* pModule, const ETHScriptEntity* entity, const str_type::string& prefix, const Platform::Logger& logger);
	bool RunEntityCallback(asIScriptContext* pContext, ETHScriptEntity* entity, asIScriptFunction* func);
	bool IsTrue(const std::string& source);
	bool FileExists(const str_type::string& file, const Platform::FileManagerPtr& fileManager);
	std::string AppendExtensionIfNeeded(std::string source, const std::string& ext);
	str_type::string GetDataResourceFullPath(const str_type::string& path, const str_type::string& file);
	Vector2 ToScreenPos(const Vector3 &v3Pos, const Vector2 &zAxisDirection);
	Vector2 ToVector2(const Vector3 &v3);
	Vector2 V2iToV2F(const Vector2i &v2);
	bool ToBool(const ETH_BOOL b);
	bool IsSphereInScreen(const Vector3& v3Pos, const float radius, const Vector2& zAxisDir, const VideoPtr& video);

	bool IsValidUTF8(const str_type::string& str);
	unsigned int DistanceUTF8(const str_type::string& str);

	float ParseFloat(const str_type::char_t *str);
	double ParseDouble(const str_type::char_t *str);
	int ParseInt(const str_type::char_t *str);
	unsigned int ParseUInt(const str_type::char_t *str);
	long long ParseInt64(const str_type::char_t *str);
	unsigned long long ParseUInt64(const str_type::char_t *str);

	float ParseFloatStd(const str_type::string &str);
	double ParseDoubleStd(const str_type::string &str);
	int ParseIntStd(const str_type::string &str);
	unsigned int ParseUIntStd(const str_type::string &str);
	long long ParseInt64Std(const str_type::string &str);
	unsigned long long ParseUInt64Std(const str_type::string &str);
} // namespace ETHGlobal

#endif
