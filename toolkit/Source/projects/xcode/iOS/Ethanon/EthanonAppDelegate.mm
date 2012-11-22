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

#import "EthanonAppDelegate.h"

@implementation EthanonAppDelegate

- (void) applicationDidFinishLaunching: (UIApplication*) application
{
	m_viewController = nil;
	CGRect screenBounds = [[UIScreen mainScreen] bounds];
	m_window = [[UIWindow alloc] initWithFrame: screenBounds];
	
	m_viewController = [[GS2DViewController alloc] init];
	[m_window setRootViewController:m_viewController];
	
	[m_window makeKeyAndVisible];
}

- (void) applicationDidEnterBackground:(UIApplication *)application
{
	if (m_viewController != nil)
		[m_viewController applicationDidEnterBackground:application];
}

- (void) applicationWillEnterForeground:(UIApplication *)application
{
	if (m_viewController != nil)
		[m_viewController applicationWillEnterForeground:application];
}

- (void) applicationWillResignActive:(UIApplication *)application
{
	if (m_viewController != nil)
		[m_viewController applicationWillResignActive:application];
}

- (void) applicationDidBecomeActive:(UIApplication *)application
{
	if (m_viewController != nil)
		[m_viewController applicationDidBecomeActive:application];
}

- (void) dealloc
{
	[m_viewController release];
    [m_window release];
    [super dealloc];
}

@end
