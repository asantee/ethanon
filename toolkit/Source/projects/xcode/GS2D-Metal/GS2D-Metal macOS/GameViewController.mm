#import "GameViewController.h"
#import "Renderer.h"

#include <Platform/FileIOHub.h>
#include <Platform/StdFileManager.h>

#include <Video/Metal/MetalVideo.h>

@implementation GameViewController
{
	boost::shared_ptr<gs2d::Video> m_video;

    MTKView *_view;

    Renderer *_renderer;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

	_view = (MTKView *)self.view;

	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, "fonts/");

	m_video = gs2d::CreateVideo(fileIOHub, _view);

	gs2d::MetalVideo* metalVideo = (gs2d::MetalVideo*)m_video.get();

	_view.device = metalVideo->GetDevice();

    if(!_view.device)
    {
        NSLog(@"Metal is not supported on this device");
        self.view = [[NSView alloc] initWithFrame:self.view.frame];
        return;
    }

    _renderer = [[Renderer alloc] initWithMetalKitView:_view];

    [_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

    _view.delegate = _renderer;
}

@end
