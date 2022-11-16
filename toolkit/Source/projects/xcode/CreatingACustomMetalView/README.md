# Creating a Custom Metal View

Implement a lightweight view for Metal rendering that's customized to your app's needs.

## Overview

While MetalKit's [`MTKView`][MTKView] provides significant functionality, allowing you to quickly get started writing Metal code, sometimes you want more control over how your Metal content is rendered.
This sample app demonstrates how to create a simple Metal view derived directly from an [`NSView`][NSView] or [`UIView`][UIView].
It uses a [`CAMetalLayer`][CAMetalLayer] object to hold the view's contents.

## Configure the Sample Code Project

This sample has targets for iOS, tvOS, and macOS.
There are significant differences between apps that use UIKit and AppKit.
Because of these differences, this sample creates two different classes. The iOS and tvOS versions of the sample use the `AAPLUIView` class, and the macOS version uses the `AAPLNSView` class.
Both are derived from a common `AAPLView` class.

This sample provides a number of options you can enable when building the app, such as whether to animate the view's contents or handle updates through system events.
You control these options by changing the preprocessor definitions in the `AAPLConfig.h` file.

``` objective-c
// When enabled, rendering occurs on the main application thread.
// This can make responding to UI events during redraw simpler
// to manage because UI calls usually must occur on the main thread.
// When disabled, rendering occurs on a background thread, allowing
// the UI to respond more quickly in some cases because events can be 
// processed asynchronously from potentially CPU-intensive rendering code.
#define RENDER_ON_MAIN_THREAD 1

// When enabled, the view continually animates and renders
// frames 60 times a second.  When disabled, rendering is event
// based, occurring when a UI event requests a redraw.
#define ANIMATION_RENDERING   1

// When enabled, the drawable's size is updated automatically whenever
// the view is resized. When disabled, you can update the drawable's
// size explicitly outside the view class.
#define AUTOMATICALLY_RESIZE  1

// When enabled, the renderer creates a depth target (i.e. depth buffer)
// and attaches with the render pass descritpr along with the drawable
// texture for rendering.  This enables the app properly perform depth testing.
#define CREATE_DEPTH_BUFFER   1
```

## Configure the View With a Metal Layer

For Metal to render to the view, the view must be backed by a [`CAMetalLayer`][CAMetalLayer].

All views in UIKit are layer backed.
To indicate the type of layer backing, the view implements the `layerClass` class method.  To indicate that your view should be backed by a `CAMetalLayer`, you must return the `CAMetalLayer` class type.

``` objective-c
+ (Class) layerClass
{
    return [CAMetalLayer class];
}
```

In AppKit, you make the view layer backed by setting the view's `wantsLayer` property.

``` objective-c
self.wantsLayer = YES;
```

This triggers a call to the view's  `makeBackingLayer` method, which returns a CAMetalLayer object.

``` objective-c
- (CALayer *)makeBackingLayer
{
    return [CAMetalLayer layer];
}
```


## Render to the View

To render to the view, create a [`MTLRenderPassDescriptor`][MTLRenderPassDescriptor] object that targets a texture provided by the layer. 
The `AAPLRenderer` class stores the render pass descriptor in the `_drawableRenderPassDescriptor` instance variable.
Most of the properties of this descriptor are set up automatically when you
initialize the renderer. The code configures the render pass to clear the contents
of the texture, and to store any rendered contents to the texture when the render
pass completes.

``` objective-c
_drawableRenderDescriptor = [MTLRenderPassDescriptor new];
_drawableRenderDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
_drawableRenderDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
_drawableRenderDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 1, 1, 1);
```

You also need to set the texture that the render pass renders into. 
Each time the app renders a frame, the renderer obtains a [`CAMetalDrawable`][CAMetalDrawable] from the Metal layer.
The drawable provides a texture for Core Animation to present onscreen.
The renderer updates the render pass descriptor to render to this texture:

``` objective-c
id<CAMetalDrawable> currentDrawable = [metalLayer nextDrawable];

// If the current drawable is nil, skip rendering this frame
if(!currentDrawable)
{
    return;
}

_drawableRenderDescriptor.colorAttachments[0].texture = currentDrawable.texture;
```

The rest of the rendering code is similar to that found in other Metal samples. For an explanation of a typical rendering path, see [Using a Render Pipeline to Render Primitives](https://developer.apple.com/documentation/metal/using_a_render_pipeline_to_render_primitives).

## Implement a Render Loop

To animate the view, the sample sets up a display link. The display link calls the view at the specified interval, synchronizing updates to the display's refresh interval. The view calls the renderer object to render a new frame of animation.

`AAPLUIView` creates a [`CADisplayLink`][CADisplayLink] in the `setupCADisplayLinkForScreen` method. 
Because you need to know which screen the window is on before creating the display link, you call this method when UIKit calls your view's [`didMoveToWindow`][didMoveToWindow] method. UIKit calls this method the first time the view is added to a window and when the view is moved to another screen. The code below stops the render loop and initializes a new display link.

``` objective-c
- (void)setupCADisplayLinkForScreen:(UIScreen*)screen
{
    [self stopRenderLoop];

    _displayLink = [screen displayLinkWithTarget:self selector:@selector(render)];

    _displayLink.paused = self.paused;

    _displayLink.preferredFramesPerSecond = 60;
}
```

`AAPLNSView` uses a [`CVDisplayLink`][CVDisplayLink] instead of a `CADisplayLink` because `CADisplayLink` is not available on macOS.
`CVDisplayLink` and `CADisplayLink` API look different, but, in principle, have the same goal, which is to allow callbacks in sync with the display.
`AAPLNSView` creates a `CVDisplayLink` in the `setupCVDisplayLinkForScreen` method.  The `setupCVDisplayLinkForScreen` method is called from [`viewDidMoveToWindow`][viewDidMoveToWindow], which AppKit calls immediately after loading the view.
If the view is moved to another screen, AppKit also calls `viewDidMoveToWindow`, and like the previous code for UIKit, the AppKit view must recreate the display link for the new screen.

``` objective-c
- (BOOL)setupCVDisplayLinkForScreen:(NSScreen*)screen
{
#if RENDER_ON_MAIN_THREAD

    // The CVDisplayLink callback, DispatchRenderLoop, never executes
    // on the main thread. To execute rendering on the main thread, create
    // a dispatch source using the main queue (the main thread).
    // DispatchRenderLoop merges this dispatch source in each call
    // to execute rendering on the main thread.
    _displaySource = dispatch_source_create(DISPATCH_SOURCE_TYPE_DATA_ADD, 0, 0, dispatch_get_main_queue());
    __weak AAPLView* weakSelf = self;
    dispatch_source_set_event_handler(_displaySource, ^(){
        @autoreleasepool
        {
            [weakSelf render];
        }
    });
    dispatch_resume(_displaySource);

#endif // END RENDER_ON_MAIN_THREAD

    CVReturn cvReturn;

    // Create a display link capable of being used with all active displays
    cvReturn = CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);

    if(cvReturn != kCVReturnSuccess)
    {
        return NO;
    }

#if RENDER_ON_MAIN_THREAD

    // Set DispatchRenderLoop as the callback function and
    // supply _displaySource as the argument to the callback.
    cvReturn = CVDisplayLinkSetOutputCallback(_displayLink, &DispatchRenderLoop, (__bridge void*)_displaySource);

#else // IF !RENDER_ON_MAIN_THREAD

    // Set DispatchRenderLoop as the callback function and
    // supply this view as the argument to the callback.
    cvReturn = CVDisplayLinkSetOutputCallback(_displayLink, &DispatchRenderLoop, (__bridge void*)self);

#endif // END !RENDER_ON_MAIN_THREAD

    if(cvReturn != kCVReturnSuccess)
    {
        return NO;
    }

    // Associate the display link with the display on which the
    // view resides
    CGDirectDisplayID viewDisplayID =
        (CGDirectDisplayID) [self.window.screen.deviceDescription[@"NSScreenNumber"] unsignedIntegerValue];;

    cvReturn = CVDisplayLinkSetCurrentCGDisplay(_displayLink, viewDisplayID);

    if(cvReturn != kCVReturnSuccess)
    {
        return NO;
    }

    CVDisplayLinkStart(_displayLink);

    NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];

    // Register to be notified when the window closes so that you
    // can stop the display link
    [notificationCenter addObserver:self
                           selector:@selector(windowWillClose:)
                               name:NSWindowWillCloseNotification
                             object:self.window];

    return YES;
}
```

The macOS version of this code performs a few additional steps. After creating the display link, it sets the callback and a parameter to pass to the callback. If you want rendering to happen on the main thread, it passes a dispatch source object; otherwise, it passes a reference to the view itself. Finally, it tells the display link which display the window is located on, and sets a notification to be called when the window is closed.

[MTKView]: https://developer.apple.com/documentation/metalkit/mtkview
[NSView]: https://developer.apple.com/documentation/appkit/nsview
[UIView]: https://developer.apple.com/documentation/uikit/uiview
[CAMetalLayer]: https://developer.apple.com/documentation/quartzcore/cametallayer
[CAMetalDrawable]: https://developer.apple.com/documentation/quartzcore/cametaldrawable
[MTLRenderPassDescriptor]: https://developer.apple.com/documentation/metal/mtlrenderpassdescriptor
[CADisplayLink]: https://developer.apple.com/documentation/quartzcore/cadisplaylink
[CVDisplayLink]: https://developer.apple.com/documentation/corevideo/cvdisplaylink-k0k
[didMoveToWindow]: https://developer.apple.com/documentation/uikit/uiview/1622527-didmovetowindow
[viewDidMoveToWindow]: https://developer.apple.com/documentation/appkit/nsview/1483329-viewdidmovetowindow
