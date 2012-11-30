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

#import "GLView.h"
#import <OpenGLES/ES2/gl.h>

#import <Platform/ios/Platform.ios.h>
#import <Platform/FileLogger.h>
#import <Platform/ios/IOSFileIOHub.h>

#import <Input/iOS/IOSInput.h>
#import <Audio/iOS/IOSAudio.h>

#import <Platform/ios/IOSNativeCommandListener.h>

@implementation GLView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (void) tryRetinaDisplay: (CAEAGLLayer*)layer
{
	if([[UIScreen mainScreen] respondsToSelector: NSSelectorFromString(@"scale")])
	{
		if([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")])
		{
			layer.contentsScale = self.contentScaleFactor = m_density = [[UIScreen mainScreen] scale];
		}
	}
}

- (id) initWithFrame: (CGRect)frame
{
    if (self = [super initWithFrame:frame])
	{
		CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
		[self tryRetinaDisplay:eaglLayer];
		eaglLayer.opaque = YES;

		EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
		m_context = [[EAGLContext alloc] initWithAPI:api];

		if (!m_context || ![EAGLContext setCurrentContext:m_context])
		{
			[self release];
			return nil;
		}

		GLuint renderBuffer, frameBuffer;

		glGenRenderbuffers(1, &renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);		

		[m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable: eaglLayer];

		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);

		m_audio = gs2d::CreateAudio(0);
		m_input = gs2d::CreateInput(0, false);

		Platform::FileIOHubPtr fileIOHub(new Platform::IOSFileIOHub(GS_L("data/")));

		m_video = gs2d::CreateVideo(CGRectGetWidth(frame) * m_density, CGRectGetHeight(frame) * m_density, fileIOHub);

		m_engine = gs2d::CreateBaseApplication();
		m_engine->Start(m_video, m_input, m_audio);

		[self renderFrame: nil];

		CADisplayLink* displayLink;
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderFrame:)];
		displayLink.frameInterval = 1;
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	}

	Platform::ios::StartTime::m_startTime = [[NSDate date] timeIntervalSince1970];

	[self insertCommandListener:(Platform::NativeCommandListenerPtr(new Platform::IOSNativeCommmandListener(m_video)))];

    return self;
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)orientation
{
    return NO;
}

- (void) setAccelVectorToInput:(const gs2d::math::Vector3&) accelVector
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	input->SetAccelerometerData(accelVector);
}

- (void) renderFrame:(CADisplayLink*)displayLink
{
	if (!m_mayRender)
		return;

	static bool render = false;

	#warning to-do: do update and render on different threads
	if (render)
	{
		m_engine->RenderFrame();

		if (displayLink != nil)
			[m_context presentRenderbuffer:GL_RENDERBUFFER];
	}
	else
	{
		if (m_video->HandleEvents() == gs2d::Application::APP_QUIT)
		{
			exit(0);
		}
		
		[self manageCommands];
		m_input->Update();

		static double lastTime = [[NSDate date] timeIntervalSince1970];
		const double elapsedTime = [[NSDate date] timeIntervalSince1970] - lastTime;
		lastTime = [[NSDate date] timeIntervalSince1970];

		m_engine->Update(Min(static_cast<unsigned long>(1000), static_cast<unsigned long>(elapsedTime * 1000.0)));
	}
	render = !render;
}

- (void) insertCommandListener:(Platform::NativeCommandListenerPtr)listener
{
	m_commandListeners.push_back(listener);
}

- (void) manageCommands
{
	const gs2d::str_type::string str = m_video->PullCommands();
	for (std::size_t t = 0; t < m_commandListeners.size(); t++)
	{
		m_commandListeners[t]->ParseAndExecuteCommands(str);
	}
}

- (void) applicationDidEnterBackground:(UIApplication *)application
{
	m_mayRender = false;
}

- (void) applicationWillEnterForeground:(UIApplication *)application
{
	m_mayRender = true;
}

- (void) applicationWillResignActive:(UIApplication *)application
{
	m_mayRender = false;
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
	m_mayRender = true;
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	for (UITouch *touch in touches)
	{
		gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
		for (std::size_t t = 0; t < m_input->GetMaxTouchCount(); t++)
		{
			if (input->GetTouchPos(t, m_video) == gs2d::GS_NO_TOUCH)
			{
				CGPoint location = [touch locationInView: self];
				input->SetCurrentTouchPos(t, gs2d::math::Vector2(location.x, location.y) * m_density);
				break;
			}
		}
	}
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	for (UITouch *touch in touches)
	{
		CGPoint prevLocation = [touch previousLocationInView: self];
		const gs2d::math::Vector2 prevLocationV2(gs2d::math::Vector2(prevLocation.x, prevLocation.y) * m_density);
		
		CGPoint location = [touch locationInView: self];
		const gs2d::math::Vector2 locationV2(gs2d::math::Vector2(location.x, location.y) * m_density);
		for (std::size_t t = 0; t < m_input->GetMaxTouchCount(); t++)
		{
			if (input->GetTouchPos(t, m_video) == prevLocationV2 || input->GetTouchPos(t, m_video) == locationV2)
			{
				CGPoint location = [touch locationInView: self];
				input->SetCurrentTouchPos(t, gs2d::math::Vector2(location.x, location.y) * m_density);
				break;
			}
		}
	}
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	for (UITouch *touch in touches)
	{
		CGPoint prevLocation = [touch previousLocationInView: self];
		const gs2d::math::Vector2 prevLocationV2(gs2d::math::Vector2(prevLocation.x, prevLocation.y) * m_density);
		
		CGPoint location = [touch locationInView: self];
		const gs2d::math::Vector2 locationV2(gs2d::math::Vector2(location.x, location.y) * m_density);
		
		for (std::size_t t = 0; t < m_input->GetMaxTouchCount(); t++)
		{
			if (input->GetTouchPos(t, m_video) == prevLocationV2 || input->GetTouchPos(t, m_video) == locationV2)
			{
				input->SetCurrentTouchPos(t, gs2d::GS_NO_TOUCH);
				break;
			}
		}
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	for (std::size_t t = 0; t < m_input->GetMaxTouchCount(); t++)
	{
		input->SetCurrentTouchPos(t, gs2d::GS_NO_TOUCH);
	}
}

@end
