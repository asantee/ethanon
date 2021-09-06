#import "GameViewController.h"

#include <Platform/FileIOHub.h>
#include <Platform/StdFileManager.h>

#include <Video/Metal/MetalVideo.h>

@interface MetalRenderer : NSObject <MTKViewDelegate>
-(nonnull instancetype)initWithMetalVideo:(gs2d::MetalVideo*)video fileIOHub:(Platform::FileIOHubPtr)fileIOHub;
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

	m_video = gs2d::VideoPtr(new gs2d::MetalVideo(fileIOHub, _view));
	m_video->SetBackgroundColor(gs2d::Color(0xFF003366));

	gs2d::MetalVideo* metalVideo = (gs2d::MetalVideo*)m_video.get();

	_view.device = metalVideo->GetDevice();

	if(!_view.device)
	{
		NSLog(@"Metal is not supported on this device");
		self.view = [[NSView alloc] initWithFrame:self.view.frame];
		return;
	}

	_renderer = [[MetalRenderer alloc] initWithMetalVideo:metalVideo fileIOHub:fileIOHub];

	[_renderer mtkView:_view drawableSizeWillChange:_view.bounds.size];

	_view.delegate = _renderer;
}

@end

@implementation MetalRenderer
{
	gs2d::MetalVideo* m_video;
	gs2d::PolygonRendererPtr m_polygonRenderer;
	gs2d::ShaderPtr m_shader;

	Platform::FileIOHubPtr m_fileIOHub;

	id <MTLDevice> _device;
}

-(nonnull instancetype)initWithMetalVideo:(gs2d::MetalVideo*)video fileIOHub:(Platform::FileIOHubPtr)fileIOHub;
{
	self = [super init];
	if(self)
	{
		m_video = video;
		m_fileIOHub = fileIOHub;
		_device = video->GetView().device;
		
		using namespace gs2d;
		std::vector<PolygonRenderer::Vertex> vertices =
		{
			PolygonRenderer::Vertex(math::Vector3( 0.0f, 0.0f, 0.0f), math::Vector3(1.0f), math::Vector2(0.0f, 0.0f)),
			PolygonRenderer::Vertex(math::Vector3( 0.0f, 1.0f, 0.0f), math::Vector3(1.0f), math::Vector2(0.0f, 1.0f)),
			PolygonRenderer::Vertex(math::Vector3( 1.0f, 0.0f, 0.0f), math::Vector3(1.0f), math::Vector2(1.0f, 0.0f)),
			PolygonRenderer::Vertex(math::Vector3( 1.0f, 1.0f, 0.0f), math::Vector3(1.0f), math::Vector2(1.0f, 1.0f))
		};

		std::vector<uint32_t> indices = { 0, 1, 2, 3 };

		m_polygonRenderer = m_video->CreatePolygonRenderer(vertices, indices, PolygonRenderer::TRIANGLE_STRIP);
		m_shader = m_video->LoadShaderFromFile(
			m_fileIOHub->GetResourceDirectory() + "simpleVertex.metal",
			"vertex_main",
			m_fileIOHub->GetResourceDirectory() + "simpleFragment.metal",
			"fragment_main");
	}
	return self;
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
	m_video->BeginRendering();
	
	m_polygonRenderer->BeginRendering(m_shader);
	m_polygonRenderer->Render();
	m_polygonRenderer->EndRendering();
	
	m_video->EndRendering();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
	m_video->ResetVideoMode((unsigned int)size.width, (unsigned int)size.height, gs2d::Texture::PF_DEFAULT);
}

@end
