/*--------------------------------------------------------------------------------------
 Ethanon Engine (C) Copyright 2008-2013 Andre Santee
 http://ethanonengine.com/
 
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

#import "DefaultWebViewer.h"

@implementation DefaultWebViewer

@synthesize webView, activityIndicator;

- (id)initWithURL:(NSString*)url
{
	self = [super init];
	m_url = url;
	return self;
}

- (IBAction)done:(id)sender
{
	[self.view removeFromSuperview];
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
}

- (void)viewDidLoad
{
	m_timer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(webViewLoading) userInfo:nil repeats:YES];
	[webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:m_url]]];
	activityIndicator.hidesWhenStopped = YES;
	[super viewDidLoad];
}

- (void)webViewLoading
{
	if (!webView.loading)
	{
		[activityIndicator stopAnimating];
	}
	else
	{
		[activityIndicator startAnimating];
	}
}

- (void) dealloc
{
	[webView release];
	[super dealloc];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
}

/*- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	//return (interfaceOrientation == UIInterfaceOrientationPortrait);
	return YES;
}*/

@end
