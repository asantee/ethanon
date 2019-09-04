#import "EthanonViewController.h"

#import "Application.h"

#import <GameController/GameController.h>

@interface EthanonViewController ()

@property (nonatomic, strong) id connectObserver;
@property (nonatomic, strong) id disconnectObserver;
@property (nonatomic, strong) id pauseToggleObserver;

@property (strong, nonatomic) CMMotionManager *motionManager;

@property (strong, nonatomic) EAGLContext *context;

- (void)shutDownEngine;
- (void)setupAccelerometer;

@end

@implementation EthanonViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
	
	self.ethanonApplication = [[ApplicationWrapper alloc] init];
	
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

	for (UIGestureRecognizer *recognizer in view.gestureRecognizers)
	{
		recognizer.delaysTouchesBegan = FALSE;
		recognizer.delaysTouchesEnded = FALSE;
		recognizer.enabled = FALSE;
	}

	UIWindow* window = view.window;
	for (UIGestureRecognizer *recognizer in window.gestureRecognizers)
	{
		recognizer.delaysTouchesBegan = FALSE;
		recognizer.delaysTouchesEnded = FALSE;
		recognizer.enabled = FALSE;
	}

	self.navigationController.interactivePopGestureRecognizer.delaysTouchesBegan = FALSE;

	[EAGLContext setCurrentContext:self.context];
}

- (UIRectEdge)preferredScreenEdgesDeferringSystemGestures
{
	NSLog(@"Preferred edge set");
    return UIRectEdgeBottom | UIRectEdgeLeft | UIRectEdgeRight;
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];


	[GCController startWirelessControllerDiscoveryWithCompletionHandler:^{
		[self.ethanonApplication detectJoysticks];
	}];

	// register gamepad listeners
	__weak typeof(self) weakself = self;
	self.connectObserver =
		[[NSNotificationCenter defaultCenter]
			addObserverForName:GCControllerDidConnectNotification
			object:nil
			queue:[NSOperationQueue mainQueue]
			usingBlock:^(NSNotification *note)
			{
				__strong typeof(self) strongSelf = weakself;
				if (strongSelf)
					[strongSelf.ethanonApplication detectJoysticks];
			}
		];

	self.disconnectObserver =
		[[NSNotificationCenter defaultCenter]
			addObserverForName:GCControllerDidDisconnectNotification
			object:nil
			queue:[NSOperationQueue mainQueue]
			usingBlock:^(NSNotification *note)
			{
				__strong typeof(self) strongSelf = weakself;
				if (strongSelf)
					[strongSelf.ethanonApplication detectJoysticks];
			}
		];

	self.pauseToggleObserver =
		[[NSNotificationCenter defaultCenter]
			addObserverForName:@"GameTogglePauseNotification"
			object:nil
			queue:[NSOperationQueue mainQueue]
			usingBlock:^(NSNotification *note)
			{
				__strong typeof(self) strongSelf = weakself;
				if (strongSelf)
					[strongSelf.ethanonApplication forceGamepadPause];
			}
		];
}

- (CGFloat)customContentScaleFactor
{
	GLKView *view = (GLKView *)self.view;
	return view.contentScaleFactor;
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
				[self.ethanonApplication updateAccelerometer:accelerometerData];
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

	[[NSNotificationCenter defaultCenter] removeObserver:self.connectObserver];
	[[NSNotificationCenter defaultCenter] removeObserver:self.disconnectObserver];
	[[NSNotificationCenter defaultCenter] removeObserver:self.pauseToggleObserver];
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
	[self.ethanonApplication destroy];
	[EAGLContext setCurrentContext:self.context];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
	[self.ethanonApplication update];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
	[self.ethanonApplication renderFrame:view];
}

- (void)touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	[self.ethanonApplication touchesBegan:self.view withTouches:touches withEvent:event];
}

- (void)touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	[self.ethanonApplication touchesMoved:self.view withTouches:touches withEvent:event];
}

- (void)touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	[self.ethanonApplication touchesEnded:self.view withTouches:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self.ethanonApplication touchesCancelled:self.view withTouches:touches withEvent:event];
}

@end
