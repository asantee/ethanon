#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "Application.h"

@interface EthanonViewController : GLKViewController

@property (strong, nonatomic) ApplicationWrapper *ethanonApplication;

- (CGFloat)customContentScaleFactor;

@end
