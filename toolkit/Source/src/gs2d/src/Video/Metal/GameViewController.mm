#import "GameViewController.h"

#import "../../Video/Metal/MetalVideo.h"

#import "../../Platform/StdFileManager.h"

#import "../../Video/Metal/MetalViewDelegate.h"

#import "../../Audio/AudioDummy.h"

#import "../../Input/iOS/IOSInput.h"

#import <GameKit/GameKit.h>

@implementation GameViewController
{
	boost::shared_ptr<gs2d::Video> m_video;
	boost::shared_ptr<gs2d::Audio> m_audio;
	boost::shared_ptr<gs2d::Input> m_input;
	
	MTKView *_view;
	
	MetalViewDelegate *_renderer;

	float m_pixelDensity;
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	_view = (MTKView *)self.view;

	// setup view properties
	m_pixelDensity = _view.contentScaleFactor;

	_view.userInteractionEnabled = YES;
	_view.multipleTouchEnabled = YES;
	_view.exclusiveTouch = YES;

	self.navigationController.interactivePopGestureRecognizer.delaysTouchesBegan = FALSE;

	// create video device
	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, "data/");

	m_video = gs2d::VideoPtr(new gs2d::MetalVideo(fileIOHub, _view));
	m_video->SetBackgroundColor(gs2d::Color(0xFF000000));

	gs2d::MetalVideo* metalVideo = (gs2d::MetalVideo*)m_video.get();

	_view.device = metalVideo->GetDevice();

	if(!_view.device)
	{
		NSLog(@"Metal is not supported on this device");
		self.view = [[UIView alloc] initWithFrame:self.view.frame];
		return;
	}

	// create IO devices
	m_audio = gs2d::AudioDummyPtr(new gs2d::AudioDummy()); //gs2d::CreateAudio(0);
	m_input = gs2d::CreateInput(false);

	// in case the fmod Audio object creationg should fail for reasons yet to be investigated,
	// try using dummy audio objects to prevent app crashes
	if (!m_audio)
	{
		m_audio = gs2d::AudioDummyPtr(new gs2d::AudioDummy());
	}
	
	self.ethanonApplication = [[AppleApplication alloc] initWithVideo:m_video audio:m_audio input:m_input];

	// setup renderer
	_renderer = [[MetalViewDelegate alloc] initWithMetalVideo:metalVideo fileIOHub:fileIOHub application:self.ethanonApplication];

	[_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

	_view.delegate = _renderer;
}

- (void)viewDidAppear:(BOOL)animated
{
	[super viewDidAppear:animated];

	m_pixelDensity = [_view contentScaleFactor];

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

- (UIRectEdge)preferredScreenEdgesDeferringSystemGestures
{
	NSLog(@"Preferred edge set");
	return UIRectEdgeBottom | UIRectEdgeLeft | UIRectEdgeRight;
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

- (void)dealloc
{
	[self shutDownEngine];

	[[NSNotificationCenter defaultCenter] removeObserver:self.connectObserver];
	[[NSNotificationCenter defaultCenter] removeObserver:self.disconnectObserver];
	[[NSNotificationCenter defaultCenter] removeObserver:self.pauseToggleObserver];
}

- (BOOL)prefersStatusBarHidden {
	return YES;
}

- (void)shutDownEngine
{
	[self.ethanonApplication destroy];
}

- (void)touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
	[self.arrayLock lock];
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());

	for (UITouch *touch in touches)
	{
		unsigned int touchIdx = 0;
		bool added = false;

		NSUInteger count = [self.touches count];
		for (NSUInteger ui = 0; ui < count; ui++)
		{
			id storedTouch = [self.touches objectAtIndex:ui];
			if (storedTouch == nil || storedTouch == [NSNull null])
			{
				touchIdx = static_cast<unsigned int>(ui);
				[self.touches setObject:touch atIndexedSubscript:ui];
				added = true;
				break;
			}
		}

		if (!added)
		{
			touchIdx = static_cast<unsigned int>(count);
			[self.touches addObject:touch];
			added = true;
		}

		CGPoint location = [touch locationInView:self.view];
		input->SetCurrentTouchPos(touchIdx++, gs2d::math::Vector2(location.x, location.y) * m_pixelDensity);
	}
	[self.arrayLock unlock];
}

- (void)touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
	[self.arrayLock lock];
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	for (UITouch *touch in touches)
	{
		NSUInteger touchIdx = [self.touches indexOfObject:touch];

		if (touchIdx != NSNotFound)
		{
			CGPoint location = [touch locationInView:self.view];
			input->SetCurrentTouchPos(
				static_cast<unsigned int>(touchIdx),
				gs2d::math::Vector2(location.x, location.y) * m_pixelDensity);
		}
		else
		{
			NSLog(@"TouchMoved touch not found!");
		}
	}
	[self.arrayLock unlock];
}

- (void)touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
	[self.arrayLock lock];
	gs2d::IOSInput* input = static_cast<gs2d::IOSInput*>(m_input.get());
	for (UITouch *touch in touches)
	{
		NSUInteger touchIdx = [self.touches indexOfObject:touch];
		if (touchIdx != NSNotFound)
			[self.touches setObject:[NSNull null] atIndexedSubscript:touchIdx];
		if (touchIdx != NSNotFound)
		{
			input->SetCurrentTouchPos(static_cast<unsigned int>(touchIdx), gs2d::GS_NO_TOUCH);
		}
		else
		{
			NSLog(@"TouchEnded touch not found!");
		}
	}
	[self.arrayLock unlock];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touchesEnded:touches withEvent:event];
}

@end
