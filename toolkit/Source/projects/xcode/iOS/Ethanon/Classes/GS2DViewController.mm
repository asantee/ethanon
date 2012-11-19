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

#import "GS2DViewController.h"
#import "GLView.h"

@implementation GS2DViewController

- (id) init
{
	m_glView = nil;
	return [super init];
}

- (void)loadView
{
	CGRect screen = [[UIScreen mainScreen] bounds];
#	ifdef GS2D_FORCE_HANDHELD_LANDSCAPE
	{
		const CGFloat temp = screen.size.width;
		screen.size.width = screen.size.height;
		screen.size.height = temp;
	}
#	endif
	m_glView = [[GLView alloc] initWithFrame:screen];
	m_glView.multipleTouchEnabled = YES;
	self.view = m_glView;
	[m_glView release];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
#	ifdef GS2D_FORCE_HANDHELD_LANDSCAPE
	{
		return interfaceOrientation == UIInterfaceOrientationLandscapeRight
			|| interfaceOrientation == UIInterfaceOrientationLandscapeLeft;
	}
#	else
	{
		return interfaceOrientation == UIInterfaceOrientationPortrait
			|| interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown;
	}
#	endif
}

- (void) configureAccelerometer
{
	const double updatesPerSecond = 60.0;
	m_accelerometer = [UIAccelerometer sharedAccelerometer];
	m_accelerometer.updateInterval = 1.0 / updatesPerSecond;
	m_accelerometer.delegate = self;
}

- (void) accelerometer: (UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
#	ifdef GS2D_FORCE_HANDHELD_LANDSCAPE
		[m_glView setAccelVectorToInput: gs2d::math::Vector3(-acceleration.y,-acceleration.x, acceleration.z)];
#	else
		[m_glView setAccelVectorToInput: gs2d::math::Vector3( acceleration.x, acceleration.y, acceleration.z)];
#	endif
}

- (void)viewDidUnload
{
	[super viewDidUnload];
}

- (void)dealloc
{
	[super dealloc];
}

- (void)viewDidLoad
{
	[super viewDidLoad];
	[self configureAccelerometer];
}

- (void) applicationDidEnterBackground:(UIApplication *)application
{
	if (m_glView != nil)
		[m_glView applicationDidEnterBackground:application];
}

- (void) applicationWillEnterForeground:(UIApplication *)application
{
	if (m_glView != nil)
		[m_glView applicationWillEnterForeground:application];
}

- (void) applicationWillResignActive:(UIApplication *)application
{
	if (m_glView != nil)
		[m_glView applicationWillResignActive:application];
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
	if (m_glView != nil)
		[m_glView applicationDidBecomeActive:application];
}

@end
