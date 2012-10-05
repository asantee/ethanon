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

#include <string>
#include <sstream>
#include <jni.h>
#include <signal.h>

#include <gs2dframework.h>
#include <Platform/android/Platform.android.h>
#include <Platform/android/AndroidFileIOHub.h>

#include "../../../../engine/Resource/ETHDirectories.h"

using namespace gs2d;
using namespace gs2d::math;

extern "C" {
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_mainLoop(JNIEnv* env, jobject thiz, jstring inputStr);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_resize(JNIEnv* env, jobject thiz, jint width, jint height);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_restore(JNIEnv* env, jobject thiz);
	JNIEXPORT void    JNICALL Java_net_asantee_gs2d_GS2DJNI_start(JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height);
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_destroy(JNIEnv* env, jobject thiz);
	JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_runOnUIThread(JNIEnv* env, jobject thiz, jstring inputStr);
};

BaseApplicationPtr application;

VideoPtr video;
InputPtr input;
AudioPtr audio;
boost::shared_ptr<Platform::ZipFileManager> zip;
SpritePtr splashSprite;

std::string  g_inputStr;
static float g_globalVolume = 1.0f;
static bool g_splashShown = false;

float ComputeSplashScale(const Vector2& screenSize)
{
	const float defaultScale = 1.0f;
	const Vector2 size(splashSprite->GetBitmapSizeF() * defaultScale);
	if (size.x > screenSize.x)
	{
		return screenSize.x / size.x;
	}
	else
	{
		return defaultScale;
	}
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_start(
	JNIEnv* env, jobject thiz, jstring apkPath, jstring externalPath, jstring globalPath, jint width, jint height)
{
	g_splashShown = false;

	jboolean isCopy;
	const char* strApk = env->GetStringUTFChars(apkPath, &isCopy);
	const char* strExt = env->GetStringUTFChars(externalPath, &isCopy);
	const char* strGlo = env->GetStringUTFChars(globalPath, &isCopy);
	zip = boost::shared_ptr<Platform::ZipFileManager>(new Platform::ZipFileManager(strApk));
	Platform::FileIOHubPtr fileIOHub(new Platform::AndroidFileIOHub(zip, strExt, strGlo, ETHDirectories::GetBitmapFontDirectory()));

	video = CreateVideo(width, height, fileIOHub);
	input = CreateInput(&g_inputStr, true);
	audio = CreateAudio(0);

	video->ResetVideoMode(width, height, GSPF_DEFAULT, false);
	audio->SetGlobalVolume(g_globalVolume);

	splashSprite = video->CreateSprite(GS_L("assets/data/splash.bmp"));

	// if the application is already initialized, let's reset the device
	if (application)
	{
		application->Start(video, input, audio);
	}
}

static void StartApplication()
{
	if (!application)
		application = CreateBaseApplication();
	application->Start(video, input, audio);
}

static void DrawSplashScreen()
{
	video->BeginSpriteScene(gs2d::constant::BLACK);
	if (splashSprite)
	{
		splashSprite->SetOrigin(GSEO_CENTER);
		const Vector2 screenSize(video->GetScreenSizeF());
		const float scale = ComputeSplashScale(screenSize);
		splashSprite->Draw(screenSize * 0.5f, gs2d::constant::WHITE, 0.0f, Vector2(scale, scale));
	}
	video->EndSpriteScene();
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_resize(JNIEnv* env, jobject thiz, jint width, jint height)
{
	video->ResetVideoMode(width, height, GSPF_DEFAULT, false);
}

JNIEXPORT void JNICALL Java_net_asantee_gs2d_GS2DJNI_restore(JNIEnv* env, jobject thiz)
{
	if (application)
	{
		application->Restore();
	}
}

str_type::string AssembleCommands()
{
	str_type::stringstream ss;
	try
	{
		ss
		<< boost::any_cast<str_type::string>(audio->GetAudioContext())
		<< boost::any_cast<str_type::string>(video->GetGraphicContext());
		return ss.str();
	}
	catch (const boost::bad_any_cast& e)
	{
		video->Message(GS_L("Invalid type for command assembling"), GSMT_ERROR);
		return GS_L("");
	}
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_mainLoop(JNIEnv* env, jobject thiz, jstring inputStr)
{
	jboolean isCopy;
	g_inputStr = env->GetStringUTFChars(inputStr, &isCopy);

	video->HandleEvents();
	input->Update();

	// if the splash screen has already been shown, do the regular engine loop
	if (g_splashShown)
	{
		// if the engine hasn't been started yet (which means the previous frame was the splash screen frame),
		// start the engine machine before performing the regular loop
		if (!application)
		{
			StartApplication();
		}
		application->Update(Min(static_cast<unsigned long>(1000), ComputeElapsedTime(video)));
		application->RenderFrame();
	}
	else
	{
		// draw the splash screen and prepare the engine start
		DrawSplashScreen();
		g_splashShown = true;
	}

	return env->NewStringUTF(AssembleCommands().c_str());
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_destroy(JNIEnv* env, jobject thiz)
{
	application->Destroy();
	video->Message(GS_L("Application resources destroyed"), GSMT_INFO);
	g_globalVolume = audio->GetGlobalVolume();
	return env->NewStringUTF(GS_L(""));
}

JNIEXPORT jstring JNICALL Java_net_asantee_gs2d_GS2DJNI_runOnUIThread(JNIEnv* env, jobject thiz, jstring inputStr)
{
	jboolean isCopy;
	const std::string str = env->GetStringUTFChars(inputStr, &isCopy);
	const std::string outStr = application->RunOnUIThread(str);
	return env->NewStringUTF(outStr.c_str());
}
