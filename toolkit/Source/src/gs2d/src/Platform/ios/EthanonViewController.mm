//
//  EthanonViewController.m
//  iOSBase
//
//  Created by Andre Santee on 11/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#import "EthanonViewController.h"

#import "Application/Application.h"

@interface EthanonViewController () {

	ApplicationWrapper m_ethanonApplication;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)startEngine;
- (void)shutDownEngine;

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
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;

	[EAGLContext setCurrentContext:self.context];

	[self startEngine];
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

	m_ethanonApplication.Destroy();
}

- (BOOL)prefersStatusBarHidden {
	return YES;
}

- (void)startEngine
{
	m_ethanonApplication.Start();
}

- (void)shutDownEngine
{
	m_ethanonApplication.Destroy();
	[EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	m_ethanonApplication.RenderFrame();
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
	m_ethanonApplication.TouchesCancelled(touches, event);
}

@end
