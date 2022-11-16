/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of the cross-platform view controller
*/

#import "AAPLViewController.h"
#if TARGET_IOS || TARGET_TVOS
#import "AAPLUIView.h"
#else
#import "AAPLNSView.h"
#endif
#import "AAPLRenderer.h"

#import <QuartzCore/CAMetalLayer.h>

@implementation AAPLViewController
{
    AAPLRenderer *_renderer;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();

    AAPLView *view = (AAPLView *)self.view;

    // Set the device for the layer so the layer can create drawable textures that can be rendered to
    // on this device.
    view.metalLayer.device = device;

    // Set this class as the delegate to receive resize and render callbacks.
    view.delegate = self;

    view.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;

    _renderer = [[AAPLRenderer alloc] initWithMetalDevice:device
                                      drawablePixelFormat:view.metalLayer.pixelFormat];
}

- (void)drawableResize:(CGSize)size
{
    [_renderer drawableResize:size];
}

- (void)renderToMetalLayer:(nonnull CAMetalLayer *)layer
{
    [_renderer renderToMetalLayer:layer];
}

@end
