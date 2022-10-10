#ifndef GS2D_APPLE_APPLICATION_H_
#define GS2D_APPLE_APPLICATION_H_

#import <Foundation/Foundation.h>

#import "../../Video.h"
#import "../../Input.h"
#import "../../Audio.h"

@interface AppleApplication : NSObject

- (id)initWithVideo:(gs2d::VideoPtr)video audio:(gs2d::AudioPtr)audio input:(gs2d::InputPtr)input;

- (void)detectJoysticks;
- (void)forceGamepadPause;
- (void)renderFrame;
- (void)update;
- (void)restore;
- (void)destroy;

- (gs2d::Input*)input;

@end

#endif
