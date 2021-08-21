#import "EthanonViewController.h"

#import "Application.h"

#import "AppDelegate.h"

#import <GameController/GameController.h>

#import "../../Input/iOS/IOSInput.h"

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

	[self.ethanonApplication detectJoysticks];

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

- (void)pressesBegan:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event
{
	BOOL didHandleEvent = false;

	for (UIPress* press in presses)
	{
		if (!press.key)
		{
			continue;
		}
		didHandleEvent = [self process:press withState:TRUE];
	}
	if (didHandleEvent == false)
	{
		[super pressesBegan:presses withEvent:event];
	}
}

- (void)pressesEnded:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event
{
	BOOL didHandleEvent = false;

	for (UIPress* press in presses)
	{
		if (!press.key)
		{
			continue;
		}
		didHandleEvent = [self process:press withState:FALSE];
	}
	if (didHandleEvent == false)
	{
		[super pressesEnded:presses withEvent:event];
	}
}

- (BOOL)process:(UIPress*) press withState:(BOOL)pressed
{
	BOOL didHandleEvent = false;
	gs2d::IOSInput* pInput = (gs2d::IOSInput*)([self.ethanonApplication input]);

    NSString* key = press.key.charactersIgnoringModifiers;
    
	// Arrow keys
	if ([key isEqualToString:UIKeyInputLeftArrow])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_LEFT, pressed);
		didHandleEvent = true;
	}
	else if ([key isEqualToString:UIKeyInputRightArrow])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_RIGHT, pressed);
		didHandleEvent = true;
	}
	else if ([key isEqualToString:UIKeyInputUpArrow])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_UP, pressed);
		didHandleEvent = true;
	}
	else if ([key isEqualToString:UIKeyInputDownArrow])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_DOWN, pressed);
		didHandleEvent = true;
	}

	// Modifier keys
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardLeftControl || press.key.keyCode == UIKeyboardHIDUsageKeyboardRightControl)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_CTRL, pressed);
    }
    else if ([key isEqualToString:@""] && press.key.modifierFlags & UIKeyModifierCommand)
    {
        pInput->SetBooleanKeyState(gs2d::GSK_CTRL, pressed);
    }
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardLeftAlt || press.key.keyCode == UIKeyboardHIDUsageKeyboardRightAlt)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_ALT, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardLeftShift || press.key.keyCode == UIKeyboardHIDUsageKeyboardRightShift)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_SHIFT, pressed);
		didHandleEvent = true;
	}

	// Utility & general keys
	else if ([key isEqualToString:UIKeyInputEscape])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_ESC, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardTab)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_TAB, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardReturnOrEnter || press.key.keyCode == UIKeyboardHIDUsageKeypadEnter)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_ENTER, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardSpacebar)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_SPACE, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardDeleteOrBackspace)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_DELETE, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardInsert)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_INSERT, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardPause)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_PAUSE, pressed);
		didHandleEvent = true;
	}

	// Navigation
	else if ([key isEqualToString:UIKeyInputPageUp])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_PAGEUP, pressed);
		didHandleEvent = true;
	}
	else if ([key isEqualToString:UIKeyInputPageDown])
	{
		pInput->SetBooleanKeyState(gs2d::GSK_PAGEDOWN, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardHome)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_HOME, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardEnd)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_END, pressed);
		didHandleEvent = true;
	}

	// F keys
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF1)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F1, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF2)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F2, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF3)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F3, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF4)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F4, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF5)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F5, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF6)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F6, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF7)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F7, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF8)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F8, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF9)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F9, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF10)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F10, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF11)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F11, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF12)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F11, pressed);
		didHandleEvent = true;
	}

	// Alphabet
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardA)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_A, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardB)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_B, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardC)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_C, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardD)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_D, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardE)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_E, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardF)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_F, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardG)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_G, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardH)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_H, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardI)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_I, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardJ)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_J, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardK)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_K, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardL)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_L, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardM)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_M, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardN)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_N, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardO)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_O, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardP)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_P, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardQ)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_Q, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardR)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_R, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardS)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_S, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardT)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_T, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardU)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_U, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardV)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_V, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardX)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_X, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardY)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_Y, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardW)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_W, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardZ)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_Z, pressed);
		didHandleEvent = true;
	}
	
	// Numeric
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard0)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_0, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard1)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_1, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard2)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_2, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard3)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_3, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard4)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_4, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard5)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_5, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard6)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_6, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard7)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_7, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard8)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_8, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboard9)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_9, pressed);
		didHandleEvent = true;
	}

	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad0)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD0, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad1)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD1, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad2)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD2, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad3)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD3, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad4)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD4, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad5)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD5, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad6)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD6, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad7)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD7, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad8)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD8, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypad9)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_NUMPAD9, pressed);
		didHandleEvent = true;
	}
	
	// Symbols
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardHyphen || press.key.keyCode == UIKeyboardHIDUsageKeypadHyphen)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_MINUS, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardComma)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_COMMA, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeyboardPeriod)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_DOT, pressed);
		didHandleEvent = true;
	}
	else if (press.key.keyCode == UIKeyboardHIDUsageKeypadPlus)
	{
		pInput->SetBooleanKeyState(gs2d::GSK_PLUS, pressed);
		didHandleEvent = true;
	}

	return didHandleEvent;
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
	if (self.ethanonApplication != nil)
	{
		[self.ethanonApplication update];
		
		if ([AppDelegate isJustResumed])
		{
			[self.ethanonApplication forceGamepadPause];
			[AppDelegate setJustResumed:NO];
		}
	}
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
