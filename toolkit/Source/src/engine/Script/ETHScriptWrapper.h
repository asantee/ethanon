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

#ifndef ETH_SCRIPT_WRAPPER_H_
#define ETH_SCRIPT_WRAPPER_H_

#include "../Scene/ETHScene.h"
#include "../ETHPrimitiveDrawer.h"
#include "../../angelscript/include/angelscript.h"
#include "../Util/ETHInput.h"
#include "../Entity/ETHEntityCache.h"
#include "../Shader/ETHBackBufferTargetManager.h"

class ETHScriptWrapper
{
	static str_type::string m_sceneFileName;
	static bool m_roundUpPosition;
	static bool m_runningMainFunction;
	static bool m_persistentResources;
	static unsigned long m_lastFrameElapsedTime;

protected:
	static void SetLastFrameElapsedTime(const unsigned long lastFrameElapsedTime);

	enum GARBAGE_COLLECT_MODE
	{
		ONE_STEP = 0, DESTROY_ALL_GARBAGE = 1, FULL_CYCLE = 2
	};
	static GARBAGE_COLLECT_MODE m_gcMode;
	static int GetFunctionId(asIScriptModule* pModule, const str_type::string& name);

public:
	static ETHResourceProviderPtr m_provider;

	//static Vector2 GetBGMin() { return m_v2BGMin; }
	//static Vector2 GetBGMax() { return m_v2BGMax; }
	//static str_type::string GetBGBitmapName() { return m_bgBimapName; }
	static bool IsRoundingUpPosition() { return m_roundUpPosition; }
	static bool IsRunningMainFunction() { return m_runningMainFunction; }

	static str_type::string GetSceneFileName();
	static ETHSpeedTimer m_timer;
	static std::list<boost::shared_ptr<ETHPrimitiveDrawer> > m_primitiveList;
	static ETHScenePtr m_pScene;
	static bool m_abort;
	static bool m_useLightmaps;
	static bool m_richLighting;
	static bool m_highEndDevice;
	static int m_argc;
	static str_type::char_t **m_argv;
	static ETHInput m_ethInput;
	static asIScriptModule *m_pASModule;
	static asIScriptEngine *m_pASEngine;
	static Vector2 m_v2LastCamPos;
	static void LoadSceneScripts();
	static Vector2 GetLastCameraPos();
	static ETHBackBufferTargetManagerPtr m_backBuffer;

	static asIScriptContext *m_pScriptContext;
	static asIScriptContext *m_pConstructorContext;
	static int m_onSceneUpdateFunctionId;
	static int m_onResumeFunctionId;

	static ETHEntityCache m_entityCache;

	class ETH_NEXT_SCENE
	{
		str_type::string sceneName;
		str_type::string onSceneUpdateFunc;
		str_type::string onSceneLoadedFunc;
		str_type::string onResumeFunc;
		Vector2 bucketSize;

	public:
		ETH_NEXT_SCENE();
		str_type::string GetSceneName() const;
		str_type::string GetOnSceneLoadedFunc() const;
		str_type::string GetOnSceneUpdateFunc() const;
		str_type::string GetOnResumeFunc() const;
		Vector2 GetBucketSize() const;
		void SetNextScene(const str_type::string& sceneName, const str_type::string& onSceneLoadedFunc,
						  const str_type::string& onSceneUpdateFunc,  const str_type::string& onResumeFunc,
						  const Vector2& bucketSize);
		void Reset();
		bool HasNextScene() const;
	};

	static class Math
	{
	public:
		static void RegisterGlobals(asIScriptEngine *pASEngine);

		static float Length(const Vector3 &v);
		static float Length(const Vector2 &v);
		static float Sign(const float v);
		static int Sign(const int v);
		static float Distance(const Vector2 &a, const Vector2 &b);
		static float Distance(const Vector3 &a, const Vector3 &b);
	} m_math;

	void RegisterGlobalFunctions(asIScriptEngine *pASEngine);
	static bool RunMainFunction(const int mainFuncId);
	static bool WarnIfRunsInMainFunction(const str_type::string &functionName);

	static void LoadSceneInScript(const str_type::string &escFile);

	static void LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc, const str_type::string &onSceneUpdateFunc);

	static void LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc, const str_type::string &onSceneUpdateFunc, const Vector2& v2BucketSize);

	static void LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
								  const str_type::string &onSceneUpdateFunc, const str_type::string &onResumeFunc);

	static void LoadSceneInScript(const str_type::string &escFile, const str_type::string &onSceneLoadedFunc,
								  const str_type::string &onSceneUpdateFunc, const str_type::string &onResumeFunc,
								  const Vector2 &v2BucketSize);
	static bool SaveScene(const str_type::string &escFile);
	static bool LoadScene(const str_type::string &escFile, const Vector2& bucketSize);

	/// temporarily store the names of the next functions to load them after
	/// the script is finished
	static ETH_NEXT_SCENE m_nextScene;

	/// Shows a message to the programmer or user. If the message is ETH_ERROR, the program will be aborted
	static void ShowMessage(str_type::string message, const ETH_MESSAGE type, const bool abort = true);
	static void ForwardCommand(const str_type::string& cmd);

	// ETHEntity methods wrappers
	static void SetPosition(ETHEntity *pEntity, const Vector3 &v3Pos);
	static void SetPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos);
	static void AddToPosition(ETHEntity *pEntity, const Vector3 &v3Pos);
	static void AddToPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos);
	static Vector2 GetCurrentBucket(ETHEntity *pEntity);
	static Vector2 GetScreenRectMin(ETHEntity *pEntity);
	static Vector2 GetScreenRectMax(ETHEntity *pEntity);
	static void PlayParticleSystem(ETHEntity *pEntity, const unsigned int n);

	static void SetPositionX(ETHEntity *pEntity, const float v);
	static void SetPositionY(ETHEntity *pEntity, const float v);
	static void SetPositionZ(ETHEntity *pEntity, const float v);
	static void AddToPositionX(ETHEntity *pEntity, const float v);
	static void AddToPositionY(ETHEntity *pEntity, const float v);
	static void AddToPositionZ(ETHEntity *pEntity, const float v);

	static void ResolveEntityJoints(ETHEntity* pEntity);

	// global wraps 
	static void HideCursor(const bool hide);
	static Vector2 GetCursorPos();
	static Vector2 GetCursorAbsolutePos();
	static str_type::string GetLastCharInput();
	static bool SetCursorPos(const Vector2 &v2Pos);
	static float GetTimeF();
	static unsigned long GetTime();
	static float UnitsPerSecond(const float speed);
	static void Exit();
	static void SetAmbientLight(const Vector3 &v3Color);
	static void SetParallaxOrigin(const Vector2 &parallaxOrigin);
	static void SetParallaxIntensity(const float intensity);
	static Vector3 GetAmbientLight();
	static unsigned int GetScreenWidth();
	static unsigned int GetScreenHeight();
	static Vector2 GetScreenSize();
	static void SetCameraPos(const Vector2 &v2Pos);
	static void AddToCameraPos(const Vector2 &v2Add);
	static void EnableLightmaps(const bool enable);
	static Vector2 GetCameraPos();
	static float GetFPSRate();
	static Vector2 ComputeCarretPosition(const str_type::string &font, const str_type::string &text, const unsigned int pos);
	static Vector2 ComputeTextBoxSize(const str_type::string &font, const str_type::string &text);
	static void DrawText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const GS_DWORD color, const float scale);
	static void DrawFadingText(const Vector2 &v2Pos, const str_type::string &text, const str_type::string &font, const GS_DWORD color, unsigned long time, const float scale);
	static void AddLight(const Vector3 &v3Pos, const Vector3 &v3Color, const float range, const bool castShadows);
	static int AddEntity(const str_type::string &file, const Vector3 &v3Pos, const float angle, ETHEntity **ppOutEntity, const str_type::string &alternativeName, const float scale);
	static int AddEntity(const str_type::string &file, const Vector3 &v3Pos, const float angle);
	static int AddScaledEntity(const str_type::string &file, const Vector3 &v3Pos, const float scale);
	static int AddScaledEntity(const str_type::string &file, const Vector3 &v3Pos, const float scale, ETHEntity **ppOutEntity);
	static int AddEntity(const str_type::string &file, const Vector3 &v3Pos, ETHEntity **ppOutEntity);
	static int AddEntity(const str_type::string &file, const Vector3 &v3Pos, const str_type::string &alternativeName);
	static ETHEntity *DeleteEntity(ETHEntity *pEntity);
	static bool GenerateLightmaps();
	static ETHEntity *SeekEntity(const int id);
	static ETHEntity *SeekEntity(const str_type::string &name);
	static bool LoadMusic(const str_type::string &file);
	static bool LoadSoundEffect(const str_type::string &file);
	static bool PlaySample(const str_type::string &file);
	static bool LoopSample(const str_type::string &file, const bool loop);
	static bool StopSample(const str_type::string &file);
	static bool PauseSample(const str_type::string &file);
	static bool SetSampleVolume(const str_type::string &file, const float volume);
	static bool SetSamplePan(const str_type::string &file, const float pan);
	static bool SetSampleSpeed(const str_type::string &file, const float speed);
	static bool SampleExists(const str_type::string &file);
	static bool IsSamplePlaying(const str_type::string &file);
	static void SetGlobalVolume(const float volume);
	static float GetGlobalVolume();
	static unsigned int GetNumEntities();
	static int GetLastID();
	static bool AddFloatData(const str_type::string &entity, const str_type::string &name, const float value);
	static bool AddIntData(const str_type::string &entity, const str_type::string &name, const int value);
	static bool AddUIntData(const str_type::string &entity, const str_type::string &name, const unsigned int value);
	static bool AddStringData(const str_type::string &entity, const str_type::string &name, const str_type::string &value);
	static bool AddVector2Data(const str_type::string &entity, const str_type::string &name, const Vector2 &value);
	static bool AddVector3Data(const str_type::string &entity, const str_type::string &name, const Vector3 &value);
	static float GetMaxHeight();
	static float GetMinHeight();
	static SpritePtr LoadAndGetSprite(const str_type::string &name);
	static void LoadSprite(const str_type::string &name);
	static void DrawSprite(const str_type::string &name, const Vector2 &v2Pos, const GS_DWORD color, const float angle);
	static void DrawShaped(const str_type::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle);
	static void DrawSpriteZ(const str_type::string &name, const float depth, const Vector2 &v2Pos, const GS_DWORD color, const float angle);
	static void DrawShapedZ(const str_type::string &name, const float depth, const Vector2 &v2Pos, const Vector2 &v2Size, const GS_DWORD color, const float angle);
	static Vector2 GetSpriteSize(const str_type::string &name);
	static Vector2 GetSpriteFrameSize(const str_type::string& name);
	static void SetupSpriteRects(const str_type::string& name, const unsigned int columns, const unsigned int rows);
	static void SetSpriteRect(const str_type::string& name, const unsigned int frame);
	static void SetSpriteOrigin(const str_type::string& name, const Vector2& origin);
	static void DrawLine(const Vector2 &v2A, const Vector2 &v2B, const GS_COLOR a, const GS_COLOR b, const float width);
	// static void PlayCutscene(const str_type::string &fileName);
	static bool Windowed();
	static ETH_VIDEO_MODE GetVideoMode(const unsigned int mode);
	static unsigned int GetVideoModeCount();
	static void DrawRectangle(const Vector2 &v2Pos, const Vector2 &v2Size,
							  const GS_DWORD color0, const GS_DWORD color1,
							  const GS_DWORD color2, const GS_DWORD color3);
	static GS_DWORD GetBackgroundColor();
	static void SetBackgroundColor(const GS_DWORD color);
	//static bool SetBackgroundImage(const str_type::string &name);
	//static void PositionBackgroundImage(const Vector2 &v2Min, const Vector2 &v2Max);
	static Vector2 GetSystemScreenSize();
	static void GetEntityArrayByName(const str_type::string &name, ETHEntityArray &outVector);
	static void GetEntityArrayFromBucket(const Vector2 &v2Bucket, ETHEntityArray &outVector);
	static void GetAllEntitiesInScene(ETHEntityArray &outVector);
	static void GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector);
	static void GetWhiteListedEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector, const str_type::string& semicolonSeparatedNames);
	static void GetEntitiesAroundBucketWithBlackList(const Vector2& bucket, ETHEntityArray &outVector, const str_type::string& semicolonSeparatedNames);
	static Vector2 GetBucket(const Vector2 &v2);
	static void SetPositionRoundUp(const bool roundUp);
	static bool GetPositionRoundUp();
	static void SetZBuffer(const bool enable);
	static bool GetZBuffer();
	static void SetHaloRotation(const bool enable);
	static void EnableQuitKeys(const bool enable);
	static void EnableRealTimeShadows(const bool enable);
	static void GetVisibleEntities(ETHEntityArray &entityArray);
	static void GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace);
	static int GetNumRenderedEntities();
	static void SetBorderBucketsDrawing(const bool enable);
	static bool IsDrawingBorderBuckets();
	static int GetArgc();
	static str_type::string GetArgv(const int n);
	static Vector2 GetWorldSpaceCursorPos2();
	static Vector3 GetWorldSpaceCursorPos3();
	//static void SetBackgroundAlphaAdd();
	//static void SetBackgroundAlphaPixel();
	//static void SetBackgroundAlphaModulate();
	static void UsePixelShaders(const bool enable);
	static bool IsPixelShaderSupported();
	static void SetPersistentResources(const bool enable);
	static bool IsPersistentResources();
	static void ReleaseResources();
	static void ResolveJoints();
	static void ForceEntityRendering(ETHEntity *entity);

	static void SetZAxisDirection(const Vector2& dir);
	static Vector2 GetZAxisDirection();

	static void GarbageCollect(const GARBAGE_COLLECT_MODE mode, asIScriptEngine* engine);
	static void SetFastGarbageCollector(const bool enable);

	static void SetHighEndDevice(const bool highEnd);
	static bool IsHighEndDevice();
	static str_type::string GetPlatformName();

	static void Abort();

	static str_type::string GetProgramPath();
	static str_type::string GetExternalStoragePath();
	static str_type::string GetGlobalExternalStoragePath();
	static str_type::string GetAbsolutePath(const str_type::string &fileName);

	static ETHInput *GetInputHandle();
	static void SetWindowProperties(const str_type::string &winTitle, const unsigned int width, const unsigned int height, const bool windowed, const bool sync, const GS_PIXEL_FORMAT gsPF);

	static void Print(const str_type::string &str);
	static void PrintFloat(const float f);
	static void PrintInt(const int n);
	static void PrintUInt(const unsigned int n);
	static unsigned long GetLastFrameElapsedTime();
	static str_type::string GetStringFromFileInPackage(const str_type::string& fileName);
	static bool FileInPackageExists(const str_type::string& fileName);
	static bool FileExists(const str_type::string& fileName);

	static void SetGravity(const Vector2& gravity);
	static Vector2 GetGravity();
	static void SetNumIterations(const int velocityIterations, const int positionIterations);
	static void GetNumIterations(int& velocityIterations, int& positionIterations);
	static void SetTimeStepScale(const float scale);
	static float GetTimeStepScale();
	static bool IsFixedTimeStep();
	static float GetFixedTimeStepValue();
	static void SetFixedTimeStep(const bool enable);
	static void SetFixedTimeStepValue(const float value);
	static float GetCurrentPhysicsTimeStepMS();

	static ETHEntity* GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal);
	static ETHEntity* GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal, const str_type::string& semicolonSeparatedIgnoreList);
	static bool GetContactEntities(const Vector2& a, const Vector2& b, ETHEntityArray& entities);

	static void SetFixedHeight(const float height);
	static void SetFixedWidth(const float width);
	static float GetScale();
	static float Scale(const float v);
	static Vector2 Scale(const Vector2& v);
	static Vector3 Scale(const Vector3& v);
	static void SetScaleFactor(const float v);
	static void ScaleEntities();

	// Not to the script
	static bool Aborted();
	static ETHResourceProviderPtr GetProvider();
	static void CreateDynamicBackBuffer(const str_type::string& startResourcePath);
};

#endif