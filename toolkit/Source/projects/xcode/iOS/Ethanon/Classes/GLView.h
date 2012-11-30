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

#import <QuartzCore/QuartzCore.h>

#import <Video.h>
#import <Input.h>
#import <Audio.h>
#import <Platform/NativeCommandListener.h>

#import <engine/ETHEngine.h>

@interface GLView : UIView
{
@private
    EAGLContext* m_context;
	gs2d::VideoPtr m_video;
	gs2d::InputPtr m_input;
	gs2d::AudioPtr m_audio;
	UIAccelerometer* m_accelerometer;
	gs2d::BaseApplicationPtr m_engine;
	float m_density;
	bool m_mayRender;
	std::vector<Platform::NativeCommandListenerPtr> m_commandListeners;
}

- (id) initWithFrame:(CGRect) screenBounds;
- (void) renderFrame: (CADisplayLink*) displayLink;
- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (void) setAccelVectorToInput:(const gs2d::math::Vector3&) cellVector;
- (void) manageCommands;
- (void) applicationDidEnterBackground:(UIApplication*)application;
- (void) applicationWillEnterForeground:(UIApplication*)application;
- (void) applicationWillResignActive:(UIApplication*)application;
- (void) applicationDidBecomeActive:(UIApplication*)application;
- (void) insertCommandListener:(Platform::NativeCommandListenerPtr)listener;

@end
