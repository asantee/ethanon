//
//  Application.m
//  Application
//
//  Created by Andre Santee on 4/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#import "Application.h"

//#import <Platform/ios/Platform.ios.h>
//#import <Platform/FileLogger.h>
//#import <Audio/CocosDenshion/CDAudioContext.h>
//#import <Platform/ios/IOSNativeCommandListener.h>

#import <Platform/ios/IOSFileIOHub.h>

#import <Video.h>
#import <Audio.h>
#import <Input.h>

#import <BaseApplication.h>

#import <Input/iOS/IOSInput.h>

static gs2d::VideoPtr g_video;
static gs2d::InputPtr g_input;
static gs2d::AudioPtr g_audio;
static gs2d::BaseApplicationPtr g_engine;

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

ApplicationWrapper::ApplicationWrapper() : m_pixelDensity(1.0f)
{
	// setup default subplatform
	const bool constant = false;
	gs2d::Application::SharedData.Create("com.ethanonengine.subplatform", "apple", constant);

	// setup language code
	NSString *language = [[NSLocale preferredLanguages] objectAtIndex:0];
	gs2d::Application::SharedData.Create(GS_L("ethanon.system.language"), [language cStringUsingEncoding:1], true);
}

void ApplicationWrapper::Start()
{
	g_audio = gs2d::CreateAudio(0);
	g_input = gs2d::CreateInput(0, false);

	Platform::FileIOHubPtr fileIOHub(new Platform::IOSFileIOHub(GS_L("data/")));

	const gs2d::math::Vector2 size = GetScreenSize();
	g_video = gs2d::CreateVideo(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y), fileIOHub);

	g_engine = gs2d::CreateBaseApplication();
	g_engine->Start(g_video, g_input, g_audio);

	m_pixelDensity = [[UIScreen mainScreen] scale];
}

void ApplicationWrapper::RenderFrame()
{
	if (g_video->HandleEvents() == gs2d::Application::APP_QUIT)
	{
		exit(0);
	}
	
	g_input->Update();

	const float elapsedTime = (gs2d::ComputeElapsedTimeF(g_video));
	g_engine->Update(gs2d::math::Min(1000.0f, elapsedTime));

	g_engine->RenderFrame();
}

void ApplicationWrapper::Destroy()
{
	g_engine->Destroy();
}

void ApplicationWrapper::TouchesBegan(UIView* thisView, NSSet* touches, UIEvent* event)
{
	for (UITouch *touch in touches)
	{
		gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(g_input.get());
		for (std::size_t t = 0; t < g_input->GetMaxTouchCount(); t++)
		{
			if (input->GetTouchPos(t, g_video) == gs2d::GS_NO_TOUCH)
			{
				CGPoint location = [touch locationInView: thisView];
				input->SetCurrentTouchPos(t, gs2d::math::Vector2(location.x, location.y) * m_pixelDensity);
				break;
			}
		}
	}
}

void ApplicationWrapper::TouchesMoved(UIView* thisView, NSSet* touches, UIEvent* event)
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(g_input.get());
	for (UITouch *touch in touches)
	{
		CGPoint prevLocation = [touch previousLocationInView: thisView];
		const gs2d::math::Vector2 prevLocationV2(gs2d::math::Vector2(prevLocation.x, prevLocation.y) * m_pixelDensity);
		
		CGPoint location = [touch locationInView: thisView];
		const gs2d::math::Vector2 locationV2(gs2d::math::Vector2(location.x, location.y) * m_pixelDensity);
		for (std::size_t t = 0; t < g_input->GetMaxTouchCount(); t++)
		{
			if (input->GetTouchPos(t, g_video) == prevLocationV2 || input->GetTouchPos(t, g_video) == locationV2)
			{
				CGPoint location = [touch locationInView: thisView];
				input->SetCurrentTouchPos(t, gs2d::math::Vector2(location.x, location.y) * m_pixelDensity);
				break;
			}
		}
	}
}

void ApplicationWrapper::TouchesEnded(UIView* thisView, NSSet* touches, UIEvent* event)
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(g_input.get());
	for (UITouch *touch in touches)
	{
		CGPoint prevLocation = [touch previousLocationInView: thisView];
		const gs2d::math::Vector2 prevLocationV2(gs2d::math::Vector2(prevLocation.x, prevLocation.y) * m_pixelDensity);
		
		CGPoint location = [touch locationInView: thisView];
		const gs2d::math::Vector2 locationV2(gs2d::math::Vector2(location.x, location.y) * m_pixelDensity);
		
		for (std::size_t t = 0; t < g_input->GetMaxTouchCount(); t++)
		{
			if (input->GetTouchPos(t, g_video) == prevLocationV2 || input->GetTouchPos(t, g_video) == locationV2)
			{
				input->SetCurrentTouchPos(t, gs2d::GS_NO_TOUCH);
				break;
			}
		}
	}
}

void ApplicationWrapper::TouchesCancelled(NSSet* touches, UIEvent* event)
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(g_input.get());
	for (std::size_t t = 0; t < g_input->GetMaxTouchCount(); t++)
	{
		input->SetCurrentTouchPos(t, gs2d::GS_NO_TOUCH);
	}
}
