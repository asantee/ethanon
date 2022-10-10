#import "MetalViewDelegate.h"

@implementation MetalViewDelegate
{
	gs2d::MetalVideo* m_video;
	Platform::FileIOHubPtr m_fileIOHub;
	
	AppleApplication* m_application;

	id <MTLDevice> _device;
}

-(nonnull instancetype)initWithMetalVideo:(nonnull gs2d::MetalVideo*)video fileIOHub:(Platform::FileIOHubPtr)fileIOHub application:(AppleApplication*)application
{
	self = [super init];
	if(self)
	{
		m_video = video;
		m_fileIOHub = fileIOHub;
		m_application = application;
		_device = video->GetView().device;
	}
	return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
	using namespace gs2d;
	using namespace gs2d::math;

	[m_application update];
	[m_application renderFrame];
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
	if (m_video)
	{
		m_video->ResetVideoMode((unsigned int)size.width, (unsigned int)size.height, gs2d::Texture::PF_DEFAULT);
	}
}

@end
