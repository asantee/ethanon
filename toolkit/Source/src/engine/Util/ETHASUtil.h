/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/

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

#ifndef ETH_AS_UTIL_H_
#define ETH_AS_UTIL_H_

#include "../../angelscript/include/angelscript.h"
#include "../ETHTypes.h"
#include "../Entity/ETHScriptEntity.h"

namespace ETHGlobal {
	void CheckFunctionSeekError(const int id, const str_type::string& function);
	void PrintException(asIScriptContext *pContext);
	void ShowMessage(str_type::string message, const ETH_MESSAGE type);
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

	float ParseFloat(const str_type::char_t *str);
	int ParseInt(const str_type::char_t *str);
	unsigned int ParseUInt(const str_type::char_t *str);
	float ParseFloatStd(const str_type::string &str);
	int ParseIntStd(const str_type::string &str);
	unsigned int ParseUIntStd(const str_type::string &str);

#ifdef GS2D_STR_TYPE_WCHAR
	std::wstring AppendExtensionIfNeeded(std::wstring source, const std::wstring& ext);
	bool IsTrue(const std::wstring& source);
#endif

} // namespace ETHGlobal

#endif
