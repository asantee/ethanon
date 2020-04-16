#ifndef GS2D_IOS_APPLICATION_H_
#define GS2D_IOS_APPLICATION_H_

#import <Foundation/Foundation.h>

#import <CoreMotion/CoreMotion.h>

#import <GLKit/GLKit.h>

#import <Platform/ios/IOSNativeCommandListener.h>

@interface ApplicationWrapper : NSObject

- (id)init;

- (void)detectJoysticks;
- (void)forceGamepadPause;
- (void)renderFrame:(GLKView*) view;
- (void)update;
- (void)destroy;

- (void)touchesBegan:(UIView*) thisView withTouches:(NSSet*) touches withEvent:(UIEvent*) event;
- (void)touchesMoved:(UIView*) thisView withTouches:(NSSet*) touches withEvent:(UIEvent*) event;
- (void)touchesEnded:(UIView*) thisView withTouches:(NSSet*) touches withEvent:(UIEvent*) event;
- (void)touchesCancelled:(UIView*) thisView withTouches:(NSSet*) touches withEvent:(UIEvent*) event;

- (void)insertCommandListener:(const Platform::NativeCommandListenerPtr) listener;

- (void)updateAccelerometer:(CMAccelerometerData*) accelerometerData;

@end

#endif
