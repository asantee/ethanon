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

#include "ETHASUtil.h"

#include <iostream>

#include "../Resource/ETHResourceProvider.h"

#include "../addons/scriptbuilder.h"

#include "../../gs2d/src/Unicode/utf8/utf8.h"

namespace ETHGlobal {
void ExecuteContext(asIScriptContext *pContext, asIScriptFunction* func, const bool prepare)
{
	if (prepare)
	{
		if (pContext->Prepare(func) < 0)
		{
			ETH_STREAM_DECL(ss) << GS_L("(ExecuteContext) Couldn't prepare context for function  ID ") << func->GetId();
			ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
			return;
		}
	}

	pContext->Execute();
}

void CheckFunctionSeekError(const int id, const str_type::string& function)
{
	str_type::stringstream ss;
	switch (id)
	{
	case asERROR:
		ss << GS_L("Callback function seeking error - Invalid module (") << function << GS_L(").");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		break;
	case asMULTIPLE_FUNCTIONS:
		ss << GS_L("\n*Script error:\nCallback function seeking error - there are multiple functions with this name (") << function << GS_L(").");
		ETHResourceProvider::Log(ss.str(), Platform::Logger::ERROR);
		break;
	};
}

asIScriptFunction* FindCallbackFunction(asIScriptModule* pModule, const ETHScriptEntity* entity, const str_type::string& prefix, const Platform::Logger& logger)
{
	const str_type::string entityName = Platform::RemoveExtension(entity->GetEntityName().c_str());
	str_type::stringstream funcName;
	funcName << prefix << entityName;
	asIScriptFunction* func = pModule->GetFunctionByName(funcName.str().c_str());

	// TODO/TO-DO: handle function overload ambiguity
	GS2D_UNUSED_ARGUMENT(logger);
	/*if (id == asMULTIPLE_FUNCTIONS)
	{
		str_type::stringstream ss;
		ss << GS_L("ETHScene::FindCallbackFunction: found multiple functions named (") << funcName.str() << GS_L(").");
		logger.Log(ss.str(), Platform::FileLogger::ERROR);
	}*/
	return func;
}

bool RunEntityCallback(asIScriptContext* pContext, ETHScriptEntity* entity, asIScriptFunction* func)
{
	if (!func)
		return false;

	if (pContext->Prepare(func) < 0)
		return false;

	if (pContext->SetArgObject(0, entity) >= 0)
		ETHGlobal::ExecuteContext(pContext, func, false);
	else
		return false;
	return true;
}

bool IsTrue(const std::string& source)
{
	if (source == "true" || source == "TRUE" || source == "yes" || source == "YES" || source == "1")
		return true;
	else
		return false;
}

bool FileExists(const str_type::string& file, const Platform::FileManagerPtr& fileManager)
{
	return fileManager->FileExists(file);
}

std::string AppendExtensionIfNeeded(std::string source, const std::string& ext)
{
	if (source.rfind(ext) != (source.size() - ext.size()))
	{
		source.append(ext);
	}
	return source;
}

str_type::string GetDataResourceFullPath(const str_type::string& path, const str_type::string& file)
{
	return path + file;
}

Vector2 ToScreenPos(const Vector3 &v3Pos, const Vector2 &zAxisDirection)
{
	return Vector2(v3Pos.x, v3Pos.y)+(zAxisDirection*v3Pos.z);
}

Vector2 ToVector2(const Vector3 &v3)
{
	return Vector2(v3.x, v3.y);
}

Vector2 V2iToV2F(const Vector2i &v2)
{
	return Vector2(static_cast<float>(v2.x), static_cast<float>(v2.y));
}

bool ToBool(const ETH_BOOL b)
{
	return (b == ETH_TRUE);
}

float ParseFloat(const str_type::char_t* str)
{
	float f = 0.0f;
	GS2D_SSCANF(str, GS_L("%f"), &f);
	return f;
}

int ParseInt(const str_type::char_t* str)
{
	int n = 0;
	GS2D_SSCANF(str, GS_L("%d"), &n);
	return n;
}

unsigned int ParseUInt(const str_type::char_t* str)
{
	unsigned int n = 0;
	GS2D_SSCANF(str, GS_L("%u"), &n);
	return n;
}

float ParseFloatStd(const str_type::string& str)
{
	return ParseFloat(str.c_str());
}

int ParseIntStd(const str_type::string& str)
{
	return ParseInt(str.c_str());
}

unsigned int ParseUIntStd(const str_type::string& str)
{
	return ParseUInt(str.c_str());
}

bool IsValidUTF8(const str_type::string& str)
{
	return utf8::is_valid(&str[0], &str[0] + str.length());
}

unsigned int DistanceUTF8(const str_type::string& str)
{
	return static_cast<unsigned int>(utf8::distance(&str[0], &str[0] + str.length()));
}

bool IsSphereInScreen(const Vector3& pos, const float radius, const Vector2& zAxisDir, const VideoPtr& video)
{
	const Vector2 v2Pos(ToScreenPos(pos, zAxisDir));
	const Vector2& v2Cam = video->GetCameraPos();
	const Vector2& v2Screen = video->GetScreenSizeF();
	const float minX = v2Cam.x - radius;
	const float maxX = v2Cam.x + v2Screen.x + radius;
	const float minY = v2Cam.y - radius;
	const float maxY = v2Cam.y + v2Screen.y + radius;
	if (v2Pos.x < minX || v2Pos.x > maxX || v2Pos.y < minY || v2Pos.y > maxY)
	{
		return false;
	}
	else
	{
		return true;
	}
}

} // namespace ETHGlobal