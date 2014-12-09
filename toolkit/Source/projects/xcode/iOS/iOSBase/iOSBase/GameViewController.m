//
//  GameViewController.m
//  iOSBase
//
//  Created by Andre Santee on 8/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

#import "GameViewController.h"

@interface GameViewController () {

	// declare engine here

}

@property (strong, nonatomic) EAGLContext *context;

- (void)startEngine;
- (void)shutDownEngine;

@end

@implementation GameViewController

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

	// Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
	return YES;
}

- (void)startEngine
{
}

- (void)shutDownEngine
{
	[EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
}

@end
