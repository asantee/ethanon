//
//  EthanonViewController.m
//  iOSBase
//
//  Created by Andre Santee on 11/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#import "EthanonViewController.h"

#import "Application.h"

@interface EthanonViewController ()
{
	ApplicationWrapper m_ethanonApplication;
}

@property (strong, nonatomic) CMMotionManager *motionManager;

@property (strong, nonatomic) EAGLContext *context;

- (void)shutDownEngine;
- (void)setupAccelerometer;

@end

@implementation EthanonViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
	
	self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

	if (!self.context) {
		NSLog(@"Failed to create ES context");
	}

	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
	view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;

	view.userInteractionEnabled = YES;
	view.multipleTouchEnabled = YES;
	view.exclusiveTouch = YES;

	view.contentScaleFactor = [self customContentScaleFactor];

	[EAGLContext setCurrentContext:self.context];
}

- (CGFloat)customContentScaleFactor
{
	GLKView *view = (GLKView *)self.view;
	return view.contentScaleFactor;
}

- (CGPoint)getScreenAbsoluteSize
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

	CGPoint r;
	r.x = width  * [[UIScreen mainScreen] scale];
	r.y = height * [[UIScreen mainScreen] scale];

	return r;
}

- (void)setupAccelerometer
{
	// setup accelerometer
	self.motionManager = [[CMMotionManager alloc] init];
	self.motionManager.accelerometerUpdateInterval = .2;
	self.motionManager.gyroUpdateInterval = .2;

	[self.motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
		withHandler:^(CMAccelerometerData *accelerometerData, NSError *error)
		{
			if (error)
			{
				NSLog(@"%@", [error description]);
			}
			else
			{
				m_ethanonApplication.UpdateAccelerometer(accelerometerData);
			}
		 }
	];
}

- (void)dealloc
{	
	[self shutDownEngine];

	if ([EAGLContext currentContext] == self.context) {
		[EAGLContext setCurrentContext:nil];
	}
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	if ([self isViewLoaded] && ([[self view] window] == nil)) {
		self.view = nil;

		[self shutDownEngine];

		if ([EAGLContext currentContext] == self.context) {
			[EAGLContext setCurrentContext:nil];
		}
		self.context = nil;
	}
}

- (BOOL)prefersStatusBarHidden {
	return YES;
}

- (void)shutDownEngine
{
	m_ethanonApplication.Destroy();
	[EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
	m_ethanonApplication.Update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	m_ethanonApplication.RenderFrame(view);
}

- (void)touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	m_ethanonApplication.TouchesBegan(self.view, touches, event);
}

- (void)touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	m_ethanonApplication.TouchesMoved(self.view, touches, event);
}

- (void)touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	m_ethanonApplication.TouchesEnded(self.view, touches, event);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	m_ethanonApplication.TouchesCancelled(self.view, touches, event);
}

@end
