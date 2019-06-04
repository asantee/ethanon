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

#include <string>
#include <sstream>
#include <jni.h>
#include <signal.h>

#include <BaseApplication.h>

#include <Platform/android/Platform.android.h>
#include <Platform/android/AndroidFileIOHub.h>

#include <Input/Android/AndroidInput.h>

#include "../../../../engine/ETHEngine.h"
#include "../../../../engine/Resource/ETHDirectories.h"

using namespace gs2d;
using namespace gs2d::math;

extern "C" {
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_mainLoop(JNIEnv* env, jobject thiz, jstring inputStr);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_resize(JNIEnv* env, jobject thiz, jint width, jint height);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_restore(JNIEnv* env, jobject thiz);
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

std::string  g_inputStr;

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_start(
	JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height)
{
	jboolean isCopy;
	const char* strApk = env->GetStringUTFChars(apkPath, &isCopy);
	const char* strExt = env->GetStringUTFChars(externalPath, &isCopy);
	const char* strGlo = env->GetStringUTFChars(globalPath, &isCopy);
	zip = boost::shared_ptr<Platform::ZipFileManager>(new Platform::ZipFileManager(strApk));
	Platform::FileIOHubPtr fileIOHub(new Platform::AndroidFileIOHub(zip, strExt, strGlo, ETHDirectories::GetBitmapFontDirectory()));

	video = CreateVideo(width, height, fileIOHub);
	input = CreateInput(&g_inputStr, true);
	audio = CreateAudio(0);

	video->SetCameraPos(lastCameraPos);

	video->ResetVideoMode(width, height, Texture::PF_DEFAULT, false);

	splashSprite = video->CreateSprite(GS_L("assets/data/splash.png"));
	splashSprite->SetOrigin(gs2d::Sprite::EO_CENTER);

	cogSprite = video->CreateSprite(GS_L("assets/data/cog.png"));
	cogSprite->SetOrigin(gs2d::Sprite::EO_CENTER);

	if (!application)
	{
		BaseApplicationPtr newApplication = CreateBaseApplication(false /*autoStartScriptEngine*/);
		newApplication->Start(video, input, audio);
		application = newApplication;
		engine = ETHEngine::Cast(application);
	}
	else
	{
		application->Start(video, input, audio);
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
	}
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_restore(JNIEnv* env, jobject thiz)
{
	if (application)
	{
		application->Restore();
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

str_type::string AssembleCommands()
{
	str_type::stringstream ss;
	try
	{
		ss
		<< boost::any_cast<str_type::string>(video->GetGraphicContext())
		<< boost::static_pointer_cast<AndroidInput>(input)->PullCommands();

		return ss.str();
	}
	catch (const boost::bad_any_cast& e)
	{
		video->Message(GS_L("Invalid type for command assembling"), GSMT_ERROR);
		return GS_L("");
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
	audio->Update();

	if (IsScriptEngineLoaded())
	{
		// Do default main loop
		const float lastFrameElapsedTime = ComputeElapsedTimeF(video);
		application->Update(Min(1000.0f, lastFrameElapsedTime));

		lastCameraPos = video->GetCameraPos();

		application->RenderFrame();

	}
	else
	{
		// Draw SPLASH SCREEN
		video->BeginSpriteScene(gs2d::constant::BLACK);
		if (splashSprite)
		{
			const Vector2 screenSize(video->GetScreenSizeF());
			const float scale = (screenSize.y / 720.0f) * 0.8f;

			splashSprite->Draw(screenSize * 0.5f, gs2d::constant::WHITE, 0.0f, Vector2(scale, scale));

			const Vector2 cogMiddle(screenSize.x * 0.5f, screenSize.y * 0.8f);
			static float angle = 0.0f;
			cogSprite->Draw(cogMiddle - Vector2(32.0f, 0.0f), gs2d::Color(0xE0FFFFFF),-angle + 24.0f, Vector2(0.6f, 0.6f));
			cogSprite->Draw(cogMiddle + Vector2(32.0f, 0.0f), gs2d::Color(0xE0FFFFFF),-angle - 24.0f, Vector2(0.6f, 0.6f));
			cogSprite->Draw(cogMiddle, gs2d::Color(0xFFFFFFFF), angle, Vector2(0.6f, 0.6f));
			angle -= 4.0f;
		}
		video->EndSpriteScene();
	}

	return env->NewStringUTF(AssembleCommands().c_str());
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_destroy(JNIEnv* env, jobject thiz)
{
	if (IsScriptEngineLoaded())
	{
		application->Destroy();
	}

	if (video && audio)
	{
		video->Message(GS_L("Application resources destroyed"), GSMT_INFO);
	}
	return env->NewStringUTF(GS_L(""));
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
