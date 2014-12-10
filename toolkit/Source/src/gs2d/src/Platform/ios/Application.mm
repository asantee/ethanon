//
//  Application.m
//  Application
//
//  Created by Andre Santee on 4/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#import "Application.h"

#import <UIKit/UIKit.h>

//#import <Platform/ios/Platform.ios.h>
//#import <Platform/FileLogger.h>
//#import <Input/iOS/IOSInput.h>
//#import <Audio/CocosDenshion/CDAudioContext.h>
//#import <Platform/ios/IOSNativeCommandListener.h>

#import <Platform/ios/IOSFileIOHub.h>

#import <Video.h>
#import <Audio.h>
#import <Input.h>

#import <BaseApplication.h>

static gs2d::VideoPtr video;
static gs2d::InputPtr input;
static gs2d::AudioPtr audio;
static gs2d::BaseApplicationPtr engine;

static gs2d::math::Vector2 GetScreenSize()
{
	float width;
	float height;
	UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
	if (( [[[UIDevice currentDevice] systemVersion] floatValue] < 8.0f)
		&& UIInterfaceOrientationIsLandscape(orientation))
	{
		width = [[UIScreen mainScreen] bounds].size.height;
		height = [[UIScreen mainScreen] bounds].size.width;
	}
	else
	{
		width = [[UIScreen mainScreen] bounds].size.width;
		height = [[UIScreen mainScreen] bounds].size.height;
	}
	
	return (gs2d::math::Vector2(width, height) * [[UIScreen mainScreen] scale]);
}

void ApplicationWrapper::Start()
{
	audio = gs2d::CreateAudio(0);
	input = gs2d::CreateInput(0, false);

	Platform::FileIOHubPtr fileIOHub(new Platform::IOSFileIOHub(GS_L("data/")));

	const gs2d::math::Vector2 size = GetScreenSize();
	video = gs2d::CreateVideo(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y), fileIOHub);

	engine = gs2d::CreateBaseApplication();
	engine->Start(video, input, audio);
}

void ApplicationWrapper::RenderFrame()
{
	if (video->HandleEvents() == gs2d::Application::APP_QUIT)
	{
		exit(0);
	}
	
	input->Update();

	const float elapsedTime = (gs2d::ComputeElapsedTimeF(video));
	engine->Update(gs2d::math::Min(1000.0f, elapsedTime));

	engine->RenderFrame();
}

void ApplicationWrapper::Destroy()
{
	engine->Destroy();
}
