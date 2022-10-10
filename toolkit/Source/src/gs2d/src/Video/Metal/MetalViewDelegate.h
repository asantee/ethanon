#import <Video/Metal/MetalVideo.h>

#import <Platform/FileIOHub.h>

#import "AppleApplication.h"

@interface MetalViewDelegate : NSObject <MTKViewDelegate>
-(nonnull instancetype)initWithMetalVideo:(nonnull gs2d::MetalVideo*)video fileIOHub:(Platform::FileIOHubPtr)fileIOHub application:(nonnull AppleApplication*)application;
@end
