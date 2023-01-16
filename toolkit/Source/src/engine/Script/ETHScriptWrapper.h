#ifndef ETH_SCRIPT_WRAPPER_H_
#define ETH_SCRIPT_WRAPPER_H_

#include <cstdint>

#include "../Scene/ETHScene.h"

#include "../Util/ETHInput.h"

#include "../Entity/ETHEntityCache.h"

#include "../Drawing/ETHDrawableManager.h"

#include "../Shader/ETHBackBufferTargetManager.h"

#include "../../angelscript/include/angelscript.h"

class ETHScriptWrapper
{
	static std::string m_sceneFileName;
	static bool m_runningMainFunction;
	static bool m_persistentResources;
	static float m_lastFrameElapsedTime;
	
protected:
	static void SetLastFrameElapsedTime(const float lastFrameElapsedTime);

	enum GARBAGE_COLLECT_MODE
	{
		ONE_STEP = 0, DESTROY_ALL_GARBAGE = 1, FULL_CYCLE = 2
	};

	static GARBAGE_COLLECT_MODE m_gcMode;

public:
	struct ETH_VIDEO_MODE_POD
	{
		uint32_t width, height;
		Texture::PIXEL_FORMAT pf;
	};

	struct ETH_VIDEO_MODE : public ETH_VIDEO_MODE_POD
	{
		ETH_VIDEO_MODE()
		{
			width = height = 0;
			pf = Texture::PF_UNKNOWN;
		}
	};

	static ETHResourceProviderPtr m_provider;

	static bool IsRunningMainFunction() { return m_runningMainFunction; }

	static std::string GetSceneFileName();
	static ETHDrawableManager m_drawableManager;
	static ETHScenePtr m_pScene;
	static bool m_abort;
	static bool m_highEndDevice;
	static int32_t m_argc;
	static char **m_argv;
	static ETHInput m_ethInput;
	static asIScriptModule *m_pASModule;
	static asIScriptEngine *m_pASEngine;
	static Vector2 m_v2LastCamPos;
	static void LoadSceneScripts();
	static Vector2 GetLastCameraPos();
	static ETHBackBufferTargetManagerPtr m_backBuffer;

	static asIScriptContext *m_pScriptContext;
	static asIScriptFunction* m_onSceneUpdateFunction;
	static asIScriptFunction* m_onResumeFunction;

	static ETHEntityCache m_entityCache;

	class ETH_NEXT_SCENE
	{
		std::string sceneName;
		std::string onSceneUpdateFunc;
		std::string onSceneLoadedFunc;
		std::string onResumeFunc;
		std::string lightmapDirectory;
		Vector2 bucketSize;

	public:
		ETH_NEXT_SCENE();
		std::string GetSceneName() const;
		std::string GetOnSceneLoadedFunc() const;
		std::string GetOnSceneUpdateFunc() const;
		std::string GetOnResumeFunc() const;
		std::string GetLightmapDirectory() const;
		Vector2 GetBucketSize() const;

		void SetNextScene(
			const std::string& sceneName,
			const std::string& onSceneLoadedFunc,
			const std::string& onSceneUpdateFunc,
			const std::string& onResumeFunc,
			const std::string& lightmapDirectory,
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
		static int32_t Sign(const int32_t v);
		static float Distance(const Vector2 &a, const Vector2 &b);
		static float Distance(const Vector3 &a, const Vector3 &b);

		// Encryption‎ functions
		static std::string GetHashFromString(const std::string& type, const std::string& str);
		static std::string GetMD5HashFromString(const std::string& str);
		static std::string GetSHA1HashFromString(const std::string& str);
	} m_math;

	void RegisterGlobalFunctions(asIScriptEngine *pASEngine);
	static bool RunMainFunction(asIScriptFunction* mainFunc);
	static bool WarnIfRunsInMainFunction(const std::string &functionName);

	static void LoadSceneInScript(const std::string &escFile);

	static void LoadSceneInScript(
		const std::string &escFile,
		const std::string &onSceneLoadedFunc,
		const std::string &onSceneUpdateFunc);

	static void LoadSceneInScript(
		const std::string &escFile,
		const std::string &onSceneLoadedFunc,
		const std::string &onSceneUpdateFunc,
		const Vector2& v2BucketSize);

	static void LoadSceneInScript(
		const std::string& escFile,
		const std::string& onSceneLoadedFunc,
		const std::string& onSceneUpdateFunc,
		const std::string& onResumeFunc);

	static void LoadSceneInScript(
		const std::string& escFile,
		const std::string& onSceneLoadedFunc,
		const std::string& onSceneUpdateFunc,
		const std::string& onResumeFunc,
		const Vector2& v2BucketSize);

	static void LoadSceneInScript(
		const std::string& escFile,
		const std::string& onSceneLoadedFunc,
		const std::string& onSceneUpdateFunc,
		const std::string& onResumeFunc,
		const std::string &lightmapDirectory,
		const Vector2& v2BucketSize);

	static void AddSceneInScript(const std::string& escFile, const Vector3& offset, ETHEntityArray &outVector, const bool immediatelyLoadSprites);
	static void AddSceneFromString(const std::string& content, const Vector3& offset, ETHEntityArray &outVector, const bool immediatelyLoadSprites);

	static bool SaveScene(const std::string &escFile);
	static bool LoadScene(const std::string &escFile, const std::string& lightmapDirectory, const Vector2& bucketSize);

	/// temporarily store the names of the next functions to load them after
	/// the script is finished
	static ETH_NEXT_SCENE m_nextScene;

	/// Shows a message to the programmer or user. If the message is ETH_ERROR, the program will be aborted
	static void ShowMessage(std::string message, const ETH_MESSAGE type, const bool abort = true);
	static void ForwardCommand(const std::string& cmd);

	// ETHEntity methods wrappers
	static void SetPosition(ETHEntity *pEntity, const Vector3 &v3Pos);
	static void SetPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos);
	static void AddToPosition(ETHEntity *pEntity, const Vector3 &v3Pos);
	static void AddToPositionXY(ETHEntity *pEntity, const Vector2 &v2Pos);
	static Vector2 GetCurrentBucket(ETHEntity *pEntity);
	static void PlayParticleSystem(ETHEntity *pEntity, const uint32_t n);

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
	static std::string GetLastCharInput();
	static bool SetCursorPos(const Vector2 &v2Pos);
	static float GetTimeF();
	static unsigned long GetTime();
	static float UnitsPerSecond(const float speed);
	static void Exit();
	static void SetAmbientLight(const Vector3 &v3Color);
	static void SetParallaxOrigin(const Vector2 &parallaxOrigin);
	static Vector2 GetParallaxOrigin();
	static void SetParallaxIntensity(const float intensity);
	static float GetParallaxIntensity();
	static void SetBucketClearenceFactor(const float factor);
	static float GetBucketClearenceFactor();
	static Vector3 GetAmbientLight();
	static uint32_t GetScreenWidth();
	static uint32_t GetScreenHeight();
	static Vector2 GetScreenSize();
	static Vector2 GetScreenSizeInPixels();
	static void SetCameraPos(const Vector2 &v2Pos);
	static void AddToCameraPos(const Vector2 &v2Add);

	static Vector2 GetCameraPos();
	static float GetFPSRate();
	static Vector2 ComputeCarretPosition(const std::string &font, const std::string &text, const uint32_t pos);
	static Vector2 ComputeTextBoxSize(const std::string &font, const std::string &text);
	static void DrawText(const Vector2 &v2Pos, const std::string &text, const std::string &font, const uint32_t color, const float scale);
	static std::string AssembleColorCode(const uint32_t color);
	static void DrawFadingText(const Vector2 &v2Pos, const std::string &text, const std::string &font, const uint32_t color, unsigned long time, const float scale);
	static int32_t AddEntity(const std::string &file, const Vector3 &v3Pos, const float angle, ETHEntity **ppOutEntity, const std::string &alternativeName, const float scale);
	static int32_t AddEntity(const std::string &file, const Vector3 &v3Pos, const float angle);
	static int32_t AddScaledEntity(const std::string &file, const Vector3 &v3Pos, const float scale);
	static int32_t AddScaledEntity(const std::string &file, const Vector3 &v3Pos, const float scale, ETHEntity **ppOutEntity);
	static int32_t AddEntity(const std::string &file, const Vector3 &v3Pos, ETHEntity **ppOutEntity);
	static int32_t AddEntity(const std::string &file, const Vector3 &v3Pos, const std::string &alternativeName);
	static void DeleteEntity(ETHEntity *pEntity);
	static void LoadLightmaps(const std::string& directory);
	static ETHEntity *SeekEntity(const int32_t id);
	static ETHEntity *SeekEntity(const std::string &name);
	static bool LoadMusic(const std::string &file);
	static bool LoadSoundEffect(const std::string &file);
	static bool PlaySample(const std::string &file);
	static bool LoopSample(const std::string &file, const bool loop);
	static bool StopSample(const std::string &file);
	static bool PauseSample(const std::string &file);
	static bool SetSampleVolume(const std::string &file, const float volume);
	static bool SetSamplePan(const std::string &file, const float pan);
	static bool SetSampleSpeed(const std::string &file, const float speed);
	static bool SampleExists(const std::string &file);
	static bool IsSamplePlaying(const std::string &file);
	static void SetGlobalVolume(const float volume);
	static float GetGlobalVolume();
	static uint32_t GetNumEntities();

	static bool AddFloatData(const std::string &entity, const std::string &name, const float value);
	static bool AddIntData(const std::string &entity, const std::string &name, const int32_t value);
	static bool AddUIntData(const std::string &entity, const std::string &name, const uint32_t value);
	static bool AddStringData(const std::string &entity, const std::string &name, const std::string &value);
	static bool AddVector2Data(const std::string &entity, const std::string &name, const Vector2 &value);
	static bool AddVector3Data(const std::string &entity, const std::string &name, const Vector3 &value);

	static SpritePtr LoadAndGetSprite(const std::string &name);
	static const ETHGraphicResourceManager::SpriteResource* LoadAndGetResource(const std::string &name);
	static void DrawShapedFromResource(const ETHGraphicResourceManager::SpriteResource* resource, const Vector2 &v2Pos, const Vector2 &v2Size, const Vector4 &color, const float angle);
	static void LoadSprite(const std::string& name);
	static bool ReleaseSprite(const std::string& name);
	static void DrawSprite(const std::string &name, const Vector2 &v2Pos, const uint32_t color, const float angle);
	static void DrawShaped(const std::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const uint32_t color, const float angle);
	static void DrawSprite(const std::string &name, const Vector2 &v2Pos, const float alpha, const Vector3 &color, const float angle);
	static void DrawShaped(const std::string &name, const Vector2 &v2Pos, const Vector2 &v2Size, const float alpha, const Vector3 &color, const float angle);
	static void PlayParticleEffect(const std::string& fileName, const Vector2& pos, const float angle, const float scale);
	static Vector2 GetSpriteSize(const std::string &name);
	static Vector2 GetSpriteFrameSize(const std::string& name);
	static void SetupSpriteRects(const std::string& name, const uint32_t columns, const uint32_t rows);
	static void SetSpriteRect(const std::string& name, const uint32_t frame);
	static void SetSpriteOrigin(const std::string& name, const Vector2& origin);
	static void SetSpriteFlipX(const std::string& name, const bool flip);
	static void SetSpriteFlipY(const std::string& name, const bool flip);
	static bool Windowed();
	static ETH_VIDEO_MODE GetVideoMode(const uint32_t mode);
	static uint32_t GetVideoModeCount();

	static uint32_t GetBackgroundColor();
	static void SetBackgroundColor(const uint32_t color);
	static Vector2 GetSystemScreenSize();
	static void GetEntityArrayByName(const std::string &name, ETHEntityArray &outVector);
	static void GetEntityArrayFromBucket(const Vector2 &v2Bucket, ETHEntityArray &outVector);
	static void GetEntitiesFromBucket(const Vector2 &v2Bucket, ETHEntityArray& outVector, const std::string& semicolonSeparatedNames);
	static void GetAllEntitiesInScene(ETHEntityArray &outVector);
	static void GetEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector);
	static void GetEntitiesAroundEntity(ETHEntity* entity, ETHEntityArray &outVector);
	static void GetWhiteListedEntitiesAroundBucket(const Vector2& bucket, ETHEntityArray &outVector, const std::string& semicolonSeparatedNames);
	static void GetEntitiesAroundBucketWithBlackList(const Vector2& bucket, ETHEntityArray &outVector, const std::string& semicolonSeparatedNames);
	static Vector2 GetBucket(const Vector2 &v2);
	static void GetVisibleEntities(ETHEntityArray &entityArray);
	static void GetIntersectingEntities(const Vector2 &v2Here, ETHEntityArray &outVector, const bool screenSpace);
	static int32_t GetNumProcessedEntities();
	static int32_t GetNumRenderedPieces();
	static void SetBorderBucketsDrawing(const bool enable);
	static bool IsDrawingBorderBuckets();
	static int32_t GetArgc();
	static std::string GetArgv(const int32_t n);
	static Vector2 GetWorldSpaceCursorPos2();
	static void SetPersistentResources(const bool enable);
	static bool ArePersistentResourcesEnabled();
	static void ReleaseResources();
	static void ResolveJoints();
	static void ForceEntityRendering(ETHEntity *entity);

	static void SetZAxisDirection(const Vector2& dir);
	static Vector2 GetZAxisDirection();

	static void GarbageCollect(const GARBAGE_COLLECT_MODE mode, asIScriptEngine* engine);
	static void SetFastGarbageCollector(const bool enable);
	static std::string GetCurrentCallstack();

	static void SetHighEndDevice(const bool highEnd);
	static bool IsHighEndDevice();
	static std::string GetPlatformName();

	static void Abort();

	static std::string GetResourceDirectory();
	static std::string GetExternalStorageDirectory();
	static std::string GetGlobalExternalStorageDirectory();
	static std::string GetAbsolutePath(const std::string &fileName);

	static ETHInput *GetInputHandle();

	static void ResetVideoMode(
		const std::string& winTitle,
		const uint32_t width,
		const uint32_t height,
		const bool windowed,
		const bool sync,
		const Texture::PIXEL_FORMAT gsPF);

	static void SetAppDefaultVideoMode(const Vector2& size, const bool windowed);
	static Vector2 GetAppDefaultVideoMode(bool& windowed);

	static void Print(const std::string &str);
	static void PrintFloat(const float f);
	static void PrintInt(const int32_t n);
	static void PrintUInt(const uint32_t n);
	static unsigned long GetLastFrameElapsedTime();
	static float GetLastFrameElapsedTimeF();
	static std::string GetStringFromFileInPackage(const std::string& fileName);
	static bool FileInPackageExists(const std::string& fileName);
	static bool FileExists(const std::string& fileName);

	static void SetGravity(const Vector2& gravity);
	static Vector2 GetGravity();
	static void SetNumIterations(const int32_t velocityIterations, const int32_t positionIterations);
	static void GetNumIterations(int32_t& velocityIterations, int32_t& positionIterations);
	static void SetTimeStepScale(const float scale);
	static float GetTimeStepScale();
	static bool IsFixedTimeStep();
	static float GetFixedTimeStepValue();
	static void SetFixedTimeStep(const bool enable);
	static void SetFixedTimeStepValue(const float value);
	static float GetCurrentPhysicsTimeStepMS();
	static void DisableContact();

	static ETHEntity* GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal);
	static ETHEntity* GetClosestContact(const Vector2& a, const Vector2& b, Vector2& point, Vector2& normal, const std::string& semicolonSeparatedIgnoreList);
	static bool GetContactEntities(const Vector2& a, const Vector2& b, ETHEntityArray& entities);

	static void SetFixedHeight(const float height);
	static void UpdateFixedHeight();
	static float GetDummyScale();
	static float DummyScale(const float v);
	static Vector2 DummyScale(const Vector2& v);
	static Vector3 DummyScale(const Vector3& v);

	// SharedData
	static bool SetSharedData(const std::string& key, const std::string& data);
	static bool IsSharedDataConstant(const std::string& key);
	static std::string GetSharedData(const std::string& key);
	static bool SharedDataExists(const std::string& key);
	static bool RemoveSharedData(const std::string& key);

	// Not to the script
	static bool Aborted();
	static ETHResourceProviderPtr GetProvider();
	static void CreateDynamicBackBuffer(const ETHAppEnmlFile& file);

	static void SetArgc(const int32_t argc);
	static void SetArgv(char **argv);
};

#endif
