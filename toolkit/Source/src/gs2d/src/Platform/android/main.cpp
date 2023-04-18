#include <string>
#include <sstream>
#include <jni.h>
#include <signal.h>

#include <BaseApplication.h>

#include <Platform/android/Platform.android.h>
#include <Platform/android/AndroidFileIOHub.h>

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
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_start(JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_engineStartup(JNIEnv* env, jobject thiz);
	JNIEXPORT jboolean JNICALL Java_net_asantee_gs2d_GS2DJNI_isLoading(JNIEnv* env, jobject thiz);
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_destroy(JNIEnv* env, jobject thiz);
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_runOnUIThread(JNIEnv* env, jobject thiz, jstring inputStr);

	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_getSharedData(JNIEnv* env, jobject thiz, jstring key);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_setSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_createConstSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value);
};

BaseApplicationPtr application;

VideoPtr video;
InputPtr input;
AudioPtr audio;
boost::shared_ptr<Platform::ZipFileManager> zip;
SpritePtr splashSprite, cogSprite;
ETHEnginePtr engine;
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

std::string g_inputStr;

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_start(
	JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height)
{
	jboolean isCopy;
	const char* strApk = env->GetStringUTFChars(apkPath, &isCopy);
	const char* strExt = env->GetStringUTFChars(externalPath, &isCopy);
	const char* strGlo = env->GetStringUTFChars(globalPath, &isCopy);
	zip = boost::shared_ptr<Platform::ZipFileManager>(new Platform::ZipFileManager(strApk));
	Platform::FileIOHubPtr fileIOHub(new Platform::AndroidFileIOHub(zip, strExt, strGlo, ETHDirectories::GetBitmapFontDirectory()));

	video = VideoPtr(new AndroidGLES2Video(width, height, "Ethanon Engine", fileIOHub));
	input = CreateInput(true, &g_inputStr);
	audio = CreateAudio(0);

	video->SetBackgroundColor(lastBackgroundColor);
	video->SetCameraPos(lastCameraPos);

	video->ResetVideoMode(width, height, Texture::PF_DEFAULT, false);

	CreateLoadingSprite(video, splashSprite, "assets/data/splash.png");
	CreateLoadingSprite(video, cogSprite, "assets/data/cog.png");

	if (!application)
	{
		BaseApplicationPtr newApplication = CreateBaseApplication(false /*autoStartScriptEngine*/);
		newApplication->Start(video, input, audio);
		application = newApplication;
		engine = ETHEngine::Cast(application);
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
		<< boost::any_cast<std::string>(audio->GetAudioContext())
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
	jboolean isCopy;
	g_inputStr = env->GetStringUTFChars(inputStr, &isCopy);

	video->HandleEvents();
	input->Update();

	if (IsScriptEngineLoaded())
	{
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
			cogSprite->Draw(Vector3(cogMiddle - Vector2(32.0f, 0.0f), 0.0f), 0.6f,-angle + 24.0f, Rect2D());
			cogSprite->Draw(Vector3(cogMiddle + Vector2(32.0f, 0.0f), 0.0f), 0.6f,-angle - 24.0f, Rect2D());
			cogSprite->Draw(Vector3(cogMiddle, 0.0f), 0.6f, angle, Rect2D());
			angle -= 4.0f;
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

	if (video && audio)
	{
		video->Message("Application resources destroyed", GSMT_INFO);
	}
	return env->NewStringUTF("");
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_runOnUIThread(JNIEnv* env, jobject thiz, jstring inputStr)
{
	jboolean isCopy;
	const std::string str = env->GetStringUTFChars(inputStr, &isCopy);
	const std::string outStr = application->RunOnUIThread(str);
	return env->NewStringUTF(outStr.c_str());
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_getSharedData(JNIEnv* env, jobject thiz, jstring key)
{
	jboolean isCopy;
	const char* cstrKey = env->GetStringUTFChars(key, &isCopy);
	return env->NewStringUTF(Application::SharedData.Get(cstrKey).c_str());
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_setSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value)
{
	jboolean isCopy;
	const char* cstrKey   = env->GetStringUTFChars(key, &isCopy);
	const char* cstrValue = env->GetStringUTFChars(value, &isCopy);
	Application::SharedData.Set(cstrKey, cstrValue);
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_createConstSharedData(JNIEnv* env, jobject thiz, jstring key, jstring value)
{
	jboolean isCopy;
	const char* cstrKey   = env->GetStringUTFChars(key, &isCopy);
	const char* cstrValue = env->GetStringUTFChars(value, &isCopy);
	Application::SharedData.Create(cstrKey, cstrValue, true);
}
