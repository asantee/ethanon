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

#include "ETHScriptWrapper.h"


void ETHScriptWrapper::RegisterGlobalFunctions(asIScriptEngine *pASEngine)
{
	int r;

	// Register input
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetCursorPos()", asFUNCTION(GetCursorPos), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetCursorAbsolutePos()", asFUNCTION(GetCursorAbsolutePos), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "void SetCursorPos(const vector2 &in pos)", asFUNCTION(SetCursorPos), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "string GetLastCharInput() const", asFUNCTION(GetLastCharInput), asCALL_CDECL_OBJLAST); assert(r >= 0);
	// ETHInput retriever
	r = pASEngine->RegisterGlobalFunction("ETHInput @GetInputHandle()", asFUNCTION(GetInputHandle), asCALL_CDECL); assert(r >= 0);

	// register ETHEntity retrievers
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPosition(const vector3 &in)", asFUNCTION(SetPosition), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPositionXY(const vector2 &in)", asFUNCTION(SetPositionXY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPosition(const vector3 &in)", asFUNCTION(AddToPosition), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPositionXY(const vector2 &in)", asFUNCTION(AddToPositionXY), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetCurrentBucket() const", asFUNCTION(GetCurrentBucket), asCALL_CDECL_OBJFIRST); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("ETHEntity @SeekEntity(const string &in)", asFUNCTIONPR(SeekEntity, (const str_type::string&), ETHEntity*), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("ETHEntity @SeekEntity(const int)", asFUNCTIONPR(SeekEntity, (const int), ETHEntity*), asCALL_CDECL); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(Print), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void print(const float)", asFUNCTION(PrintFloat), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void print(const int)", asFUNCTION(PrintInt), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void print(const uint)", asFUNCTION(PrintUInt), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in)", asFUNCTIONPR(LoadSceneInScript, (const str_type::string &), void), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in)", asFUNCTIONPR(LoadSceneInScript, (const str_type::string&,const str_type::string&,const str_type::string&), void), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in, const vector2 &in)", asFUNCTIONPR(LoadSceneInScript, (const str_type::string&,const str_type::string&,const str_type::string&, const Vector2&), void), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetTimeF()", asFUNCTION(GetTimeF), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetTime()", asFUNCTION(GetTime), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float UnitsPerSecond(const float)", asFUNCTION(UnitsPerSecond), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void Exit()", asFUNCTION(Exit), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, const float)", asFUNCTIONPR(AddEntity, (const str_type::string &file, const Vector3 &v3Pos, const float angle), int), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, ETHEntity@ &out)", asFUNCTIONPR(AddEntity, (const str_type::string &file, const Vector3 &v3Pos, ETHEntity **), int), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, const string &in)", asFUNCTIONPR(AddEntity, (const str_type::string &file, const Vector3 &v3Pos, const str_type::string &alternativeName), int), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, const float, ETHEntity@ &out, const string &in, const float)",
		asFUNCTIONPR(AddEntity, (const str_type::string&, const Vector3&, const float, ETHEntity**, const str_type::string&, const float), int, asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("ETHEntity @DeleteEntity(ETHEntity @)", asFUNCTION(DeleteEntity), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GenerateLightmaps()", asFUNCTION(GenerateLightmaps), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int rand(const int)", asFUNCTIONPR(Randomizer::Int, (const int), int), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int rand(const int, const int)", asFUNCTIONPR(Randomizer::Int, (const int, const int), int), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float randF(const float)", asFUNCTIONPR(Randomizer::Float, (const float), float), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float randF(const float, const float)", asFUNCTIONPR(Randomizer::Float, (const float, const float), float), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetAmbientLight(const vector3 &in)", asFUNCTION(SetAmbientLight), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector3 GetAmbientLight()", asFUNCTION(GetAmbientLight), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetParallaxOrigin(const vector2 &in)", asFUNCTION(SetParallaxOrigin), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction(
		"void SetWindowProperties(const string &in, const uint, const uint, const bool, const bool, const PIXEL_FORMAT)",
		asFUNCTION(SetWindowProperties), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetScreenWidth()", asFUNCTION(GetScreenWidth), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetScreenHeight()", asFUNCTION(GetScreenHeight), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetCameraPos(const vector2 &in)", asFUNCTION(SetCameraPos), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void AddToCameraPos(const vector2 &in)", asFUNCTION(AddToCameraPos), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetCameraPos()", asFUNCTION(GetCameraPos), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void EnableLightmaps(const bool)", asFUNCTION(EnableLightmaps), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void UsePixelShaders(const bool)", asFUNCTION(UsePixelShaders), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawText(const vector2 &in, const string &in, const string &in, const uint, const float scale = 1.0f)", asFUNCTION(DrawText), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawFadingText(const vector2 &in, const string &in, const string &in, const uint, const uint, const float scale = 1.0f)", asFUNCTION(DrawFadingText), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetFPSRate()", asFUNCTION(GetFPSRate), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void AddLight(const vector3 &in, const vector3 &in, const float, const bool)", asFUNCTION(AddLight), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void HideCursor(const bool)", asFUNCTION(HideCursor), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool LoadMusic(const string &in)", asFUNCTION(LoadMusic), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool LoadSoundEffect(const string &in)", asFUNCTION(LoadSoundEffect), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool PlaySample(const string &in)", asFUNCTION(PlaySample), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool LoopSample(const string &in, const bool)", asFUNCTION(LoopSample), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool StopSample(const string &in)", asFUNCTION(StopSample), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool PauseSample(const string &in)", asFUNCTION(PauseSample), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SetSampleVolume(const string &in, const float)", asFUNCTION(SetSampleVolume), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SetSamplePan(const string &in, const float)", asFUNCTION(SetSamplePan), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SampleExists(const string &in)", asFUNCTION(SampleExists), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsSamplePlaying(const string &in)", asFUNCTION(IsSamplePlaying), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetGlobalVolume(const float)", asFUNCTION(SetGlobalVolume), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetGlobalVolume()", asFUNCTION(GetGlobalVolume), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetNumEntities()", asFUNCTION(GetNumEntities), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int GetLastID()", asFUNCTION(GetLastID), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddFloatData(const string &in, const string &in, const float)", asFUNCTION(AddFloatData), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddIntData(const string &in, const string &in, const int)", asFUNCTION(AddIntData), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddUIntData(const string &in, const string &in, const uint)", asFUNCTION(AddUIntData), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddStringData(const string &in, const string &in, const string &in)", asFUNCTION(AddStringData), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddVector2Data(const string &in, const string &in, const vector2 &in)", asFUNCTION(AddVector2Data), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddVector3Data(const string &in, const string &in, const vector3 &in)", asFUNCTION(AddVector3Data), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SaveScene(const string &in)", asFUNCTION(SaveScene), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetMaxHeight()", asFUNCTION(GetMaxHeight), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetMinHeight()", asFUNCTION(GetMinHeight), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector3 normalize(const vector3 &in)", asFUNCTIONPR(Normalize, (const Vector3&), Vector3), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 normalize(const vector2 &in)", asFUNCTIONPR(Normalize, (const Vector2&), Vector2), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float radianToDegree(const float)", asFUNCTION(RadianToDegree), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float degreeToRadian(const float)", asFUNCTION(DegreeToRadian), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint ARGB(const uint8, const uint8, const uint8, const uint8)", asFUNCTION(ARGB), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadSprite(const string &in)", asFUNCTION(LoadSprite), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawSprite(const string &in, const vector2 &in, const uint)", asFUNCTION(DrawSprite), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawShapedSprite(const string &in, const vector2 &in, const vector2 &in, const uint)", asFUNCTION(DrawShaped), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetSpriteSize(const string &in)", asFUNCTION(GetSpriteSize), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawRectangle(const vector2 &in, const vector2 &in, const uint, const uint, const uint, const uint)", asFUNCTION(DrawRectangle), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawLine(const vector2 &in, const vector2 &in, const uint, const uint, const float)", asFUNCTION(DrawLine), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetSceneFileName()", asFUNCTION(GetSceneFileName), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void PlayCutscene(const string &in)", asFUNCTION(PlayCutscene), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetScreenSize()", asFUNCTION(GetScreenSize), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool Windowed()", asFUNCTION(Windowed), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetVideoModeCount()", asFUNCTION(GetVideoModeCount), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("videoMode GetVideoMode(const uint)", asFUNCTION(GetVideoMode), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetBackgroundColor()", asFUNCTION(GetBackgroundColor), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetBackgroundColor(const uint)", asFUNCTION(SetBackgroundColor), asCALL_CDECL); assert(r >= 0);
	//r = pASEngine->RegisterGlobalFunction("bool SetBackgroundImage(const string &in)", asFUNCTION(SetBackgroundImage), asCALL_CDECL); assert(r >= 0);
	//r = pASEngine->RegisterGlobalFunction("void PositionBackgroundImage(const vector2 &in, const vector2 &in)", asFUNCTION(PositionBackgroundImage), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetSystemScreenSize()", asFUNCTION(GetSystemScreenSize), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetEntityArray(const string &in, ETHEntityArray &)", asFUNCTION(GetEntityArrayByName), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetEntitiesFromBucket(const vector2 &in, ETHEntityArray &)", asFUNCTION(GetEntityArrayFromBucket), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetVisibleEntities(ETHEntityArray &)", asFUNCTION(GetVisibleEntities), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetIntersectingEntities(const vector2 &in, ETHEntityArray &, const bool)", asFUNCTION(GetIntersectingEntities), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetBucket(const vector2 &in)", asFUNCTION(GetBucket), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetAllEntitiesInScene(ETHEntityArray &)", asFUNCTION(GetAllEntitiesInScene), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsPixelShaderSupported()", asFUNCTION(IsPixelShaderSupported), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetProgramPath()", asFUNCTION(GetProgramPath), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetExternalStoragePath()", asFUNCTION(GetExternalStoragePath),  asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawSpriteZ(const string &in, const float, const vector2 &in, const uint)", asFUNCTION(DrawSpriteZ), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawShapedSpriteZ(const string &in, const float, const vector2 &in, const vector2 &in, const uint)", asFUNCTION(DrawShapedZ), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetPositionRoundUp(const bool)", asFUNCTION(SetPositionRoundUp), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetPositionRoundUp()", asFUNCTION(GetPositionRoundUp), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetHaloRotation(const bool)", asFUNCTION(SetHaloRotation), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void EnableQuitKeys(const bool)", asFUNCTION(EnableQuitKeys), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void EnableRealTimeShadows(const bool)", asFUNCTION(EnableRealTimeShadows), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetBorderBucketsDrawing(const bool)", asFUNCTION(SetBorderBucketsDrawing), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsDrawingBorderBuckets()", asFUNCTION(IsDrawingBorderBuckets), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetAbsolutePath(const string &in)", asFUNCTION(GetAbsolutePath), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetLastCameraPos()", asFUNCTION(GetLastCameraPos), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int GetNumRenderedEntities()", asFUNCTION(GetNumRenderedEntities), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int ParseInt(const string &in)", asFUNCTION(ETHGlobal::ParseIntStd), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint ParseUInt(const string &in)", asFUNCTION(ETHGlobal::ParseUIntStd), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float ParseFloat(const string &in)", asFUNCTION(ETHGlobal::ParseFloatStd), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int GetArgc()", asFUNCTION(GetArgc), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetArgv(const int)", asFUNCTION(GetArgv), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetWorldSpaceCursorPos2()", asFUNCTION(GetWorldSpaceCursorPos2), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector3 GetWorldSpaceCursorPos3()", asFUNCTION(GetWorldSpaceCursorPos3), asCALL_CDECL); assert(r >= 0);
	//r = pASEngine->RegisterGlobalFunction("void SetBackgroundAlphaModulate()", asFUNCTION(SetBackgroundAlphaModulate), asCALL_CDECL); assert(r >= 0);
	//r = pASEngine->RegisterGlobalFunction("void SetBackgroundAlphaAdd()", asFUNCTION(SetBackgroundAlphaAdd), asCALL_CDECL); assert(r >= 0);
	//r = pASEngine->RegisterGlobalFunction("void SetBackgroundAlphaPixel()", asFUNCTION(SetBackgroundAlphaPixel), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 ComputeCarretPosition(const string &in, const string &in, const uint pos)", asFUNCTION(ComputeCarretPosition), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 ComputeTextBoxSize(const string &in, const string &in)", asFUNCTION(ComputeTextBoxSize), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void ForwardCommand(const string &in)", asFUNCTION(ForwardCommand), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetZBuffer(const bool)", asFUNCTION(SetZBuffer), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetZBuffer()",           asFUNCTION(GetZBuffer), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetPersistentResources(const bool)", asFUNCTION(SetPersistentResources), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsPersistentResources()",            asFUNCTION(IsPersistentResources),  asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void ReleaseResources()",                 asFUNCTION(ReleaseResources),       asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetFastGarbageCollector(const bool)", asFUNCTION(SetFastGarbageCollector), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetLastFrameElapsedTime()", asFUNCTION(GetLastFrameElapsedTime), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetSpriteFrameSize(const string &in)",                    asFUNCTION(GetSpriteFrameSize), asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetupSpriteRects(const string &in, const uint, const uint)", asFUNCTION(SetupSpriteRects),   asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetSpriteRect(const string &in, const uint)",                asFUNCTION(SetSpriteRect),      asCALL_CDECL); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetSpriteOrigin(const string &in, const vector2 &in)",       asFUNCTION(SetSpriteOrigin),    asCALL_CDECL); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("string GetStringFromFileInPackage(const string &in)", asFUNCTION(GetStringFromFileInPackage), asCALL_CDECL); assert(r >= 0);

	m_math.RegisterGlobals(pASEngine);
}
