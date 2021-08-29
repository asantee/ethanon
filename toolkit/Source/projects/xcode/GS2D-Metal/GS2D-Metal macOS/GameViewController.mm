#import "GameViewController.h"

#include <Platform/FileIOHub.h>
#include <Platform/StdFileManager.h>

#include <Video/Metal/MetalVideo.h>

@interface MetalRenderer : NSObject <MTKViewDelegate>
-(nonnull instancetype)initWithMetalVideo:(gs2d::MetalVideo*)video;
@end

@implementation GameViewController
{
	boost::shared_ptr<gs2d::Video> m_video;

	MTKView *_view;

	MetalRenderer *_renderer;
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	_view = (MTKView *)self.view;

	Platform::FileManagerPtr fileManager(new Platform::StdFileManager());
	Platform::FileIOHubPtr fileIOHub = Platform::CreateFileIOHub(fileManager, "fonts/");

	m_video = gs2d::MetalVideo::Create(fileIOHub, _view);
	m_video->SetBackgroundColor(gs2d::Color(0xFF003366));

	gs2d::MetalVideo* metalVideo = (gs2d::MetalVideo*)m_video.get();

	_view.device = metalVideo->GetDevice();

	if(!_view.device)
	{
		NSLog(@"Metal is not supported on this device");
		self.view = [[NSView alloc] initWithFrame:self.view.frame];
		return;
	}

	_renderer = [[MetalRenderer alloc] initWithMetalVideo:metalVideo];

	[_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

	_view.delegate = _renderer;
}

@end

@implementation MetalRenderer
{
	gs2d::MetalVideo* m_video;

	id <MTLDevice> _device;
}

-(nonnull instancetype)initWithMetalVideo:(gs2d::MetalVideo*)video;
{
	self = [super init];
	if(self)
	{
		m_video = video;
		_device = video->GetView().device;
	}
	return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
	m_video->BeginRendering();
	m_video->EndRendering();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
	m_video->ResetVideoMode((unsigned int)size.width, (unsigned int)size.height, gs2d::Texture::PF_DEFAULT);
}

@end
