#include <string>
#include <sstream>
#include <jni.h>
#include <signal.h>
#include <algorithm>
#include <mutex>

#include <BaseApplication.h>

#include <Platform/android/Platform.android.h>
#include <Platform/android/AndroidFileIOHub.h>
#include <Platform/android/AndroidZipFileManager.h>

#include <Input/Android/AndroidInput.h>

#include <Video/GLES2/android/AndroidGLES2Video.h>

#include "../../../../engine/ETHEngine.h"
#include "../../../../engine/Resource/ETHDirectories.h"

using namespace gs2d;
using namespace gs2d::math;

extern "C" {
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_mainLoop(JNIEnv* env, jobject thiz, jstring inputStr);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_resize(JNIEnv* env, jobject thiz, jint width, jint height);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_restore(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_resume(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_audioSuspend(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_audioResume(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_audioMute(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_audioUnmute(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_audioSetSoundEffectVolume(JNIEnv* env, jobject thiz, jfloat volume);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_preInit(JNIEnv* env, jobject thiz, jstring apkPath);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_start(JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_engineStartup(JNIEnv* env, jobject thiz);
	JNIEXPORT jboolean JNICALL Java_net_asantee_gs2d_GS2DJNI_isLoading(JNIEnv* env, jobject thiz);
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_destroy(JNIEnv* env, jobject thiz);
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_runOnUIThread(JNIEnv* env, jobject thiz, jstring inputStr);

	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_getSharedData(JNIEnv* env, jobject thiz, jstring key, jstring defaultValue);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_setSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_setSecuredSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value);
	JNIEXPORT jboolean JNICALL Java_net_asantee_gs2d_GS2DJNI_isSharedDataValid(JNIEnv* env, jobject thiz, jstring key);
};

BaseApplicationPtr application;

VideoPtr video;
InputPtr input;
AudioPtr audio = 0;
boost::shared_ptr<Platform::AndroidZipFileManager> zip;
std::string zipPath;
SpritePtr splashSprite, cogSprite, barSprite;
ETHEnginePtr engine;
float predictedLoadingTimeMs = 10000.0f;
float startLoadingTimeMs = 0.0f;
bool engineLoaded = false;
Vector2 lastCameraPos(0.0f, 0.0f);
Color lastBackgroundColor(0xFF000000);

void ReleaseLoadingSprite(SpritePtr& sprite)
{
	if (sprite)
	{
		sprite->GetTexture()->Free();
		sprite = SpritePtr();
	}
}

void CreateLoadingSprite(VideoPtr video, SpritePtr& sprite, const std::string& filePath)
{
	sprite = SpritePtr(new Sprite(video.get(), filePath));
	sprite->SetOrigin(Vector2(0.5f));
}

// The APK zip and the FMOD device are created by GS2DJNI.preInit on a background thread started in
// Activity.onCreate, so the GL thread no longer spends its first frames on libzip's directory read and on
// FMOD's output setup (AAudio/OpenSL binder calls into the audio server, seconds on some devices), which the
// UI thread used to wait for. start() still creates whatever preInit has not, waiting for it if it is running.
static std::mutex g_preInitMutex; // serializes EnsureZip/EnsureAudio between preInit and start
static std::mutex g_audioMutex; // 'audio' is assigned on the preInit or GL thread and read on the UI thread
static bool g_audioSuspendedOnce = false; // UI thread only

static AudioPtr GetAudio()
{
	std::lock_guard<std::mutex> lock(g_audioMutex);
	return audio;
}

// call with g_preInitMutex held
static void EnsureZip(const std::string& apkPath)
{
	if (zip && zip->IsLoaded() && zipPath == apkPath)
		return;

	zip = boost::shared_ptr<Platform::AndroidZipFileManager>(new Platform::AndroidZipFileManager(apkPath.c_str()));
	zipPath = apkPath;
}

// call with g_preInitMutex held
static void EnsureAudio()
{
	if (GetAudio())
		return;

	AudioPtr newAudio = CreateAudio(0);
	std::lock_guard<std::mutex> lock(g_audioMutex);
	audio = newAudio;
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_audioSuspend(JNIEnv* env, jobject thiz)
{
	AudioPtr currentAudio = GetAudio();
	if (currentAudio)
	{
		currentAudio->Suspend();
		g_audioSuspendedOnce = true;
	}
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_audioResume(JNIEnv* env, jobject thiz)
{
	// preInit can create the device before the first onStart, which used to find no device and skip the resume.
	// Keep it that way: FMOD expects mixerSuspend/mixerResume in pairs, so resume only after a suspend.
	AudioPtr currentAudio = GetAudio();
	if (currentAudio && g_audioSuspendedOnce)
		currentAudio->Resume();
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_audioMute(JNIEnv* env, jobject thiz)
{
	AudioPtr currentAudio = GetAudio();
	if (currentAudio)
		currentAudio->SetMute(true);
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_audioUnmute(JNIEnv* env, jobject thiz)
{
	AudioPtr currentAudio = GetAudio();
	if (currentAudio)
		currentAudio->SetMute(false);
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_audioSetSoundEffectVolume(JNIEnv* env, jobject thiz, jfloat volume)
{
	AudioPtr currentAudio = GetAudio();
	if (currentAudio)
		currentAudio->SetSoundEffectVolume(volume);
}

std::string g_inputStr;

// Copies a Java string and releases the JNI buffer. ART's GetStringUTFChars always allocates a new
// copy, so every call that is not paired with ReleaseStringUTFChars leaks it (mainLoop did, every frame).
static std::string JStringToStdString(JNIEnv* env, jstring str)
{
	if (str == NULL)
		return std::string();

	const char* chars = env->GetStringUTFChars(str, NULL);
	if (chars == NULL)
		return std::string(); // OutOfMemoryError pending in the JVM

	const std::string r(chars);
	env->ReleaseStringUTFChars(str, chars);
	return r;
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_preInit(JNIEnv* env, jobject thiz, jstring apkPath)
{
	const std::string strApk = JStringToStdString(env, apkPath);
	std::lock_guard<std::mutex> lock(g_preInitMutex);
	EnsureZip(strApk);
	EnsureAudio();
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_start(
	JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height)
{
	const std::string strApk = JStringToStdString(env, apkPath);
	const std::string strExt = JStringToStdString(env, externalPath);
	const std::string strGlo = JStringToStdString(env, globalPath);
	{
		std::lock_guard<std::mutex> lock(g_preInitMutex);
		if (application)
		{
			// surface re-created: open a new archive as start always did, so no libzip handle is shared with
			// threads that may still be reading through the previous one
			zip = boost::shared_ptr<Platform::AndroidZipFileManager>(new Platform::AndroidZipFileManager(strApk.c_str()));
			zipPath = strApk;
		}
		else
		{
			EnsureZip(strApk);
		}
		EnsureAudio();
	}
	Platform::FileIOHubPtr fileIOHub(new Platform::AndroidFileIOHub(zip, strExt, strGlo, ETHDirectories::GetBitmapFontDirectory()));

	video = VideoPtr(new AndroidGLES2Video(width, height, "Ethanon Engine", fileIOHub));
	input = CreateInput(true, &g_inputStr);

	video->SetBackgroundColor(lastBackgroundColor);
	video->SetCameraPos(lastCameraPos);

	video->ResetVideoMode(width, height, Texture::PF_DEFAULT, false);

	CreateLoadingSprite(video, splashSprite, "assets/data/splash.png");
	CreateLoadingSprite(video, cogSprite, "assets/data/cog.png");
	CreateLoadingSprite(video, barSprite, "assets/data/white_16.png");

	if (!application)
	{
		BaseApplicationPtr newApplication = CreateBaseApplication(false /*autoStartScriptEngine*/);
		newApplication->Start(video, input, audio);
		application = newApplication;
		engine = ETHEngine::Cast(application);

		startLoadingTimeMs = video->GetElapsedTimeF(gs2d::Application::TU_MILLISECONDS);

		// load lastLoading time if there is any
		const std::string lastLoadingTimeFileContent = enml::GetStringFromAnsiFile(ETHEngine::GetExternalStorageDirectory() + "lastLoadingTime");
		if (!lastLoadingTimeFileContent.empty())
		{
			predictedLoadingTimeMs = ETHGlobal::ParseFloat(lastLoadingTimeFileContent.c_str());
		}

		// sanity check
		predictedLoadingTimeMs = std::max(100.0f, predictedLoadingTimeMs);

		ETHResourceProvider::Log("predictedLoadingTimeMs = " + std::to_string(predictedLoadingTimeMs), Platform::Logger::LT_INFO);
	}
	else
	{
		engine->ReleaseSpriteResources();
		engine->Start(video, input, audio);
	}
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_engineStartup(JNIEnv* env, jobject thiz)
{
	engine->StartScriptEngine();
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_resize(JNIEnv* env, jobject thiz, jint width, jint height)
{
	if (video)
	{
		video->ResetVideoMode(width, height, Texture::PF_DEFAULT, false);

		if (engine)
		{
			engine->UpdateFixedHeight();
		}
	}
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_restore(JNIEnv* env, jobject thiz)
{
	if (application)
	{
		application->Restore();
	}
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_resume(JNIEnv* env, jobject thiz)
{
	if (engine)
	{
		engine->Resume();
	}
}

bool IsScriptEngineLoaded()
{
	if (engine)
	{
		return engine->IsScriptEngineLoaded();
	}
	else
	{
		return false;
	}
}

std::string AssembleCommands()
{
	std::stringstream ss;
	try
	{
		ss
		<< video->PullCommands()
		<< boost::static_pointer_cast<AndroidInput>(input)->PullCommands();

		return ss.str();
	}
	catch (const boost::bad_any_cast& e)
	{
		video->Message("Invalid type for command assembling", GSMT_ERROR);
		return ("");
	}
}

JNIEXPORT jboolean JNICALL Java_net_asantee_gs2d_GS2DJNI_isLoading(JNIEnv* env, jobject thiz)
{
	return !IsScriptEngineLoaded();
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_mainLoop(JNIEnv* env, jobject thiz, jstring inputStr)
{
	g_inputStr = JStringToStdString(env, inputStr);

	video->HandleEvents();
	AudioPtr currentAudio = GetAudio();
	if (currentAudio)
		currentAudio->Update();
	input->Update();

	if (IsScriptEngineLoaded())
	{
		// compute and save loading time
		if (!engineLoaded)
		{
			const float loadingTime = video->GetElapsedTimeF(gs2d::Application::TU_MILLISECONDS) - startLoadingTimeMs;
			enml::SaveStringToAnsiFile(ETHEngine::GetExternalStorageDirectory() + "lastLoadingTime", std::to_string(loadingTime));
			ETHResourceProvider::Log("lastLoadingTime saved as " + std::to_string(loadingTime), Platform::Logger::LT_INFO);
		}
		engineLoaded = true;

		// Do default main loop
		const float lastFrameElapsedTime = ComputeElapsedTimeF(video);
		application->Update(Min(1000.0f, lastFrameElapsedTime));

		lastCameraPos = video->GetCameraPos();
		lastBackgroundColor = video->GetBackgroundColor();

		application->RenderFrame();

	}
	else
	{
		// Draw SPLASH SCREEN
		video->BeginRendering(gs2d::constant::BLACK);
		if (splashSprite)
		{
			const Vector2 screenSize(video->GetScreenSizeF());
			const float scale = (screenSize.y / 720.0f) * 0.8f;

			splashSprite->Draw(Vector3(screenSize * 0.5f, 0.0f), scale, 0.0f, Rect2D());

			const Vector2 cogMiddle(screenSize.x * 0.5f, screenSize.y * 0.8f);
			static float angle = 0.0f;
			cogSprite->Draw(Vector3(cogMiddle - Vector2(32.0f, 0.0f), 0.0f), 0.6f,-angle + 24.0f, Color(0xFFCCCCCC), Rect2D());
			cogSprite->Draw(Vector3(cogMiddle + Vector2(32.0f, 0.0f), 0.0f), 0.6f,-angle - 24.0f, Color(0xFFCCCCCC), Rect2D());
			cogSprite->Draw(Vector3(cogMiddle, 0.0f), 0.6f, angle, Color(0xFFDDDDDD), Rect2D());
			angle -= 4.0f;

			const float maxBarWidth = 96.0f;
			const float barHeight = 4.0f;
			const float barNormPosY = 0.855f;
			const float barBias = std::min((video->GetElapsedTimeF(gs2d::Application::TU_MILLISECONDS) - startLoadingTimeMs) / predictedLoadingTimeMs, 1.0f);
			barSprite->Draw(
				Vector3(screenSize * Vector2(0.5f, barNormPosY), 0.0f),
				Vector2(maxBarWidth, barHeight),
				Vector2(0.5f),
				Color(0xFF333333),
				0.0f /*angle*/,
				Rect2D(),
				false /*flipX*/,
				false /*flipY*/,
				Sprite::GetDefaultShader());

			barSprite->Draw(
				Vector3(screenSize * Vector2(0.5f, barNormPosY), 0.0f),
				Vector2(barBias * maxBarWidth, barHeight),
				Vector2(0.5f),
				Color(0xFF777777),
				0.0f /*angle*/,
				Rect2D(),
				false /*flipX*/,
				false /*flipY*/,
				Sprite::GetDefaultShader());
		}
		video->EndRendering();
	}

	return env->NewStringUTF(AssembleCommands().c_str());
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_destroy(JNIEnv* env, jobject thiz)
{
	if (IsScriptEngineLoaded())
	{
		application->Destroy();
	}

	Sprite::Finish();
	ReleaseLoadingSprite(splashSprite);
	ReleaseLoadingSprite(cogSprite);
	ReleaseLoadingSprite(barSprite);

	if (video && audio)
	{
		video->Message("Application resources destroyed", GSMT_INFO);
	}
	return env->NewStringUTF("");
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_runOnUIThread(JNIEnv* env, jobject thiz, jstring inputStr)
{
	const std::string str = JStringToStdString(env, inputStr);
	const std::string outStr = application->RunOnUIThread(str);
	return env->NewStringUTF(outStr.c_str());
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_getSharedData(JNIEnv* env, jobject thiz, jstring key, jstring defaultValue)
{
	return env->NewStringUTF(Application::SharedData.Get(JStringToStdString(env, key), JStringToStdString(env, defaultValue)).c_str());
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_setSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value)
{
	Application::SharedData.Set(JStringToStdString(env, key), JStringToStdString(env, value));
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_setSecuredSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value)
{
	Application::SharedData.SetSecured(JStringToStdString(env, key), JStringToStdString(env, value));
}

JNIEXPORT jboolean JNICALL Java_net_asantee_gs2d_GS2DJNI_isSharedDataValid(JNIEnv* env, jobject thiz, jstring key)
{
	return Application::SharedData.IsValid(JStringToStdString(env, key));
}
