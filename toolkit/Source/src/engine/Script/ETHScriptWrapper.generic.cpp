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

#include "ETHScriptWrapper.h"
#include "../../addons/aswrappedcall.h"
#include "../Entity/ETHRenderEntity.h"

#include <Math/Randomizer.h>

asDECLARE_FUNCTION_WRAPPER(__GetCursorPos,         ETHScriptWrapper::GetCursorPos);
asDECLARE_FUNCTION_WRAPPER(__GetCursorAbsolutePos, ETHScriptWrapper::GetCursorAbsolutePos);
asDECLARE_FUNCTION_WRAPPER(__SetCursorPos,         ETHScriptWrapper::SetCursorPos);
asDECLARE_FUNCTION_WRAPPER(__GetLastCharInput,     ETHScriptWrapper::GetLastCharInput);
asDECLARE_FUNCTION_WRAPPER(__GetInputHandle,       ETHScriptWrapper::GetInputHandle);
asDECLARE_FUNCTION_WRAPPER(__HideCursor,           ETHScriptWrapper::HideCursor);

//asDECLARE_FUNCTION_OBJ_WRAPPERPR(wrapper_name,func,objfirst,params,rettype)
//asDECLARE_FUNCTION_OBJ_WRAPPER(wrapper_name,func,objfirst)

asDECLARE_FUNCTION_OBJ_WRAPPER(__SetPosition,         ETHScriptWrapper::SetPosition, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__SetPositionXY,       ETHScriptWrapper::SetPositionXY, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__SetPositionX,        ETHScriptWrapper::SetPositionX, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__SetPositionY,        ETHScriptWrapper::SetPositionY, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__SetPositionZ,        ETHScriptWrapper::SetPositionZ, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__AddToPosition,       ETHScriptWrapper::AddToPosition, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__AddToPositionXY,     ETHScriptWrapper::AddToPositionXY, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__AddToPositionX,      ETHScriptWrapper::AddToPositionX, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__AddToPositionY,      ETHScriptWrapper::AddToPositionY, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__AddToPositionZ,      ETHScriptWrapper::AddToPositionZ, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__PlayParticleSystem,  ETHScriptWrapper::PlayParticleSystem, true);
asDECLARE_FUNCTION_OBJ_WRAPPER(__ResolveEntityJoints, ETHScriptWrapper::ResolveEntityJoints, true);

asDECLARE_FUNCTION_WRAPPERPR(__SeekEntityStr,   ETHScriptWrapper::SeekEntity, (const str_type::string&), ETHEntity *);
asDECLARE_FUNCTION_WRAPPERPR(__SeekEntityInt,   ETHScriptWrapper::SeekEntity, (const int), ETHEntity *);

asDECLARE_FUNCTION_WRAPPER(__Print,      ETHScriptWrapper::Print);
asDECLARE_FUNCTION_WRAPPER(__PrintFloat, ETHScriptWrapper::PrintFloat);
asDECLARE_FUNCTION_WRAPPER(__PrintInt,   ETHScriptWrapper::PrintInt);
asDECLARE_FUNCTION_WRAPPER(__PrintUInt,  ETHScriptWrapper::PrintUInt);

asDECLARE_FUNCTION_WRAPPERPR(__LoadScene1Arg,      ETHScriptWrapper::LoadSceneInScript, (const str_type::string&), void);
asDECLARE_FUNCTION_WRAPPERPR(__LoadSceneSSSArgs,   ETHScriptWrapper::LoadSceneInScript, (const str_type::string&, const str_type::string&, const str_type::string&), void);
asDECLARE_FUNCTION_WRAPPERPR(__LoadSceneSSSSArgs,  ETHScriptWrapper::LoadSceneInScript, (const str_type::string&, const str_type::string&, const str_type::string&, const str_type::string&), void);
asDECLARE_FUNCTION_WRAPPERPR(__LoadSceneSSSVArgs,  ETHScriptWrapper::LoadSceneInScript, (const str_type::string&, const str_type::string&, const str_type::string&, const Vector2&), void);
asDECLARE_FUNCTION_WRAPPERPR(__LoadSceneSSSSVArgs, ETHScriptWrapper::LoadSceneInScript, (const str_type::string&, const str_type::string&, const str_type::string&, const str_type::string&, const Vector2&), void);
asDECLARE_FUNCTION_WRAPPERPR(__LoadSceneSSSSSVArgs, ETHScriptWrapper::LoadSceneInScript, (const str_type::string&, const str_type::string&, const str_type::string&, const str_type::string&, const str_type::string&, const Vector2&), void);

asDECLARE_FUNCTION_WRAPPER(__SaveScene, ETHScriptWrapper::SaveScene);

asDECLARE_FUNCTION_WRAPPER(__GetTimeF,       ETHScriptWrapper::GetTimeF);
asDECLARE_FUNCTION_WRAPPER(__GetTime,        ETHScriptWrapper::GetTime);
asDECLARE_FUNCTION_WRAPPER(__UnitsPerSecond, ETHScriptWrapper::UnitsPerSecond);
asDECLARE_FUNCTION_WRAPPER(__Exit,           ETHScriptWrapper::Exit);
asDECLARE_FUNCTION_WRAPPER(__GetLastFrameElapsedTime, ETHScriptWrapper::GetLastFrameElapsedTime);

asDECLARE_FUNCTION_WRAPPERPR(__AddEntityA, ETHScriptWrapper::AddEntity,       (const str_type::string&, const Vector3&, const float), int);
asDECLARE_FUNCTION_WRAPPERPR(__AddEntityR, ETHScriptWrapper::AddEntity,       (const str_type::string&, const Vector3&, ETHEntity**), int);
asDECLARE_FUNCTION_WRAPPERPR(__AddEntityN, ETHScriptWrapper::AddEntity,       (const str_type::string&, const Vector3&, const str_type::string&), int);
asDECLARE_FUNCTION_WRAPPERPR(__AddEntityS, ETHScriptWrapper::AddScaledEntity, (const str_type::string&, const Vector3&, const float), int);
asDECLARE_FUNCTION_WRAPPERPR(__AddEntitySR,ETHScriptWrapper::AddScaledEntity, (const str_type::string&, const Vector3&, const float, ETHEntity**), int);
asDECLARE_FUNCTION_WRAPPERPR(__AddEntityF, ETHScriptWrapper::AddEntity,       (const str_type::string&, const Vector3&, const float, ETHEntity**, const str_type::string&, const float), int);

//asDECLARE_FUNCTION_WRAPPER(__DeleteEntity,      ETHScriptWrapper::DeleteEntity);
asDECLARE_FUNCTION_WRAPPER(__GenerateLightmaps, ETHScriptWrapper::GenerateLightmaps);
asDECLARE_FUNCTION_WRAPPER(__AddLight,   ETHScriptWrapper::AddLight);

asDECLARE_FUNCTION_WRAPPERPR(__RandomizerInt,        Randomizer::Int,   (const int), int);
asDECLARE_FUNCTION_WRAPPERPR(__RandomizerInt2Args,   Randomizer::Int,   (const int, const int), int);
asDECLARE_FUNCTION_WRAPPERPR(__RandomizerFloat,      Randomizer::Float, (const float), float);
asDECLARE_FUNCTION_WRAPPERPR(__RandomizerFloat2Args, Randomizer::Float, (const float, const float), float);

asDECLARE_FUNCTION_WRAPPER(__SetAmbientLight,   ETHScriptWrapper::SetAmbientLight);
asDECLARE_FUNCTION_WRAPPER(__GetAmbientLight,   ETHScriptWrapper::GetAmbientLight);
asDECLARE_FUNCTION_WRAPPER(__SetParallaxOrigin, ETHScriptWrapper::SetParallaxOrigin);
asDECLARE_FUNCTION_WRAPPER(__GetParallaxOrigin, ETHScriptWrapper::GetParallaxOrigin);
asDECLARE_FUNCTION_WRAPPER(__SetParallaxIntensity, ETHScriptWrapper::SetParallaxIntensity);
asDECLARE_FUNCTION_WRAPPER(__GetParallaxIntensity, ETHScriptWrapper::GetParallaxIntensity);
asDECLARE_FUNCTION_WRAPPER(__SetParallaxVerticalIntensity, ETHScriptWrapper::SetParallaxVerticalIntensity);
asDECLARE_FUNCTION_WRAPPER(__GetParallaxVerticalIntensity, ETHScriptWrapper::GetParallaxVerticalIntensity);
asDECLARE_FUNCTION_WRAPPER(__SetBucketClearenceFactor, ETHScriptWrapper::SetBucketClearenceFactor);
asDECLARE_FUNCTION_WRAPPER(__GetBucketClearenceFactor, ETHScriptWrapper::GetBucketClearenceFactor);

asDECLARE_FUNCTION_WRAPPER(__SetWindowProperties, ETHScriptWrapper::SetWindowProperties);
asDECLARE_FUNCTION_WRAPPER(__SetCameraPos,        ETHScriptWrapper::SetCameraPos);
asDECLARE_FUNCTION_WRAPPER(__AddToCameraPos,      ETHScriptWrapper::AddToCameraPos);
asDECLARE_FUNCTION_WRAPPER(__GetCameraPos,        ETHScriptWrapper::GetCameraPos);

asDECLARE_FUNCTION_WRAPPER(__DrawText,              ETHScriptWrapper::DrawText);
asDECLARE_FUNCTION_WRAPPER(__DrawFadingText,        ETHScriptWrapper::DrawFadingText);
asDECLARE_FUNCTION_WRAPPER(__ComputeCarretPosition, ETHScriptWrapper::ComputeCarretPosition);
asDECLARE_FUNCTION_WRAPPER(__ComputeTextBoxSize,    ETHScriptWrapper::ComputeTextBoxSize);
asDECLARE_FUNCTION_WRAPPER(__AssembleColorCode,     ETHScriptWrapper::AssembleColorCode);

asDECLARE_FUNCTION_WRAPPER(__EnableLightmaps, ETHScriptWrapper::EnableLightmaps);
asDECLARE_FUNCTION_WRAPPER(__UsePixelShaders, ETHScriptWrapper::UsePixelShaders);
asDECLARE_FUNCTION_WRAPPER(__GetFPSRate,      ETHScriptWrapper::GetFPSRate);
asDECLARE_FUNCTION_WRAPPER(__EnablePreLoadedLightmapsFromFile, ETHScriptWrapper::EnablePreLoadedLightmapsFromFile);

asDECLARE_FUNCTION_WRAPPER(__LoadMusic,       ETHScriptWrapper::LoadMusic);
asDECLARE_FUNCTION_WRAPPER(__LoadSoundEffect, ETHScriptWrapper::LoadSoundEffect);
asDECLARE_FUNCTION_WRAPPER(__PlaySample,      ETHScriptWrapper::PlaySample);
asDECLARE_FUNCTION_WRAPPER(__LoopSample,      ETHScriptWrapper::LoopSample);
asDECLARE_FUNCTION_WRAPPER(__StopSample,      ETHScriptWrapper::StopSample);
asDECLARE_FUNCTION_WRAPPER(__PauseSample,     ETHScriptWrapper::PauseSample);
asDECLARE_FUNCTION_WRAPPER(__SetSampleVolume, ETHScriptWrapper::SetSampleVolume);
asDECLARE_FUNCTION_WRAPPER(__SetSampleSpeed,  ETHScriptWrapper::SetSampleSpeed);
asDECLARE_FUNCTION_WRAPPER(__SetSamplePan,    ETHScriptWrapper::SetSamplePan);
asDECLARE_FUNCTION_WRAPPER(__SampleExists,    ETHScriptWrapper::SampleExists);
asDECLARE_FUNCTION_WRAPPER(__IsSamplePlaying, ETHScriptWrapper::IsSamplePlaying);
asDECLARE_FUNCTION_WRAPPER(__SetGlobalVolume, ETHScriptWrapper::SetGlobalVolume);
asDECLARE_FUNCTION_WRAPPER(__GetGlobalVolume, ETHScriptWrapper::GetGlobalVolume);

asDECLARE_FUNCTION_WRAPPER(__GetNumEntities, ETHScriptWrapper::GetNumEntities);

asDECLARE_FUNCTION_WRAPPER(__AddFloatData,   ETHScriptWrapper::AddFloatData);
asDECLARE_FUNCTION_WRAPPER(__AddIntData,     ETHScriptWrapper::AddIntData);
asDECLARE_FUNCTION_WRAPPER(__AddUIntData,    ETHScriptWrapper::AddUIntData);
asDECLARE_FUNCTION_WRAPPER(__AddStringData,  ETHScriptWrapper::AddStringData);
asDECLARE_FUNCTION_WRAPPER(__AddVector2Data, ETHScriptWrapper::AddVector2Data);
asDECLARE_FUNCTION_WRAPPER(__AddVector3Data, ETHScriptWrapper::AddVector3Data);

asDECLARE_FUNCTION_WRAPPER(__SetZAxisDirection, ETHScriptWrapper::SetZAxisDirection);
asDECLARE_FUNCTION_WRAPPER(__GetZAxisDirection, ETHScriptWrapper::GetZAxisDirection);

asDECLARE_FUNCTION_WRAPPERPR(__normalize3, Normalize, (const Vector3&), Vector3);
asDECLARE_FUNCTION_WRAPPERPR(__normalize2, Normalize, (const Vector2&), Vector2);

asDECLARE_FUNCTION_WRAPPER(__radianToDegree, RadianToDegree);
asDECLARE_FUNCTION_WRAPPER(__degreeToRadian, DegreeToRadian);
asDECLARE_FUNCTION_WRAPPER(__ARGB,           ARGB);

asDECLARE_FUNCTION_WRAPPER(__LoadSprite,       ETHScriptWrapper::LoadSprite);
asDECLARE_FUNCTION_WRAPPER(__ReleaseSprite,    ETHScriptWrapper::ReleaseSprite);
asDECLARE_FUNCTION_WRAPPER(__GetSpriteSize,    ETHScriptWrapper::GetSpriteSize);
asDECLARE_FUNCTION_WRAPPER(__DrawRectangle,    ETHScriptWrapper::DrawRectangle);
asDECLARE_FUNCTION_WRAPPER(__DrawLine,         ETHScriptWrapper::DrawLine);

asDECLARE_FUNCTION_WRAPPERPR(__DrawSprite,       ETHScriptWrapper::DrawSprite, (const str_type::string &name, const Vector2 &v2Pos, const GS_DWORD color, const float angle), void);
asDECLARE_FUNCTION_WRAPPERPR(__DrawShapedSprite, ETHScriptWrapper::DrawShaped, (const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle), void);

asDECLARE_FUNCTION_WRAPPERPR(__DrawSpriteEx,       ETHScriptWrapper::DrawSprite, (const str_type::string &name, const Vector2 &v2Pos, const float alpha, const Vector3 &color, const float angle), void);
asDECLARE_FUNCTION_WRAPPERPR(__DrawShapedSpriteEx, ETHScriptWrapper::DrawShaped, (const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const float alpha, const Vector3 &color, const float angle), void);

asDECLARE_FUNCTION_WRAPPER(__PlayParticleEffect, ETHScriptWrapper::PlayParticleEffect);

asDECLARE_FUNCTION_WRAPPER(__GetSpriteFrameSize, ETHScriptWrapper::GetSpriteFrameSize);
asDECLARE_FUNCTION_WRAPPER(__SetupSpriteRects,   ETHScriptWrapper::SetupSpriteRects);
asDECLARE_FUNCTION_WRAPPER(__SetSpriteRect,      ETHScriptWrapper::SetSpriteRect);
asDECLARE_FUNCTION_WRAPPER(__SetSpriteOrigin,    ETHScriptWrapper::SetSpriteOrigin);
asDECLARE_FUNCTION_WRAPPER(__SetSpriteFlipX,    ETHScriptWrapper::SetSpriteFlipX);
asDECLARE_FUNCTION_WRAPPER(__SetSpriteFlipY,    ETHScriptWrapper::SetSpriteFlipY);

asDECLARE_FUNCTION_WRAPPER(__GetSceneFileName,    ETHScriptWrapper::GetSceneFileName);
asDECLARE_FUNCTION_WRAPPER(__GetScreenSize,       ETHScriptWrapper::GetScreenSize);
asDECLARE_FUNCTION_WRAPPER(__Windowed,            ETHScriptWrapper::Windowed);
asDECLARE_FUNCTION_WRAPPER(__GetVideoModeCount,   ETHScriptWrapper::GetVideoModeCount);
asDECLARE_FUNCTION_WRAPPER(__GetVideoMode,        ETHScriptWrapper::GetVideoMode);
asDECLARE_FUNCTION_WRAPPER(__GetBackgroundColor,  ETHScriptWrapper::GetBackgroundColor);
asDECLARE_FUNCTION_WRAPPER(__SetBackgroundColor,  ETHScriptWrapper::SetBackgroundColor);
asDECLARE_FUNCTION_WRAPPER(__GetSystemScreenSize, ETHScriptWrapper::GetSystemScreenSize);

asDECLARE_FUNCTION_WRAPPER(__GetEntityArray,          ETHScriptWrapper::GetEntityArrayByName);
asDECLARE_FUNCTION_WRAPPER(__GetEntitiesFromBucket,   ETHScriptWrapper::GetEntityArrayFromBucket);
asDECLARE_FUNCTION_WRAPPER(__GetVisibleEntities,      ETHScriptWrapper::GetVisibleEntities);
asDECLARE_FUNCTION_WRAPPER(__GetIntersectingEntities, ETHScriptWrapper::GetIntersectingEntities);
asDECLARE_FUNCTION_WRAPPER(__GetEntitiesAroundBucket, ETHScriptWrapper::GetEntitiesAroundBucket)
asDECLARE_FUNCTION_WRAPPER(__GetEntitiesAroundEntity, ETHScriptWrapper::GetEntitiesAroundEntity)
asDECLARE_FUNCTION_WRAPPER(__GetBucket,               ETHScriptWrapper::GetBucket);
asDECLARE_FUNCTION_WRAPPER(__GetAllEntitiesInScene,   ETHScriptWrapper::GetAllEntitiesInScene);
asDECLARE_FUNCTION_WRAPPER(__ForceEntityRendering,    ETHScriptWrapper::ForceEntityRendering);
asDECLARE_FUNCTION_WRAPPER(__GetWhiteListedEntitiesAroundBucket,  ETHScriptWrapper::GetWhiteListedEntitiesAroundBucket);
asDECLARE_FUNCTION_WRAPPER(__GetEntitiesAroundBucketWithBlackList, ETHScriptWrapper::GetEntitiesAroundBucketWithBlackList);

asDECLARE_FUNCTION_WRAPPER(__IsPixelShaderSupported,  ETHScriptWrapper::IsPixelShaderSupported);
asDECLARE_FUNCTION_WRAPPER(__SetPositionRoundUp,      ETHScriptWrapper::SetPositionRoundUp);
asDECLARE_FUNCTION_WRAPPER(__GetPositionRoundUp,      ETHScriptWrapper::GetPositionRoundUp);
asDECLARE_FUNCTION_WRAPPER(__EnableQuitKeys,          ETHScriptWrapper::EnableQuitKeys);
asDECLARE_FUNCTION_WRAPPER(__EnableRealTimeShadows,   ETHScriptWrapper::EnableRealTimeShadows);
asDECLARE_FUNCTION_WRAPPER(__SetBorderBucketsDrawing, ETHScriptWrapper::SetBorderBucketsDrawing);
asDECLARE_FUNCTION_WRAPPER(__IsDrawingBorderBuckets,  ETHScriptWrapper::IsDrawingBorderBuckets);

asDECLARE_FUNCTION_WRAPPER(__GetResourceDirectory,         ETHScriptWrapper::GetResourceDirectory);
asDECLARE_FUNCTION_WRAPPER(__GetExternalStoragePath,       ETHScriptWrapper::GetExternalStorageDirectory);
asDECLARE_FUNCTION_WRAPPER(__GetGlobalExternalStoragePath, ETHScriptWrapper::GetGlobalExternalStorageDirectory);
asDECLARE_FUNCTION_WRAPPER(__GetAbsolutePath,              ETHScriptWrapper::GetAbsolutePath);
asDECLARE_FUNCTION_WRAPPER(__GetLastCameraPos,             ETHScriptWrapper::GetLastCameraPos);
asDECLARE_FUNCTION_WRAPPER(__GetNumRenderedEntities,       ETHScriptWrapper::GetNumRenderedEntities);
asDECLARE_FUNCTION_WRAPPER(__ParseInt,                     ETHGlobal::ParseIntStd);
asDECLARE_FUNCTION_WRAPPER(__ParseUInt,                    ETHGlobal::ParseUIntStd);
asDECLARE_FUNCTION_WRAPPER(__ParseFloat,                   ETHGlobal::ParseFloatStd);
asDECLARE_FUNCTION_WRAPPER(__IsValidUTF8,                  ETHGlobal::IsValidUTF8);
asDECLARE_FUNCTION_WRAPPER(__DistanceUTF8,                 ETHGlobal::DistanceUTF8);
asDECLARE_FUNCTION_WRAPPER(__GetArgc,                      ETHScriptWrapper::GetArgc);
asDECLARE_FUNCTION_WRAPPER(__GetArgv,                      ETHScriptWrapper::GetArgv);
asDECLARE_FUNCTION_WRAPPER(__GetWorldSpaceCursorPos2,      ETHScriptWrapper::GetWorldSpaceCursorPos2);
asDECLARE_FUNCTION_WRAPPER(__ForwardCommand,               ETHScriptWrapper::ForwardCommand);

asDECLARE_FUNCTION_WRAPPER(__SetZBuffer, ETHScriptWrapper::SetZBuffer);
asDECLARE_FUNCTION_WRAPPER(__GetZBuffer, ETHScriptWrapper::GetZBuffer);

asDECLARE_FUNCTION_WRAPPER(__SetPersistentResources,        ETHScriptWrapper::SetPersistentResources);
asDECLARE_FUNCTION_WRAPPER(__ArePersistentResourcesEnabled, ETHScriptWrapper::ArePersistentResourcesEnabled);
asDECLARE_FUNCTION_WRAPPER(__ReleaseResources,              ETHScriptWrapper::ReleaseResources);
asDECLARE_FUNCTION_WRAPPER(__ResolveJoints,                 ETHScriptWrapper::ResolveJoints);
asDECLARE_FUNCTION_WRAPPER(__SetFastGarbageCollector,       ETHScriptWrapper::SetFastGarbageCollector);
asDECLARE_FUNCTION_WRAPPER(__GetStringFromFileInPackage,    ETHScriptWrapper::GetStringFromFileInPackage);
asDECLARE_FUNCTION_WRAPPER(__FileInPackageExists,           ETHScriptWrapper::FileInPackageExists);
asDECLARE_FUNCTION_WRAPPER(__FileExists,                    ETHScriptWrapper::FileExists);
asDECLARE_FUNCTION_WRAPPER(__IsHighEndDevice,               ETHScriptWrapper::IsHighEndDevice);
asDECLARE_FUNCTION_WRAPPER(__GetPlatformName,               ETHScriptWrapper::GetPlatformName);

asDECLARE_FUNCTION_WRAPPER(__EnablePackLoading,                ETHScriptWrapper::EnablePackLoading);
asDECLARE_FUNCTION_WRAPPER(__DisablePackLoading,               ETHScriptWrapper::DisablePackLoading);
asDECLARE_FUNCTION_WRAPPER(__IsResourcePackingSupported,       ETHScriptWrapper::IsResourcePackingSupported);
asDECLARE_FUNCTION_WRAPPER(__IsPackLoadingEnabled,             ETHScriptWrapper::IsPackLoadingEnabled);
asDECLARE_FUNCTION_WRAPPER(__EnableLightmapsFromExpansionPack, ETHScriptWrapper::EnableLightmapsFromExpansionPack);

asDECLARE_FUNCTION_WRAPPER(__SetGravity, ETHScriptWrapper::SetGravity);
asDECLARE_FUNCTION_WRAPPER(__GetGravity, ETHScriptWrapper::GetGravity);

asDECLARE_FUNCTION_WRAPPER(__SetNumIterations, ETHScriptWrapper::SetNumIterations);
asDECLARE_FUNCTION_WRAPPER(__GetNumIterations, ETHScriptWrapper::GetNumIterations);

asDECLARE_FUNCTION_WRAPPER(__SetTimeStepScale, ETHScriptWrapper::SetTimeStepScale);
asDECLARE_FUNCTION_WRAPPER(__GetTimeStepScale, ETHScriptWrapper::GetTimeStepScale);

asDECLARE_FUNCTION_WRAPPERPR(__GetClosestContact,   ETHScriptWrapper::GetClosestContact, (const Vector2&, const Vector2&, Vector2&, Vector2&),                          ETHEntity*);
asDECLARE_FUNCTION_WRAPPERPR(__GetClosestContactEx, ETHScriptWrapper::GetClosestContact, (const Vector2&, const Vector2&, Vector2&, Vector2&, const str_type::string&), ETHEntity*);
asDECLARE_FUNCTION_WRAPPER(__GetContactEntities, ETHScriptWrapper::GetContactEntities);
asDECLARE_FUNCTION_WRAPPER(__DisableContact,     ETHScriptWrapper::DisableContact);

asDECLARE_FUNCTION_WRAPPER(__IsFixedTimeStep,				ETHScriptWrapper::IsFixedTimeStep);
asDECLARE_FUNCTION_WRAPPER(__GetFixedTimeStepValue,			ETHScriptWrapper::GetFixedTimeStepValue);
asDECLARE_FUNCTION_WRAPPER(__SetFixedTimeStep,				ETHScriptWrapper::SetFixedTimeStep);
asDECLARE_FUNCTION_WRAPPER(__SetFixedTimeStepValue,			ETHScriptWrapper::SetFixedTimeStepValue);
asDECLARE_FUNCTION_WRAPPER(__GetCurrentPhysicsTimeStepMS,	ETHScriptWrapper::GetCurrentPhysicsTimeStepMS);

asDECLARE_FUNCTION_WRAPPER(__SetFixedHeight, ETHScriptWrapper::SetFixedHeight);
asDECLARE_FUNCTION_WRAPPER(__SetFixedWidth,  ETHScriptWrapper::SetFixedWidth);
asDECLARE_FUNCTION_WRAPPER(__GetScale,       ETHScriptWrapper::GetScale);
asDECLARE_FUNCTION_WRAPPER(__SetScaleFactor, ETHScriptWrapper::SetScaleFactor);
asDECLARE_FUNCTION_WRAPPERPR(__ScaleF, ETHScriptWrapper::Scale, (const float),    float);
asDECLARE_FUNCTION_WRAPPERPR(__Scale2, ETHScriptWrapper::Scale, (const Vector2&), Vector2);
asDECLARE_FUNCTION_WRAPPERPR(__Scale3, ETHScriptWrapper::Scale, (const Vector3&), Vector3);

asDECLARE_FUNCTION_WRAPPER(__SetSharedData, ETHScriptWrapper::SetSharedData);
asDECLARE_FUNCTION_WRAPPER(__IsSharedDataConstant, ETHScriptWrapper::IsSharedDataConstant);
asDECLARE_FUNCTION_WRAPPER(__GetSharedData, ETHScriptWrapper::GetSharedData);
asDECLARE_FUNCTION_WRAPPER(__SharedDataExists, ETHScriptWrapper::SharedDataExists);
asDECLARE_FUNCTION_WRAPPER(__RemoveSharedData, ETHScriptWrapper::RemoveSharedData);

static void __GetCurrentBucket(asIScriptGeneric *gen)
{
	ETHRenderEntity *s = (ETHRenderEntity*)gen->GetObject();
	Vector2 r = s->GetCurrentBucket(ETHScriptWrapper::m_pScene->GetBucketManager());
	gen->SetReturnObject(&r);
}

static void __DeleteEntity(asIScriptGeneric *gen)
{
	ETHRenderEntity *s = (ETHRenderEntity*)gen->GetArgObject(0);
	s->AddRef();
	ETHEntity *r = ETHScriptWrapper::DeleteEntity(s);
	gen->SetReturnObject(r);
}

void ETHScriptWrapper::RegisterGlobalFunctions(asIScriptEngine *pASEngine)
{
	int r;

	// Register input
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetCursorPos()",                   asFUNCTION(__GetCursorPos),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "vector2 GetCursorAbsolutePos()",           asFUNCTION(__GetCursorAbsolutePos), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "void SetCursorPos(const vector2 &in pos)", asFUNCTION(__SetCursorPos),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHInput", "string GetLastCharInput() const",          asFUNCTION(__GetLastCharInput),     asCALL_GENERIC); assert(r >= 0);
	// ETHInput retriever
	r = pASEngine->RegisterGlobalFunction("ETHInput @GetInputHandle()",  asFUNCTION(__GetInputHandle), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void HideCursor(const bool)", asFUNCTION(__HideCursor),     asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPosition(const vector3 &in)",     asFUNCTION(__SetPosition),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPositionXY(const vector2 &in)",   asFUNCTION(__SetPositionXY),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPositionX(const float)",          asFUNCTION(__SetPositionX),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPositionY(const float)",          asFUNCTION(__SetPositionY),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void SetPositionZ(const float)",          asFUNCTION(__SetPositionZ),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPosition(const vector3 &in)",   asFUNCTION(__AddToPosition),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPositionXY(const vector2 &in)", asFUNCTION(__AddToPositionXY),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPositionX(const float)",        asFUNCTION(__AddToPositionX),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPositionY(const float)",        asFUNCTION(__AddToPositionY),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void AddToPositionZ(const float)",        asFUNCTION(__AddToPositionZ),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "vector2 GetCurrentBucket() const",        asFUNCTION(__GetCurrentBucket),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void PlayParticleSystem(const uint)",     asFUNCTION(__PlayParticleSystem), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterObjectMethod("ETHEntity", "void ResolveJoints()",                    asFUNCTION(__ResolveEntityJoints), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("ETHEntity @SeekEntity(const string &in)", asFUNCTION(__SeekEntityStr), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("ETHEntity @SeekEntity(const int)",        asFUNCTION(__SeekEntityInt), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(__Print),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void print(const float)",      asFUNCTION(__PrintFloat), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void print(const int)",        asFUNCTION(__PrintInt),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void print(const uint)",       asFUNCTION(__PrintUInt),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in)", asFUNCTION(__LoadScene1Arg), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in onSceneUpdate = \"\")",                                   asFUNCTION(__LoadSceneSSSArgs), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in, const string &in)",                                      asFUNCTION(__LoadSceneSSSSArgs), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in, const vector2 &in)",                                     asFUNCTION(__LoadSceneSSSVArgs), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in, const string &in, const vector2 &in)",                   asFUNCTION(__LoadSceneSSSSVArgs), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void LoadScene(const string &in, const string &in, const string &in, const string &in, const string &in, const vector2 &in)", asFUNCTION(__LoadSceneSSSSSVArgs), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SaveScene(const string &in)",														                   asFUNCTION(__SaveScene),      asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("float GetTimeF()",                  asFUNCTION(__GetTimeF),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetTime()",                    asFUNCTION(__GetTime),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float UnitsPerSecond(const float)", asFUNCTION(__UnitsPerSecond), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void Exit()",                       asFUNCTION(__Exit),           asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetLastFrameElapsedTime()",    asFUNCTION(__GetLastFrameElapsedTime), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, const float angle = 0.0f)", asFUNCTION(__AddEntityA), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, ETHEntity@ &out)",    asFUNCTION(__AddEntityR), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, const string &in)",   asFUNCTION(__AddEntityN), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddScaledEntity(const string &in, const vector3 &in, const float)",  asFUNCTION(__AddEntityS), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddScaledEntity(const string &in, const vector3 &in, const float, ETHEntity@ &out)", asFUNCTION(__AddEntitySR), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int AddEntity(const string &in, const vector3 &in, const float, ETHEntity@ &out, const string &in, const float)", asFUNCTION(__AddEntityF), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("ETHEntity@ DeleteEntity(ETHEntity@)",										  asFUNCTION(__DeleteEntity),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GenerateLightmaps()",													  asFUNCTION(__GenerateLightmaps), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void AddLight(const vector3 &in, const vector3 &in, const float, const bool)", asFUNCTION(__AddLight),          asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("int rand(const int)",                   asFUNCTION(__RandomizerInt),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int rand(const int, const int)",        asFUNCTION(__RandomizerInt2Args),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float randF(const float)",              asFUNCTION(__RandomizerFloat),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float randF(const float, const float)", asFUNCTION(__RandomizerFloat2Args), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetAmbientLight(const vector3 &in)",   asFUNCTION(__SetAmbientLight),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector3 GetAmbientLight()",                 asFUNCTION(__GetAmbientLight),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetParallaxOrigin(const vector2 &in)", asFUNCTION(__SetParallaxOrigin), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetParallaxIntensity(const float)",    asFUNCTION(__SetParallaxIntensity), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetParallaxOrigin()",               asFUNCTION(__GetParallaxOrigin),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetParallaxIntensity()",              asFUNCTION(__GetParallaxIntensity), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetParallaxVerticalIntensity(const float)", asFUNCTION(__SetParallaxVerticalIntensity), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetParallaxVerticalIntensity()",           asFUNCTION(__GetParallaxVerticalIntensity), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetBucketClearenceFactor(const float)", asFUNCTION(__SetBucketClearenceFactor), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetBucketClearenceFactor()",           asFUNCTION(__GetBucketClearenceFactor), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetZAxisDirection(const vector2 &in)", asFUNCTION(__SetZAxisDirection), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetZAxisDirection()",               asFUNCTION(__GetZAxisDirection), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetWindowProperties(const string &in, const uint, const uint, const bool, const bool, const PIXEL_FORMAT)", asFUNCTION(__SetWindowProperties), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetCameraPos(const vector2 &in)",   asFUNCTION(__SetCameraPos),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void AddToCameraPos(const vector2 &in)", asFUNCTION(__AddToCameraPos),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetCameraPos()",                 asFUNCTION(__GetCameraPos),    asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void DrawText(const vector2 &in, const string &in, const string &in, const uint color = 0xFFFFFFFF, const float scale = 1.0f)",	asFUNCTION(__DrawText),			asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawFadingText(const vector2 &in, const string &in, const string &in, const uint, const uint, const float scale = 1.0f)",	asFUNCTION(__DrawFadingText),	asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 ComputeCarretPosition(const string &in, const string &in, const uint pos)",	asFUNCTION(__ComputeCarretPosition),asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 ComputeTextBoxSize(const string &in, const string &in)",						asFUNCTION(__ComputeTextBoxSize),	asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string AssembleColorCode(const uint)",												asFUNCTION(__AssembleColorCode),	asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void EnableLightmaps(const bool)", asFUNCTION(__EnableLightmaps), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void UsePixelShaders(const bool)", asFUNCTION(__UsePixelShaders), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetFPSRate()",               asFUNCTION(__GetFPSRate),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void EnablePreLoadedLightmapsFromFile(const bool)", asFUNCTION(__EnablePreLoadedLightmapsFromFile), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("bool LoadMusic(const string &in)",                    asFUNCTION(__LoadMusic),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool LoadSoundEffect(const string &in)",              asFUNCTION(__LoadSoundEffect), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool PlaySample(const string &in)",                   asFUNCTION(__PlaySample),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool LoopSample(const string &in, const bool)",       asFUNCTION(__LoopSample),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool StopSample(const string &in)",                   asFUNCTION(__StopSample),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool PauseSample(const string &in)",                  asFUNCTION(__PauseSample),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SetSampleVolume(const string &in, const float)", asFUNCTION(__SetSampleVolume), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SetSamplePan(const string &in, const float)",    asFUNCTION(__SetSamplePan),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SetSampleSpeed(const string &in, const float)",  asFUNCTION(__SetSampleSpeed),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SampleExists(const string &in)",                 asFUNCTION(__SampleExists),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsSamplePlaying(const string &in)",              asFUNCTION(__IsSamplePlaying), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetGlobalVolume(const float)",                   asFUNCTION(__SetGlobalVolume), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetGlobalVolume()",							 asFUNCTION(__GetGlobalVolume), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("uint GetNumEntities()", asFUNCTION(__GetNumEntities), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("bool AddFloatData(const string &in, const string &in, const float)",         asFUNCTION(__AddFloatData),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddIntData(const string &in, const string &in, const int)",             asFUNCTION(__AddIntData),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddUIntData(const string &in, const string &in, const uint)",           asFUNCTION(__AddUIntData),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddStringData(const string &in, const string &in, const string &in)",   asFUNCTION(__AddStringData),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddVector2Data(const string &in, const string &in, const vector2 &in)", asFUNCTION(__AddVector2Data), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool AddVector3Data(const string &in, const string &in, const vector3 &in)", asFUNCTION(__AddVector3Data), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("vector3 normalize(const vector3 &in)", asFUNCTION(__normalize3), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 normalize(const vector2 &in)", asFUNCTION(__normalize2), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("float radianToDegree(const float)",                             asFUNCTION(__radianToDegree), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float degreeToRadian(const float)",                             asFUNCTION(__degreeToRadian), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint ARGB(const uint8, const uint8, const uint8, const uint8)", asFUNCTION(__ARGB),           asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void LoadSprite(const string &in)",    asFUNCTION(__LoadSprite),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool ReleaseSprite(const string &in)", asFUNCTION(__ReleaseSprite), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void DrawSprite(const string &in, const vector2 &in, const uint color = 0xFFFFFFFF, const float angle = 0.0f)",                      asFUNCTION(__DrawSprite),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawShapedSprite(const string &in, const vector2 &in, const vector2 &in, const uint color = 0xFFFFFFFF, const float angle = 0.0f)", asFUNCTION(__DrawShapedSprite), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawSprite(const string &in, const vector2 &in, const float, const vector3 &in, const float angle = 0.0f)",                      asFUNCTION(__DrawSpriteEx),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawShapedSprite(const string &in, const vector2 &in, const vector2 &in, const float, const vector3 &in, const float angle = 0.0f)", asFUNCTION(__DrawShapedSpriteEx), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetSpriteSize(const string &in)",                                                                  asFUNCTION(__GetSpriteSize),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawRectangle(const vector2 &in, const vector2 &in, const uint, const uint, const uint, const uint)", asFUNCTION(__DrawRectangle),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DrawLine(const vector2 &in, const vector2 &in, const uint, const uint, const float)",                 asFUNCTION(__DrawLine),         asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void PlayParticleEffect(const string &in, const vector2 &in, const float, const float)", asFUNCTION(__PlayParticleEffect), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("vector2 GetSpriteFrameSize(const string &in)",                    asFUNCTION(__GetSpriteFrameSize), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetupSpriteRects(const string &in, const uint, const uint)", asFUNCTION(__SetupSpriteRects),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetSpriteRect(const string &in, const uint)",                asFUNCTION(__SetSpriteRect),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetSpriteOrigin(const string &in, const vector2 &in)",       asFUNCTION(__SetSpriteOrigin),    asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetSpriteFlipX(const string &in, const bool)",       asFUNCTION(__SetSpriteFlipX),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetSpriteFlipY(const string &in, const bool)",       asFUNCTION(__SetSpriteFlipY),    asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("string GetSceneFileName()",           asFUNCTION(__GetSceneFileName),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetScreenSize()",             asFUNCTION(__GetScreenSize),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool Windowed()",                     asFUNCTION(__Windowed),            asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetVideoModeCount()",            asFUNCTION(__GetVideoModeCount),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("videoMode GetVideoMode(const uint)",  asFUNCTION(__GetVideoMode),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint GetBackgroundColor()",           asFUNCTION(__GetBackgroundColor),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetBackgroundColor(const uint)", asFUNCTION(__SetBackgroundColor),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetSystemScreenSize()",       asFUNCTION(__GetSystemScreenSize), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void GetEntityArray(const string &in, ETHEntityArray &)",                       asFUNCTION(__GetEntityArray),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetEntitiesFromBucket(const vector2 &in, ETHEntityArray &)",               asFUNCTION(__GetEntitiesFromBucket),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetVisibleEntities(ETHEntityArray &)",                                     asFUNCTION(__GetVisibleEntities),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetEntitiesAroundBucket(const vector2 &in, ETHEntityArray &)",             asFUNCTION(__GetEntitiesAroundBucket), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetIntersectingEntities(const vector2 &in, ETHEntityArray &, const bool)", asFUNCTION(__GetIntersectingEntities), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetBucket(const vector2 &in)",                                          asFUNCTION(__GetBucket),               asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetAllEntitiesInScene(ETHEntityArray &)",                                  asFUNCTION(__GetAllEntitiesInScene),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void ForceEntityRendering(ETHEntity@)",                                         asFUNCTION(__ForceEntityRendering),    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetEntitiesAroundBucket(const vector2 &in, ETHEntityArray &, const string &in)",              asFUNCTION(__GetWhiteListedEntitiesAroundBucket), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetEntitiesAroundBucketWithBlackList(const vector2 &in, ETHEntityArray &, const string &in)", asFUNCTION(__GetEntitiesAroundBucketWithBlackList), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetEntitiesAroundEntity(ETHEntity@, ETHEntityArray &)",                    asFUNCTION(__GetEntitiesAroundEntity), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("bool IsPixelShaderSupported()",            asFUNCTION(__IsPixelShaderSupported),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetPositionRoundUp(const bool)",      asFUNCTION(__SetPositionRoundUp),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetPositionRoundUp()",                asFUNCTION(__GetPositionRoundUp),      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void EnableQuitKeys(const bool)",          asFUNCTION(__EnableQuitKeys),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void EnableRealTimeShadows(const bool)",   asFUNCTION(__EnableRealTimeShadows),   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetBorderBucketsDrawing(const bool)", asFUNCTION(__SetBorderBucketsDrawing), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsDrawingBorderBuckets()",            asFUNCTION(__IsDrawingBorderBuckets),  asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("string GetResourceDirectory()",              asFUNCTION(__GetResourceDirectory),          asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetExternalStorageDirectory()",       asFUNCTION(__GetExternalStoragePath),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetGlobalExternalStorageDirectory()", asFUNCTION(__GetGlobalExternalStoragePath),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetAbsolutePath(const string &in)",   asFUNCTION(__GetAbsolutePath),               asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetLastCameraPos()",                 asFUNCTION(__GetLastCameraPos),              asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int GetNumRenderedEntities()",               asFUNCTION(__GetNumRenderedEntities),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int parseInt(const string &in)",             asFUNCTION(__ParseInt),                      asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint parseUInt(const string &in)",           asFUNCTION(__ParseUInt),                     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float parseFloat(const string &in)",         asFUNCTION(__ParseFloat),                    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("uint distanceUTF8(const string &in)",        asFUNCTION(__DistanceUTF8),                  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool isValidUTF8(const string &in)",         asFUNCTION(__IsValidUTF8),                   asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("int GetArgc()",                              asFUNCTION(__GetArgc),                       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetArgv(const int)",                  asFUNCTION(__GetArgv),                       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetWorldSpaceCursorPos2()",          asFUNCTION(__GetWorldSpaceCursorPos2),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void ForwardCommand(const string &in)",      asFUNCTION(__ForwardCommand),                asCALL_GENERIC); assert(r >= 0);

	#ifdef ETH_DEFINE_DEPRECATED_SIGNATURES_FROM_0_9_5
	{
		r = pASEngine->RegisterGlobalFunction("string GetExternalStoragePath()",          asFUNCTION(__GetExternalStoragePath),        asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterGlobalFunction("string GetGlobalExternalStoragePath()",    asFUNCTION(__GetGlobalExternalStoragePath),  asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterGlobalFunction("string GetProgramPath()",                  asFUNCTION(__GetResourceDirectory),          asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterGlobalFunction("int ParseInt(const string &in)",           asFUNCTION(__ParseInt),                      asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterGlobalFunction("uint ParseUInt(const string &in)",         asFUNCTION(__ParseUInt),                     asCALL_GENERIC); assert(r >= 0);
		r = pASEngine->RegisterGlobalFunction("float ParseFloat(const string &in)",       asFUNCTION(__ParseFloat),                    asCALL_GENERIC); assert(r >= 0);
	}
	#endif

	r = pASEngine->RegisterGlobalFunction("void SetZBuffer(const bool)", asFUNCTION(__SetZBuffer), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetZBuffer()",           asFUNCTION(__GetZBuffer), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetPersistentResources(const bool)",  asFUNCTION(__SetPersistentResources),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool ArePersistentResourcesEnabled()",     asFUNCTION(__ArePersistentResourcesEnabled), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void ReleaseResources()",                  asFUNCTION(__ReleaseResources),              asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void ResolveJoints()",                     asFUNCTION(__ResolveJoints),                 asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetFastGarbageCollector(const bool)", asFUNCTION(__SetFastGarbageCollector),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsHighEndDevice()",                   asFUNCTION(__IsHighEndDevice),               asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetPlatformName()",                 asFUNCTION(__GetPlatformName),               asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("bool EnablePackLoading(const string &in, const string &in password = \"\")", asFUNCTION(__EnablePackLoading),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DisablePackLoading()",         asFUNCTION(__DisablePackLoading),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsResourcePackingSupported()", asFUNCTION(__IsResourcePackingSupported), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsPackLoadingEnabled()",       asFUNCTION(__IsPackLoadingEnabled),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool EnableLightmapsFromExpansionPack(const bool)", asFUNCTION(__EnableLightmapsFromExpansionPack), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("string GetStringFromFileInPackage(const string &in)", asFUNCTION(__GetStringFromFileInPackage), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool FileInPackageExists(const string &in)",          asFUNCTION(__FileInPackageExists),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool FileExists(const string &in)",                   asFUNCTION(__FileExists),                 asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetGravity(const vector2 &in)", asFUNCTION(__SetGravity), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 GetGravity()",               asFUNCTION(__GetGravity), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetNumIterations(const int, const int)", asFUNCTION(__SetNumIterations), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void GetNumIterations(int &out, int &out)",   asFUNCTION(__GetNumIterations), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetTimeStepScale(const float)", asFUNCTION(__SetTimeStepScale), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetTimeStepScale()",           asFUNCTION(__GetTimeStepScale), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void DisableContact()",              asFUNCTION(__DisableContact),   asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("ETHEntity@ GetClosestContact(const vector2 &in, const vector2 &in, vector2 &out, vector2 &out)", asFUNCTION(__GetClosestContact),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("ETHEntity@ GetClosestContact(const vector2 &in, const vector2 &in, vector2 &out, vector2 &out, const string &in)", asFUNCTION(__GetClosestContactEx),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool GetContactEntities(const vector2 &in, const vector2 &in, ETHEntityArray@)",                 asFUNCTION(__GetContactEntities), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("bool IsFixedTimeStep()",					 asFUNCTION(__IsFixedTimeStep),			    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetFixedTimeStepValue()",			 asFUNCTION(__GetFixedTimeStepValue),	    asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetFixedTimeStep(const bool)",		 asFUNCTION(__SetFixedTimeStep),			asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetFixedTimeStepValue(const float)", asFUNCTION(__SetFixedTimeStepValue),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetCurrentPhysicsTimeStepMS()",	 asFUNCTION(__GetCurrentPhysicsTimeStepMS), asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("void SetFixedHeight(const float)", asFUNCTION(__SetFixedHeight), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetFixedWidth(const float)",  asFUNCTION(__SetFixedWidth),  asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float GetScale()",                 asFUNCTION(__GetScale),       asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("float Scale(const float)",         asFUNCTION(__ScaleF),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("void SetScaleFactor(const float)", asFUNCTION(__SetScaleFactor), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector2 Scale(const vector2 &in)", asFUNCTION(__Scale2),         asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("vector3 Scale(const vector3 &in)", asFUNCTION(__Scale3),         asCALL_GENERIC); assert(r >= 0);

	r = pASEngine->RegisterGlobalFunction("bool SetSharedData(const string &in, const string &in)", asFUNCTION(__SetSharedData),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool IsSharedDataConstant(const string &in)",            asFUNCTION(__IsSharedDataConstant), asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("string GetSharedData(const string &in)",                 asFUNCTION(__GetSharedData),        asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool SharedDataExists(const string &in)",                asFUNCTION(__SharedDataExists),     asCALL_GENERIC); assert(r >= 0);
	r = pASEngine->RegisterGlobalFunction("bool RemoveSharedData(const string &in)",                asFUNCTION(__RemoveSharedData),     asCALL_GENERIC); assert(r >= 0);

	m_math.RegisterGlobals(pASEngine);
}
