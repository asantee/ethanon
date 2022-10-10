#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#import "AppleApplication.h"

@interface GameViewController : UIViewController

@property (strong, nonatomic) AppleApplication *ethanonApplication;

@property (nonatomic, strong) id connectObserver;
@property (nonatomic, strong) id disconnectObserver;
@property (nonatomic, strong) id pauseToggleObserver;

@property (strong, nonatomic) NSMutableArray* touches;
@property (strong, nonatomic) NSLock* arrayLock;

@end
